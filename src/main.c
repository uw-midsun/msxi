#include <msp430.h>
#include "sm/state_machine.h"
#include "modules/input.h"
#include "modules/controls.h"
#include "modules/mc_state.h"
#include "modules/signals.h"
#include "config.h"
#include "can/config.h"

int main(void) {
  WDTCTL = WDTPW | WDTHOLD; // Stop watchdog timer

  __enable_interrupt();
  can_init(&can);
  input_init(&input);

  sm_framework_init(NULL);
  sm_init(controls_sm());

  while (true) {
    // TODO: Limit update rate to ~100ms?
    // TODO: Add LCD support
    input_poll(&input);

    struct Event e = event_get_next(); // TODO: add LPM support to events

    mc_state_update(&mc_state, e.id);

    signals_process(&signals, &e);
    sm_process_event(controls_sm(), &e);
  }
}

#pragma vector=PORT1_VECTOR
__interrupt void PORT1_ISR(void) {
  // TODO: Figure out brake event generation - possible for both events to happen at the same time
  input_process(&input);
}

#pragma vector=PORT2_VECTOR
__interrupt void PORT2_ISR(void) {
  input_process(&input);
  if (io_process_interrupt(&can.interrupt_pin)) {
    event_raise_isr(CAN_INTERRUPT, 0);
  }
}
