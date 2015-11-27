#include "peripheral.h"
#include "sm/state_machine.h"
#include "sm/sm_debug.h"
#include "sm/main.h"
#include "wdt_a.h"

int main() {
  WDT_A_hold(WDT_A_BASE);

  peripheral_init();

  sm_framework_init(sm_debug_alert);
  sm_init(main_get_sm());

  __enable_interrupt();

  struct Event e = NULL_EVENT;

  while (true) {
    e = event_get_next();
    sm_process_event(main_get_sm(), e);
  }
}
