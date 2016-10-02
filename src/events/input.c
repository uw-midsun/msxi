#include "input.h"
#include "sm/event_queue.h"
#include "drivers/led.h"

typedef enum {
  POWER_CHARGE = IO_HIGH,
  POWER_RUN = IO_LOW
} PowerSelect;

void input_init(struct InputConfig *input) {
  io_set_dir(&input->power.pin, PIN_IN);
  io_set_state(&input->power_led, IO_LOW);
  io_set_dir(&input->power_led, PIN_OUT);
  io_set_dir(&input->select, PIN_IN);
  io_set_dir(&input->killswitch.pin, PIN_IN);

  // Pre-populate the power switch:
  // Don't want to count it as an event if already enabled on startup
  input->power.state = io_get_state(&input->power.pin);

  // The killswitch should always be inactive on boot.
  input->killswitch.state = IO_HIGH;
}

void input_poll(uint16_t elapsed_ms, void *context) {
  struct InputConfig *input = context;

  IOState power_state = io_get_state(&input->power.pin);
  if (power_state != input->power.state) {
    input->power.state = power_state;

    // Active-low momentary switch - only trigger on falling edge
    if (power_state == IO_LOW) {
      event_raise(POWER_TOGGLE, io_get_state(&input->select));
    }
  }

  // Active-low momentary switch - only trigger on falling edge
  IOState killswitch_state = io_get_state(&input->killswitch.pin);
  if (killswitch_state != input->killswitch.state) {
    input->killswitch.state = killswitch_state;

    if (killswitch_state == IO_LOW) {
      event_raise(EMERGENCY_STOP, 0);
    }
  }
}

// Guards
bool input_is_charge(uint64_t data) {
  return (data == POWER_CHARGE);
}

bool input_is_run(uint64_t data) {
  return (data == POWER_RUN);
}
