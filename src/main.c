#include "sm/state_machine.h"
#include "wdt_a.h"

int main() {
  WDT_A_hold(WDT_A_BASE);

  sm_framework_init();
  //sm_init(get_main_sm());

  __enable_interrupt();

  struct Event e = NULL_EVENT;

  while (true) {
    e = event_get_next();
    //sm_process_event(&sm, e);
  }
}
