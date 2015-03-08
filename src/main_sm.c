#include "main_sm.h"
#include "lib\sm\state_machine.h"
#include "states\precharge.h"
#include <stdio.h>

static struct StateMachine main = { 0 };
static struct State initialize = { 0 }, precharge_mcs = { 0 }, complete = { 0 };

static void init_entry() {
	raise_event(INIT_COMPLETE);
}

static void complete_init() {
	raise_event(STARTUP_COMPLETE);
}

static void raise_error(struct StateMachine *sm, uint16_t error) {

}

void init_main_sm() {
	initialize.enter = init_entry;
	add_transition(&initialize, &(struct TransitionRule)
			{ INIT_COMPLETE, NO_GUARD,
			{ .fn_pointer = change_state, .pointer = &precharge_mcs, POINTER }});

	precharge_mcs.enter = init_precharge_sm;
	precharge_mcs.sub_sm = get_precharge_sm();
	add_transition(&precharge_mcs, &(struct TransitionRule) { PRECHARGE_COMPLETE, NO_GUARD,
		{ .fn_pointer = change_state, .pointer = &complete, POINTER }});
	add_transition(&precharge_mcs, &(struct TransitionRule) { PRECHARGE_FAIL, NO_GUARD,
		{ .fn_data = raise_error, .data = 1, DATA }});
	
	complete.enter = complete_init;

	change_state(&main, &initialize);
}

struct StateMachine *get_main_sm() {
	return &main;
}
