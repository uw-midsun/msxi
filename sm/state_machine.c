#include "state_machine.h"
#include <stddef.h>

static SMDebugFunc sm_debug = NULL;

void sm_framework_init(SMDebugFunc debug_fn) {
  sm_debug = debug_fn;
  event_queue_init();
  transitions_init();
}

void sm_init(struct StateMachine *sm) {
  if (!sm->initialized) {
    sm->init();
    sm->initialized = true;
  }
  sm_change_state(sm, sm->default_state);
}

void state_init(struct State *state, EntryFunc entry_fn) {
  state->enter = entry_fn;
  state->sub_sm = NULL;
  state->transitions = (struct Transitions) {
    .root = NULL,
    .last = NULL
  };
}

void state_init_composite(struct State *state, struct StateMachine *sm) {
  state->enter = NULL;
  state->sub_sm = sm;
  state->transitions = (struct Transitions) {
    .root = NULL,
    .last = NULL
  };
}

// Loops through the current state's transition rules
//   until it finds a rule that matches the event's id.
// If the rule's guard is true or does not exist and the event matches,
//   it calls the rule's action with provided data.
// If nothing matches, then just ignore the event.
// This means that events will only ever call one action each.
// If the current state is composite, then events will be processed using its state machine
//   after its transition rules are checked.
void sm_process_event(struct StateMachine *sm, struct Event *e) {
  struct State *current_state = sm->current_state;
  bool matched = transitions_process(&current_state->transitions, sm, e);
  if (matched) {
    _nop();
    if (sm_debug != NULL) {
      // Event processed - run debug function
      sm_debug(sm);
    }
  } else if (current_state->sub_sm != NULL) {
    sm_process_event(current_state->sub_sm, e);
  }
}

// Adds the given rule to the struct by prepending it to the state's list of transition rules.
// Note that all rule data is held in the transition rule pool.
void state_add_transition(struct State *state, struct TransitionRule *rule) {
  transitions_add_rule(&state->transitions, rule);
}

void state_add_guarded_state_transition(struct State *state, EventID e, Guard guard, struct State *next_state) {
  state_add_transition(state, transitions_make_pointer_rule(e, guard, sm_change_state, next_state));
}

void state_add_state_transition(struct State *state, EventID e, struct State *next_state) {
  state_add_guarded_state_transition(state, e, NO_GUARD, next_state);
}

void state_add_guarded_event_rule(struct State *state, EventID e, Guard guard, EventID event) {
  state_add_transition(state, transitions_make_data_rule(e, guard, sm_raise_event, event));
}

void state_add_event_rule(struct State *state, EventID e, EventID event) {
  state_add_guarded_event_rule(state, e, NO_GUARD, event);
}

// Default action functions

// Switches the state machine to the next state.
// If the next state is a composite state, it initializes the sub-state machine
//   and switches it to the default state.
void sm_change_state(struct StateMachine *sm, void *next_state) {
  sm->current_state = next_state;
  if(sm->current_state->sub_sm != NULL) {
    sm_init(sm->current_state->sub_sm);
  }
  if(sm->current_state->enter != NULL) {
    sm->current_state->enter();
  }
}

void sm_raise_event(struct StateMachine *sm, uint16_t e) {
  event_raise(e, 0);
}
