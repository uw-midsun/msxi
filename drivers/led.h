#pragma once
#include "io_map.h"

// Provides an interface for the debug LEDs.
// It is assumed that our LEDs are active-low.

typedef enum {
  LED_ON = IO_LOW,
  LED_OFF = IO_HIGH
} LEDState;

void led_init(const struct IOMap *led);

void led_set_state(const struct IOMap *led, LEDState state);

void led_toggle(const struct IOMap *led);
