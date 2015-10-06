#include "sm/main_sm.h"
#include "sm/state_machine.h"
#include "wdt_a.h"

int main() {
  WDT_A_hold(WDT_A_BASE);

  sm_framework_init();
  sm_init(main_sm_get_info());

  __enable_interrupt();

  Event e = NULL_EVENT;

  while (true) {
    e = get_next_event();
    sm_process_event(main_sm_get_info(), e);
  }
}
