#include "interrupt.h"

#include <pthread.h>
#include <stdatomic.h>
#include <sys/eventfd.h>
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

static void drain_edge_events(void) {
    struct gpiod_line_event ev;
    while (gpiod_line_event_read(ints_s.line, &ev) == 0) {
        // no-op; just clear the queue
    }
}

// Runs on Node's main thread
static void irq_async_cb(uv_async_t *h) {
    (void)h;
    if (atomic_exchange(&pending, 0) == 0) return;
    if (on_main_cb) on_main_cb(on_main_context); // call your service() here
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
