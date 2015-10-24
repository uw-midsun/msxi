#include "debug_led.h"
  
// This is basically a wrapper for IOMap.

#define ARRAY_LENGTH(array) (sizeof((array))/sizeof((array)[0]))

void led_init(const struct IOMap leds[]) {
  uint16_t i, length = ARRAY_LENGTH(leds);
  for(i = 0; i < length; i++) {
    io_set_state(&leds[i], IO_LOW);
    io_set_dir(&leds[i], PIN_OUT);
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
