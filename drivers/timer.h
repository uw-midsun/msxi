#pragma once
#include <stdbool.h>
#include <stdint.h>

// Timer-Bx driver

// Allows time-based periodic callbacks with a millisecond resolution.
// Note that its resolution is gated by the frequency of the loop calling timer_process.
// Do not rely on this driver for accurate timeouts.

typedef void (*timer_delay_cb)(uint16_t elapsed_ms, void *context);

void timer_init();

// Call in the main loop to process any timers that have expired
void timer_process();

// Register one-shot timeout
bool timer_delay(uint16_t ms, timer_delay_cb delay_cb, void *context);

// Register periodic timeout
bool timer_delay_periodic(uint16_t ms, timer_delay_cb delay_cb, void *context);
