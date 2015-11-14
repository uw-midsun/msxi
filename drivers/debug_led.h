#pragma once
#include "io_map.h"

// Provides an interface for the debug LEDs.
// It is assumed that our LEDs are active-low.

// Initialize the given array of leds
void led_init(const struct IOMap leds[], uint8_t num);

void led_on(const struct IOMap *led);

void led_off(const struct IOMap *led);

void led_toggle(const struct IOMap *led);
