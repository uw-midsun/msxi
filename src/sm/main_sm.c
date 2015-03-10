#include "main_sm.h"
#include "sm/state_machine.h"
#include "states/startup.h"
#include <stdio.h>

static struct StateMachine sm = { 0 };
static struct State startup = { 0 };

void init_main_sm() {
	startup.enter = init_startup_sm;
	startup.sub_sm = get_startup_sm();

	change_state(&sm, &startup);
}

struct StateMachine *get_main_sm() {
	return &sm;
}
