/*
  state_machine.h - Titus Chow

  This is the framework for an object-oriented Moore? state machine.

  Each state has a specific action that it calls on entry. It is recommended to make this your initialization function.
    States also keep track of transition rules that are matched against incoming events in reverse order.
    (i.e. given rule1, rule2, rule3: events will be matched against rule3, then rule2, then rule1.)
    The transition rules support optional guards.
  If a transition rule is matched, it calls the appropriate action.
    Actions are provided with the state machine that called them and pre-defined data (either an integer or a pointer).

  The recommended process for creating a state machine is to encapsulate each state machine in its own source and header files.
  It should only expose its associated events and pointers to the state machine and its initialization function.
  
  When initializing a composite state, remember to add a transition rule to move to the next state on completion.
    When first entered, its state machine will be populated. Note that this is lazy evaluation.
    If re-entered, it will just switch back to its initial (default) state.
  
  Some generic actions are provided, such as changing states and raising events.

*/
#pragma once
#include "event_queue.h"
#include "transition.h"

#define NO_ENTRY_FN NULL

typedef void(*EntryFunc)();
typedef void(*InitFunc)();

struct State {
  EntryFunc enter;
  struct StateMachine *sub_sm;
  struct TransitionRule *transitions;
};

struct StateMachine {
  struct State *current_state;
  struct State *default_state;
  bool initialized;
  InitFunc init;
};

// init_sm_framework() initializes the state machine framework.
void init_sm_framework();

// init_sm(sm) initializes a state machine, populating it and then switching it to its default state.
// requires: init_sm_framework() has been called.
//        sm is not NULL.
void init_sm(struct StateMachine *main);

// init_state(state, entry_fn) initializes the given state, assigning its entry function.
// requires: state is not NULL.
void init_state(struct State *state, EntryFunc entry_fn);

// init_composite_state(state, init_fn, sm) initializes the given state as a composite state.
//   It also populates the sub-state machine.
// requires: sm must be initialized or init_fn must be sm's initialization function.
//           state is not NULL.
void init_composite_state(struct State *state, struct StateMachine *sm);

// process_event(sm, e) processes the given event using the provided state machine.
// If the current state is composite, then events are processed using its sub-state machine.
// requires: sm must be initialized and populated.
void process_event(struct StateMachine *sm, Event e);

// add_transition(state, rule) add a custom transition rule to the given state.
// requires: init_sm_framework() has been called.
//           state is not NULL.
void add_transition(struct State *state, struct TransitionRule *rule);

// add_guarded_state_transition(state, e, guard, next_state) adds a guarded change_state rule to the given state.
// requires: init_sm_framework() has been called.
//           state is not NULL.
void add_guarded_state_transition(struct State *state, Event e, Guard g, struct State *next_state);

// add_state_transition(state, e, guard, next_state) adds a guarded change_state rule to the given state.
// requires: init_sm_framework() has been called.
//           state is not NULL.
void add_state_transition(struct State *state, Event e, struct State *next_state);

// add_guarded_state_transition(state, e, guard, next_state) adds a guarded raise_action_event rule to the given state.
// requires: init_sm_framework() has been called.
//           state is not NULL.
void add_guarded_event_rule(struct State *state, Event e, Guard g, Event event);

// add_state_transition(state, e, guard, next_state) adds a raise_action_event rule to the given state.
// requires: init_sm_framework() has been called.
//           state is not NULL.
void add_event_rule(struct State *state, Event e, Event event);

// Generic actions

// change_state(sm, next_state) changes the current state to the given state,
//   calling its entry function.
// requires: init_sm_framework() has been called.
//        sm is not NULL.
void change_state(struct StateMachine *sm, void *next_state);

// raise_action_event(sm, e) raises the specified event in the global event queue.
// requires: init_sm_framework() has been called.
void raise_action_event(struct StateMachine *sm, uint16_t e);
