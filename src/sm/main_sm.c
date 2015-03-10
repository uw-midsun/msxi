#include "main_sm.h"
#include "sm/state_machine.h"
#include "states/startup.h"

static struct StateMachine sm = { 0 };
static struct State startup = { 0 };

void init_main_sm() {
	init_composite_state(&startup, init_startup_sm, get_startup_sm());

	change_state(&sm, &startup);
}

struct StateMachine *get_main_sm() {
	return &sm;
}
