#include <msp430.h>
#include "sm/state_machine.h"
#include "modules/input.h"
#include "modules/controls.h"
#include "modules/mc_state.h"
#include "modules/signals.h"
#include "config.h"

int main(void) {
  WDTCTL = WDTPW | WDTHOLD; // Stop watchdog timer

  __enable_interrupt();
  input_init(&input);


  sm_framework_init(NULL);
  sm_init(controls_sm());

  struct Event e = NULL_EVENT;

  while (true) {
    // TODO: Limit update rate to ~100ms?
    // TODO: Add LCD support
    input_poll(&input);

    e = event_get_next(); // TODO: add LPM support to events
    signals_process(&signals, &e);
    sm_process_event(controls_sm(), &e);
  }
}
