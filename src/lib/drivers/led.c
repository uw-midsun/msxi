#include "led.h"
  
// This is basically a wrapper for IOMap.

void led_init(const struct IOMap *led) {
  led_set_state(led, LED_OFF);
  io_set_dir(led, PIN_OUT);
}

void led_set_state(const struct IOMap *led, LEDState state) {
  io_set_state(led, (IOState)state);
}

void led_toggle(const struct IOMap *led) {
  io_toggle(led);
}
