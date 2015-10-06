#pragma once
#include "io_map.h"

// Provides an interface for the debug LEDs.
// It is assumed that our LEDs are active-low.

// All the debug leds are on Port 8.
#define DEBUG_LEDS { DEBUG_LED_PORT8 }
#define DEBUG_LED_COUNT 1

void led_init();

void led_on(const struct IOMap *led);

void led_off(const struct IOMap *led);

void led_toggle(const struct IOMap *led);
