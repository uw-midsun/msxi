#include "can/config.h"
#include "events/fail.h"
#include "events/input.h"
#include "sm_debug.h"

const struct SMDebugConfig *config = NULL;

void sm_debug_init(const struct SMDebugConfig *debug) {
  config = debug;

  uint8_t i;
  for (i = 0; i < 10; i++) {
    led_init(&config->state_leds[i]);
  }

  for (i = 0; i < 5; i++) {
    led_init(&config->event_leds[i]);
  }

  for (i = 0; i < 5; i++) {
    led_init(&config->fault_leds[i]);
  }
}

// Light up the LED row as the binary representation of the value.
static void prv_led_array(uint16_t val, const struct IOMap *leds, uint8_t size) {
  uint8_t i;
  for (i = 0; i < size; i++) {
    // Get the value of the bit at i - if it's 0, turn off the LED otherwise turn it on.
    const LEDState state = (((val >> i) & 1) == 0) ? LED_OFF : LED_ON;
    led_set_state(&leds[i], state);
  }
}

void sm_debug_alert(const struct StateMachine *sm, const struct Event *e) {
  prv_led_array(sm->id << 5 | sm->current_state->id, config->state_leds, 10);
  prv_led_array(((e->id >> 15) & (1 << 4)) | ((e->id >> 10) & 0xF), config->event_leds, 5);
  if (e->id == FAILURE_OCCURRED) {
    prv_led_array(e->data >> 32, config->fault_leds, 5);
  } else if (e->id == POWER_TOGGLE) {
    prv_led_array(0, config->fault_leds, 5);
  }

//  struct CANMessage msg = {
//    .id = CHAOS_STATE_CHANGE,
//    .data_u16 = {
//      sm->id,
//      sm->current_state->id,
//      e->id
//    }
//  };
//
//  can_transmit(config->can, &msg);
}
