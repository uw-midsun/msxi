#include "input.h"
#include "sm/event_queue.h"
#include "drivers/led.h"

typedef enum {
  POWER_CHARGE = IO_LOW,
  POWER_RUN = IO_HIGH
} PowerSelect;

void input_init(struct InputConfig *input) {
  io_set_dir(&input->power.pin, PIN_IN);
  led_init(&input->power_led);
  io_set_dir(&input->select, PIN_IN);
  io_set_dir(&input->killswitch.pin, PIN_IN);

  // Only pre-populate the power switch - don't want to count it as an event if already enabled
  // on startup
  input->power.state = io_get_state(&input->power.pin);
}

void input_poll(uint16_t elapsed_ms, void *context) {
  struct InputConfig *input = context;

  IOState power_state = io_get_state(&input->power.pin);
  if (power_state != input->power.state) {
    input->power.state = power_state;

    // Active-low switch - low = power on, high = power off
    EventID e = (power_state == IO_HIGH) ? POWER_OFF : POWER_ON;
    LEDState led_state = (power_state == IO_HIGH) ? LED_OFF : LED_ON;

    led_set_state(&input->power_led, led_state);

    // Raise an event with the selection switch's state as a data value
    event_raise(e, io_get_state(&input->select) == IO_HIGH);
  }

  IOState killswitch_state = io_get_state(&input->killswitch.pin);
  if (killswitch_state != input->killswitch.state) {
    input->killswitch.state = killswitch_state;
    event_raise_isr(EMERGENCY_STOP, 0);
  }
}

// Guards
bool input_is_charge(uint64_t data) {
  return (data == POWER_CHARGE);
}

bool input_is_run(uint64_t data) {
  return (data == POWER_RUN);
}
