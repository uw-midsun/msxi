#include "peripheral.h"
#include "sm/state_machine.h"
#include "sm/sm_debug.h"
#include "sm/main.h"
#include "drivers/timer.h"
#include "events/input.h"
#include "wdt_a.h"
#include "stack_color.h"
#include "config.h"
#include "ucs.h"
#include "pmm.h"
#include "sfr.h"
#include "can/config.h"

uint16_t status;

static void prv_set_clock(void) {
  PMM_setVCore(PMM_CORE_LEVEL_1);
  UCS_clockSignalInit(
    UCS_FLLREF,
    UCS_REFOCLK_SELECT,
    UCS_CLOCK_DIVIDER_1
  );
  UCS_clockSignalInit(
    UCS_ACLK,
    UCS_REFOCLK_SELECT,
    UCS_CLOCK_DIVIDER_1
  );
  UCS_clockSignalInit(
    UCS_SMCLK,
    UCS_DCOCLK_SELECT,
    UCS_CLOCK_DIVIDER_8
  );

  UCS_initFLLSettle(
    12000,
    366
  );

  SFR_clearInterrupt(SFR_OSCILLATOR_FAULT_INTERRUPT);
  SFR_enableInterrupt(SFR_OSCILLATOR_FAULT_INTERRUPT);
}

static void prv_blink_led(uint16_t elapsed_ms, void *context) {
  led_toggle(&sm_debug.fault_leds[4]);
}

int main() {
  WDT_A_hold(WDT_A_BASE);

  prv_set_clock();

  // stack_color();

  __enable_interrupt();
  sm_framework_init(sm_debug_alert);
  sm_init(main_get_sm());

  peripheral_init();

  timer_delay_periodic(500, prv_blink_led, NULL);

  struct Event e = NULL_EVENT;

  while (true) {
    timer_process();

    e = event_get_next();
    if (e.id != NULL_EVENT_ID) {
      sm_process_event(main_get_sm(), &e);
    }

    if (io_get_state(&can.interrupt_pin) == IO_LOW) {
      struct CANMessage msg;
      struct CANError error;
      can_process_interrupt(&can, &msg, &error);
      if (msg.id == THEMIS_HORN) {
        // We got a horn message
        // Horn messages carry a boolean value - on or off.
        IOState state = (msg.data == 1) ? IO_HIGH : IO_LOW;
        io_set_state(&horn, state);
      }
    }
  }
}

#pragma vector=UNMI_VECTOR
__interrupt void NMI_ISR(void) {
  do {
    // If it still can't clear the oscillator fault flags after the timeout,
    // trap and wait here.
    status = UCS_clearAllOscFlagsWithTimeout(1000);
  } while(status != 0);
}
