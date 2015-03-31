#include "main_sm.h"
#include "sm/state_machine.h"
#include "startup/startup.h"

static struct State startup;
static struct StateMachine sm = { .default_state = &startup, .init = init_main_sm };

void init_main_sm() {
	init_composite_state(&startup, get_startup_sm());
	// test loop: have we fixed multiple initializations?
	add_state_transition(&startup, STARTUP_COMPLETE, &startup);
}

struct StateMachine *get_main_sm() {
	return &sm;
}
