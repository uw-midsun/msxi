#include "drivers/can.h"
#include "sm/state_machine.h"
#include "sm/sm_debug.h"
#include "config.h"
#include "wdt_a.h"

static void prv_init_peripherals(void) {
  can_init(&can);
  sm_debug_init(&sm_debug);
}

int main() {
  WDT_A_hold(WDT_A_BASE);

  prv_init_peripherals();

  sm_framework_init(sm_debug_alert);
  //sm_init(get_main_sm());

  __enable_interrupt();

  struct Event e = NULL_EVENT;

  while (true) {
    e = event_get_next();
    //sm_process_event(&sm, e);
  }
}
