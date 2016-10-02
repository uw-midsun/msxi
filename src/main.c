#include <msp430.h>
#include "sm/state_machine.h"
#include "modules/input.h"
#include "modules/controls.h"
#include "modules/mc_state.h"
#include "modules/signals.h"
#include "modules/display.h"
#include "drivers/timer.h"
#include "config.h"
#include "can/config.h"

static void prv_input_poll(uint16_t elapsed_ms, void *context) {
  input_poll(&input);
}

static void prv_display_update(uint16_t elapsed_ms, void *context) {
  display_update(&display);
}

int main(void) {
  WDTCTL = WDTPW | WDTHOLD; // Stop watchdog timer

  DCOCTL = CALDCO_8MHZ;
  BCSCTL1 = CALBC1_8MHZ;
  BCSCTL2 |= DIVS_3; // Divide SMCLK by 8

  sm_framework_init(NULL);

  __enable_interrupt();
  can_init(&can);
  input_init(&input);
  signals_init(&signals);
  display_init(&display);

  timer_init();

  sm_init(controls_sm());

  // Motor controllers expect a command every 250ms
  timer_delay_periodic(40, prv_input_poll, NULL);
  // Update the LCD every second at a minimum
  timer_delay_periodic(500, prv_display_update, NULL);

  struct CANMessage reset = {
    .id = THEMIS_RESET
  };

  can_transmit(&can, &reset);

  struct CANMessage bus_current = {
    .id = THEMIS_POWER,
    .data_f = {
      0.0f, // reserved
      1.0f // bus current
    }
  };

  can_transmit(&can, &bus_current);

  while (true) {
    timer_process();

    struct Event e = event_get_next();

    if (e.id != 65535) {
      mc_state_update(&mc_state, e.id);

      signals_process(&signals, &e);
      sm_process_event(controls_sm(), &e);

      //display_update(&display);
    }

    if (io_get_state(&can.interrupt_pin) == IO_LOW) {
      struct CANMessage msg;
      can_receive(&can, &msg);

      if (msg.id == PLUTUS_FAULT) {
        event_raise(PLUTUS_CAN_FAULT, msg.data);
      }
    }
  }
}

#pragma vector=PORT1_VECTOR
__interrupt void PORT1_ISR(void) {
  input_process(&input);
}

#pragma vector=PORT2_VECTOR
__interrupt void PORT2_ISR(void) {
  input_process(&input);
  if (io_process_interrupt(&can.interrupt_pin)) {
    event_raise_isr(CAN_INTERRUPT, 0);
  }
}
