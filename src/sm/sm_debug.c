#include <can/config.h>
#include "sm_debug.h"

const struct SMDebugConfig *config = NULL;
static const struct StateMachine *current_sm = NULL;

void sm_debug_init(const struct SMDebugConfig *debug) {
  config = debug;

  uint8_t i;
  for (i = 0; i < 8; i++) {
    led_init(&config->leds[i]);
  }
}

// Light up the LED row as the binary representation of the ID.
static void prv_led_byte(uint8_t id) {
  uint8_t i;
  for (i = 0; i < 8; i++) {
    // Get the value of the bit at i - if it's 0, turn off the LED otherwise turn it on.
    LEDState state = (((id >> i) & 1) == 0) ? LED_OFF : LED_ON;
    led_set_state(&config->leds[i], state);
  }
}

void sm_debug_alert(const struct StateMachine *sm) {
  prv_led_byte(sm->id);

  if (sm != current_sm) {
    current_sm = sm;

    struct CANMessage msg = {
      .id = CHAOS_STATE_CHANGE,
      .data = sm->id
    };

    can_transmit(config->can, &msg);
  }
}
