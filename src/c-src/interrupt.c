#include "interrupt.h"

#include <errno.h>
#include <pthread.h>
#include <stdatomic.h>
#include <stdio.h>
#include <sys/eventfd.h>
#include <time.h>
#include <uv.h>

struct openi2c_bno08x_ints_s {
    int fd;
    int stop_efd; // eventfd to wake blocking poll on shutdown
    struct gpiod_chip *chip;
    struct gpiod_line *line;
};
static struct openi2c_bno08x_ints_s ints_s;

// libuv dispatch to main thread
static uv_async_t irq_async;
static atomic_int pending = 0;
static pthread_t irq_thread;
static irq_main_cb_t on_main_cb = NULL;
static void *on_main_context = NULL;

// Queue of falling-edge timestamps (producer: worker, consumer: main)
#define IRQ_TS_Q_CAP 32
static struct {
    uint32_t buf[IRQ_TS_Q_CAP];
    size_t head, tail, count;
    pthread_mutex_t mu;
} tsq = {.head = 0, .tail = 0, .count = 0, .mu = PTHREAD_MUTEX_INITIALIZER};

// Current burst timestamp (set/cleared on main thread)
static atomic_uint_fast32_t current_burst_us = 0;
static atomic_bool current_burst_active = false;

static inline uint32_t ts_to_us32(const struct timespec *ts) {
    uint64_t us =
        (uint64_t)ts->tv_sec * 1000000ull + (uint64_t)ts->tv_nsec / 1000ull;
    return (uint32_t)us; // wrap to 32-bit
}

static void tsq_push(uint32_t us) {
    pthread_mutex_lock(&tsq.mu);
    if (tsq.count == IRQ_TS_Q_CAP) {
        // Drop oldest to make room
        fprintf(stderr, "irq tsq overflow\n");
        tsq.tail = (tsq.tail + 1) % IRQ_TS_Q_CAP;
        tsq.count--;
    }
    tsq.buf[tsq.head] = us;
    tsq.head = (tsq.head + 1) % IRQ_TS_Q_CAP;
    tsq.count++;
    pthread_mutex_unlock(&tsq.mu);
}

static bool tsq_pop(uint32_t *out) {
    bool ok = false;
    pthread_mutex_lock(&tsq.mu);
    if (tsq.count > 0) {
        *out = tsq.buf[tsq.tail];
        tsq.tail = (tsq.tail + 1) % IRQ_TS_Q_CAP;
        tsq.count--;
        ok = true;
    }
    pthread_mutex_unlock(&tsq.mu);
    return ok;
}

int setup_interrupts(const char *chipname, unsigned int line_num) {
    ints_s.chip = gpiod_chip_open_by_name(chipname);
    if (!ints_s.chip) {
        perror("gpiod_chip_open_by_name");
        return -1;
    }
    ints_s.line = gpiod_chip_get_line(ints_s.chip, line_num);
    if (!ints_s.line) {
        perror("gpiod_chip_get_line");
        gpiod_chip_close(ints_s.chip);
        return -1;
    }
    if (gpiod_line_request_falling_edge_events(ints_s.line, "openi2c-bno08x") <
        0) {
        perror("gpiod_line_request_falling_edge_events");
        gpiod_chip_close(ints_s.chip);
        return -1;
    }
    ints_s.fd = gpiod_line_event_get_fd(ints_s.line);
    if (ints_s.fd < 0) {
        perror("gpiod_line_event_get_fd");
        gpiod_chip_close(ints_s.chip);
        return -1;
    }
    // Initialize stop eventfd once we know we have a valid GPIO FD
    ints_s.stop_efd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if (ints_s.stop_efd < 0) {
        perror("eventfd");
        gpiod_line_release(ints_s.line);
        ints_s.line = NULL;
        gpiod_chip_close(ints_s.chip);
        ints_s.chip = NULL;
        return -1;
    }
    return ints_s.fd;
}

