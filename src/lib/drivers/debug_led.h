#pragma once
#include "io_map.h"

// Provides an interface for the debug LEDs.
// It is assumed that our LEDs are active-low.

void led_init(const struct IOMap leds[]);

void led_on(const struct IOMap *led);

void led_off(const struct IOMap *led);

void led_toggle(const struct IOMap *led);
