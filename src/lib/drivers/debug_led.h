/*
  debug_led.h - Titus Chow
  
  Provides an interface for the debug LEDs.
  It is assumed that our LEDs are active-low.

*/
#ifndef DEBUG_LED_H_
#define DEBUG_LED_H_
#include "io_map.h"
#include <stdlib.h>

// All the debug leds are on Port 8.
#define DEBUG_LEDS { DEBUG_LED_PORT8 }
#define DEBUG_LED_COUNT 1

// init_leds() initalizes all LEDs
void init_leds();

// led_on(led) turns the specified led on.
// requires: init_leds() has been called.
void led_on(const struct IOMap *led);

// led_off(led) turns the specified led off.
// requires: init_leds() has been called.
void led_off(const struct IOMap *led);

// toggle_led(led) toggles the specified led.
// requires: init_leds() has been called.
void toggle_led(const struct IOMap *led);

#endif /* DEBUG_LED_H_ */
