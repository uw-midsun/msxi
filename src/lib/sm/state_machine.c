/*
  state_machine.c - Titus Chow

  This provides some useful functions for working with the state machine.

*/
#include "state_machine.h"
#include <stddef.h>

// init_sm_framework() prepares the transition rule pool and global event queue.
void init_sm_framework() {
	init_event_queue();
	init_transitions();
}

void init_state(struct State *state, EntryFunc entry_fn) {
	state->enter = entry_fn;
	state->sub_sm = NULL;
	state->transitions = NULL;
}

// init_composite_state(state, entry_fn, sm) initalizes the given state as a composite state
//   by setting sub_sm to the given state machine.
void init_composite_state(struct State *state, EntryFunc entry_fn, struct StateMachine *sm) {
	state->enter = entry_fn;
	state->sub_sm = sm;
	state->transitions = NULL;
}

// process_event(sm, e) loops through the current state's transition rules
//   until it finds a rule that matches the event's id.
// If the rule's guard is true or does not exist and the event matches,
//   it calls the rule's action with provided data.
// If nothing matches, then just ignore the event.
// This means that events will only ever call one action each.
// If the current state is composite, then events will be processed using its state machine
//   after its transition rules are checked.
void process_event(struct StateMachine *sm, Event e) {
	struct State *current_state = sm->current_state;
	process_transitions(current_state->transitions, sm, e);
	if (current_state->sub_sm != NULL) {
		process_event(current_state->sub_sm, e);
	}
}

// add_transition(state, rule) adds the given rule to the struct by prepending it
//   to the state's linked list of transition rules.
// Note that all rule data is held in the transition rule pool.
void add_transition(struct State *state, struct TransitionRule *rule) {
	state->transitions = add_rule(state->transitions, rule);
}

// add_guarded_state_transition(state, e, guard, next_state) is a wrapper for add_transition.
void add_guarded_state_transition(struct State *state, Event e, Guard guard, struct State *next_state) {
	add_transition(state, make_pointer_rule(e, guard, change_state, next_state));
}

void add_state_transition(struct State *state, Event e, struct State *next_state) {
	add_guarded_state_transition(state, e, NO_GUARD, next_state);
}

// add_guarded_event_rule(state, e, guard, event) is a wrapper for add_transition.
void add_guarded_event_rule(struct State *state, Event e, Guard guard, Event event) {
	add_transition(state, make_data_rule(e, guard, raise_action_event, event));
}

void add_event_rule(struct State *state, Event e, Event event) {
	add_guarded_event_rule(state, e, NO_GUARD, event);
}

// Default action functions

void change_state(struct StateMachine *sm, void *next_state) {
	sm->current_state = next_state;
	sm->current_state->enter();
}

void raise_action_event(struct StateMachine *sm, uint16_t e) {
	raise_event(e);
}
