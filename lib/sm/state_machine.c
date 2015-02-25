/*
  state_machine.c - Titus Chow

  This provides some useful functions for working with the state machine.

  Transition rules are held in an object pool to avoid having to deal with memory allocation.
  They act as linked lists to allow a variable number of transition rules per state.

  The pool size should be adjusted to match the total number of rules in the system.

*/
#include "state_machine.h"
#include <stdio.h>
#include <assert.h>

#define TRANSITION_POOL_SIZE 20
#define NO_ACTION (struct Action) { NULL, 0 }

struct TransitionRuleNode {
	struct TransitionRule rule;
	struct TransitionRuleNode *next_rule;
};

static struct TransitionRuleNode rule_pool[TRANSITION_POOL_SIZE] = { 0 };

// init_sm_framework() prepares the transition rule pool and global event queue.
void init_sm_framework() {
	init_event_queue();
	for (int i = 0; i < TRANSITION_POOL_SIZE; i++) {
		rule_pool[i].rule.event = NULL_EVENT;
	}
}

static struct TransitionRuleNode *get_free_rule() {
	for (int i = 0; i < TRANSITION_POOL_SIZE; i++) {
		if (rule_pool[i].rule.event == NULL_EVENT) {
			return &rule_pool[i];
		}
	}
	// Should never reach here - Increase pool if it does.
	assert(0 && "Transition Rule pool is too small!");
	return NULL;
}

// release_rule(rule_node) returns the node to the pool by setting it to default values.
static void release_rule(struct TransitionRuleNode *rule_node) {
	rule_node->rule.event = NULL_EVENT;
	rule_node->rule.guard = NO_GUARD;
	rule_node->rule.action = NO_ACTION;
	rule_node->next_rule = NULL;
}

// process_event(sm, e) loops through the current state's transition rules
//   until it finds a rule that matches the event's id.
// If the rule's guard is true or does not exist, then it calls the rules's
//   action with provided data.
// If nothing matches, then just ignore the event.
// This means that events will only ever call one action each.
// If the current state is composite, then events will be processed using its state machine
//   after its transition rules are checked.
void process_event(struct StateMachine *sm, Event e) {
	struct State *current_state = sm->current_state;
	struct TransitionRuleNode *rule_node = current_state->transitions;
	while (rule_node != NULL) {
		struct TransitionRule *rule = &rule_node->rule;
		if (rule->event == e &&
			(rule->guard == NO_GUARD || rule->guard())) {
			rule->action.fn(sm, rule->action.data);
			return;
		}
		rule_node = rule_node->next_rule;
	}
	if (current_state->sub_sm != NULL) {
		process_event(current_state->sub_sm, e);
	}
}

// add_transition(state, rule) adds the given rule to the struct by prepending it
//   to the state's linked list of transition rules.
// Note that we actually add the rule's data to the struct, not its pointer.
//   This is so that we don't need to initialize all the transition rules in main.
//   Instead, all rule data is held in the transition rule pool.
void add_transition(struct State *state, struct TransitionRule *rule) {
	struct TransitionRuleNode *rule_node = get_free_rule();
	rule_node->rule = *rule;
	rule_node->next_rule = state->transitions;
	state->transitions = rule_node;
}

void change_state(struct StateMachine *sm, struct State *next_state) {
	sm->current_state = next_state;
	sm->current_state->enter();
}

void raise_action_event(struct StateMachine *sm, Event e) {
	raise_event(e);
}