#include "debug_led.h"
  
// This is basically a wrapper for IOMap.

void led_init() {
  struct IOMap debug_leds[DEBUG_LED_COUNT] = DEBUG_LEDS;
  int i;
  for(i = 0; i < DEBUG_LED_COUNT; i++) {
    io_set_state(&debug_leds[i], IO_LOW);
    io_set_dir(&debug_leds[i], PIN_OUT);
  }
}

void led_on(const struct IOMap *led) {
  io_set_state(led, IO_LOW);
}

void led_off(const struct IOMap *led) {
  io_set_state(led, IO_HIGH);
}

void led_toggle(const struct IOMap *led) {
  io_toggle(led);
}
