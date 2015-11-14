#include "debug_led.h"
  
// This is basically a wrapper for IOMap.

void led_init(const struct IOMap leds[], uint8_t num) {
  uint8_t i;
  for(i = 0; i < num; i++) {
    io_set_state(&leds[i], IO_HIGH);
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
