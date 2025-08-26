#ifndef INTERRUPT_H
#define INTERRUPT_H

#include <gpiod.h>
#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <gpiod.h>
#include <poll.h>
#include <uv.h>

typedef void (*irq_main_cb_t)(void *user);

// Request falling-edge events. Returns FD on success, -1 on error.
int setup_interrupts(const char *chipname, unsigned int line_num);

// Start background watcher; on_main runs on Node’s main thread.
int start_irq_worker(uv_loop_t *loop, irq_main_cb_t on_main, void *user);

// Stop background watcher and close async handle (call on main thread).
void stop_irq_worker(void);

// Release GPIO line/chip (call after stopping).
void teardown_interrupts(void);

// Current IRQ burst timestamp. Returns true if a burst is active.
bool irq_current_burst(uint32_t *us_out);

#endif
