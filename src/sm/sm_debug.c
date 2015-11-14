#include "sm_debug.h"
#include "can_config.h"

const struct SMDebugConfig *config = NULL;

void sm_debug_init(const struct SMDebugConfig *debug) {
  config = debug;

  led_init(config->leds, 8);
}

// Light up the LED row as the binary representation of the ID.
static void prv_led_byte(uint8_t id) {
  uint8_t i;
  for (i = 0; i < 8; i++) {
    // Get the value of the bit at i - if it's 0, turn off the LED otherwise turn it on.
    if (((id >> i) & 1) == 0) {
      led_off(&config->leds[i]);
    } else {
      led_on(&config->leds[i]);
    }
  }
}

void sm_debug_alert(const struct StateMachine *sm) {
  prv_led_byte(sm->id);

  struct CANMessage msg = {
    .id = CHAOS_STATE_CHANGE,
    .data = sm->id
  };

  can_transmit(config->can, &msg);
}
