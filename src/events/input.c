#include "input.h"
#include "sm/event_queue.h"
#include <msp430.h>

typedef enum {
  POWER_CHARGE = IO_LOW,
  POWER_RUN = IO_HIGH
} PowerSelect;

static const struct SwitchInput *switches = NULL;

void input_init(const struct SwitchInput *input) {
  switches = input;

  io_set_dir(&switches->power, PIN_IN);
  io_set_dir(&switches->select, PIN_IN);
  io_set_dir(&switches->killswitch, PIN_IN);

  // Active-low: High -> Low
  io_configure_interrupt(&switches->power, true, EDGE_FALLING);
  io_configure_interrupt(&switches->killswitch, true, EDGE_FALLING);
}

void input_interrupt(void) {
  if (io_process_interrupt(&switches->power)) {
    // Active-low switch - low = power on, high = power off
    EventID e = (io_get_state(&switches->power) == IO_HIGH) ? POWER_OFF : POWER_ON;

    // Raise an event with the selection switch's state as a data value
    event_raise_isr(e, io_get_state(&switches->select));

    // Flip the power switch's interrupt edge so we trigger on the opposite action
    io_toggle_interrupt_edge(&switches->power);
  }

  if (io_process_interrupt(&switches->killswitch)) {
    event_raise_isr(EMERGENCY_STOP, 0);
    // TODO: can we exit the interrupt?
  }
}

// Guards
bool input_is_charge(uint64_t data) {
  return (data == POWER_CHARGE);
}

bool input_is_run(uint64_t data) {
  return (data == POWER_RUN);
}