// Consume all queued events and enqueue timestamps for falling edges.
static struct timespec ts;
static void drain_edge_events(void) {
    struct gpiod_line_event ev;
    while (gpiod_line_event_read(ints_s.line, &ev) == 0) {
        if (ev.event_type == GPIOD_LINE_EVENT_FALLING_EDGE) {
            clock_gettime(CLOCK_MONOTONIC, &ts);
            // Use time created here instead of ev.ts, which is
            // non-monotonic and subject to leap seconds.
            tsq_push(ts_to_us32(&ts));
        }
        // ignore rising edges here; deassert is implied when service() drains
    }
}

// Runs on Node's main thread
static void irq_async_cb(uv_async_t *h) {
    (void)h;
    if (atomic_exchange(&pending, 0) == 0) return;

    uint32_t ts;
    while (tsq_pop(&ts)) {
        atomic_store(&current_burst_us, ts);
        atomic_store(&current_burst_active, true);
        if (on_main_cb) on_main_cb(on_main_context); // drain sh2_service()
        atomic_store(&current_burst_active, false); // clear after burst handled
    }
}

// Worker thread: block until GPIO edge or stop signal
static void *irq_wait_thread(void *arg) {
    (void)arg;
    struct pollfd pfds[2] = {
        {.fd = ints_s.fd, .events = POLLIN},
        {.fd = ints_s.stop_efd, .events = POLLIN},
    };

    for (;;) {
        int rc = poll(pfds, 2, -1); // fully blocking
        if (rc < 0) {
            if (errno == EINTR) continue;
            perror("poll");
            break;
        }
        if (pfds[1].revents & POLLIN) {
            // Drain stop event and exit
            uint64_t v;
            (void)read(ints_s.stop_efd, &v, sizeof(v));
            break;
        }
        if (pfds[0].revents & POLLIN) {
            // Drain GPIO events and coalesce into one main-thread wake
            drain_edge_events();
            if (atomic_exchange(&pending, 1) == 0) {
                uv_async_send(&irq_async);
            }
        }
    }
    return NULL;
}

int start_irq_worker(uv_loop_t *loop, irq_main_cb_t on_main, void *context) {
    if (!ints_s.line || ints_s.fd < 0 || ints_s.stop_efd < 0 || !loop ||
        !on_main) {
        fprintf(stderr, "start_irq_worker: invalid state/args\n");
        return -1;
    }
    on_main_cb = on_main;
    on_main_context = context;
    atomic_store(&pending, 0);

    if (uv_async_init(loop, &irq_async, irq_async_cb) != 0) {
        fprintf(stderr, "uv_async_init failed\n");
        return -1;
    }
    if (pthread_create(&irq_thread, NULL, irq_wait_thread, NULL) != 0) {
        fprintf(stderr, "pthread_create failed\n");
        uv_close((uv_handle_t *)&irq_async, NULL);
        return -1;
    }
    return 0;
}

void stop_irq_worker(void) {
    // Wake the blocking poll by writing to eventfd
    if (ints_s.stop_efd >= 0) {
        uint64_t one = 1;
        (void)write(ints_s.stop_efd, &one, sizeof(one));
    }
    pthread_join(irq_thread, NULL);
    uv_close((uv_handle_t *)&irq_async, NULL);
}

void teardown_interrupts(void) {
    if (ints_s.stop_efd >= 0) {
        close(ints_s.stop_efd);
        ints_s.stop_efd = -1;
    }
    if (ints_s.line) {
        gpiod_line_release(ints_s.line);
        ints_s.line = NULL;
    }
    if (ints_s.chip) {
        gpiod_chip_close(ints_s.chip);
        ints_s.chip = NULL;
    }
}

// HAL function read_from_i2c() calls this to get the current burst timestamp.
bool irq_current_burst(uint32_t *us_out) {
    if (!us_out) return false;
    if (atomic_load(&current_burst_active)) {
        *us_out = atomic_load(&current_burst_us);
        return true;
    }
    return false;
}
