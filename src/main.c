#include "sm/main_sm.h"
#include "sm/state_machine.h"
#include "wdt_a.h"

int main() {
  WDT_A_hold(WDT_A_BASE);

  init_sm_framework();
  init_sm(get_main_sm());

  __enable_interrupt();

  Event e = NULL_EVENT;

  while (true) {
    e = get_next_event();
    process_event(get_main_sm(), e);
  }
}
