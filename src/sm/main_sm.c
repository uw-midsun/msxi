#include "main_sm.h"
#include "sm/state_machine.h"
#include "startup/startup.h"

static struct State startup;
static struct StateMachine sm = { .default_state = &startup, .init = main_sm_init };

void main_sm_init() {
  state_init_composite(&startup, startup_sm_get_info());
  // test loop: have we fixed multiple initializations?
  state_add_state_transition(&startup, STARTUP_COMPLETE, &startup);
}

struct StateMachine *main_sm_get_info() {
  return &sm;
}
