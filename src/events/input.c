#include "input.h"
#include "sm/event_queue.h"
#include "drivers/led.h"

typedef enum {
  POWER_CHARGE = IO_LOW,
  POWER_RUN = IO_HIGH
} PowerSelect;

void input_init(const struct SwitchInput *input) {
  io_set_dir(&input->power, PIN_IN);
  led_init(&input->power_led);
  io_set_dir(&input->select, PIN_IN);
  io_set_dir(&input->killswitch, PIN_IN);

  // Active-low: High -> Low
  io_configure_interrupt(&input->power, true, EDGE_FALLING);
  io_configure_interrupt(&input->killswitch, true, EDGE_FALLING);
}

bool input_interrupt(const struct SwitchInput *input) {
  if (io_process_interrupt(&input->power)) {
    // Active-low switch - low = power on, high = power off
    EventID e = (io_get_state(&input->power) == IO_HIGH) ? POWER_OFF : POWER_ON;
    LEDState state = (io_get_state(&input->power) == IO_HIGH) ? LED_OFF : LED_ON;

    led_set_state(&input->power_led, state);

    // Raise an event with the selection switch's state as a data value
    event_raise_isr(e, io_get_state(&input->select));

    // Flip the power switch's interrupt edge so we trigger on the opposite action
    io_toggle_interrupt_edge(&input->power);
  }

  if (io_process_interrupt(&input->killswitch)) {
    event_raise_isr(EMERGENCY_STOP, 0);

    // Break immediately
    return true;
  }

  return false;
}

// Guards
bool input_is_charge(uint64_t data) {
  return (data == POWER_CHARGE);
}

bool input_is_run(uint64_t data) {
  return (data == POWER_RUN);
}
