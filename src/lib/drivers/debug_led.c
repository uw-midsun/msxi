/*
  debug_led.c - Titus Chow
  
  This is basically a wrapper for IOMap.

*/
#include "debug_led.h"

void init_leds() {
  struct IOMap debug_leds[DEBUG_LED_COUNT] = DEBUG_LEDS;
  int i;
  for(i = 0; i < DEBUG_LED_COUNT; i++) {
    set_io_low(&debug_leds[i]);
    set_io_dir(&debug_leds[i], PIN_OUT);
  }
}

void led_on(const struct IOMap *led) {
  set_io_low(led);
}

void led_off(const struct IOMap *led) {
  set_io_high(led);
}

void toggle_led(const struct IOMap *led) {
  toggle_io(led);
}
