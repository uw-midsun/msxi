/*
  precharge.c - Titus Chow

  This is a state machine for motor controller precharging.
  Hook PRECHARGE_COMPLETE to leave the state machine and PRECHARGE_FAIL to handle errors.

  Its control flow looks like this:
    enter() -> initialize state machine
    Precharging Left:
	  enter() -> Close precharge relay, Timeout.
	  on Timeout -> Guard:
	    If voltage is above threshold, move to Left Motor Controller Enabled.
		Else, continue waiting.
	Left Motor Controller Enabled:
	  enter() -> Close main relay, open precharge relay, move to Precharging Right.
	Precharging Right:
	  enter() -> Close precharge relay, Timeout.
	  on Timeout -> Guard:
	    If voltage is above threshold, move to Right Motor Controller Enabled.
	    Else, continue waiting.
	Right Motor Controller Enabled:
	  enter() -> Close main relay, open precharge relay, raise PRECHARGE_COMPLETE.
	On error: raise PRECHARGE_FAIL.

*/
#include "..\lib\drivers\motor_controller.h"
#include "..\lib\sm\state_machine.h"
#include "precharge.h"
#include <stdio.h>
#include <Windows.h>

static struct StateMachine sm = { 0 };
static struct State precharging_left = { 0 }, left_mc_enabled = { 0 },
					precharging_right = { 0 }, right_mc_enabled = { 0 };

// safe_precharge_motor(controller) begins the precharge process, throwing PRECHARGE_FAIL
//   if any relays do not change successfully.
static void safe_precharge_motor(struct MotorController *controller) {
	if (begin_precharge(controller)) {
		raise_event(PRECHARGE_TIMEOUT);
	} else {
		raise_event(PRECHARGE_FAIL);
	}
}

// safe_precharge_motor(controller) ends the precharge process, throwing PRECHARGE_FAIL
//   if any relays do not change successfully.
static void safe_enable_mc(struct MotorController *controller) {
	if (end_precharge(controller)) {
		raise_event(MOTOR_CONTROLLER_ENABLED);
	} else {
		raise_event(PRECHARGE_FAIL);
	}
}

// -- Left precharge --
static void precharge_left_init() {
	safe_precharge_motor(&LEFT_CONTROLLER);
	printf("Closed left precharge relay!n");
}

static void left_mc_init() {
	safe_enable_mc(&LEFT_CONTROLLER);
	printf("Opened left precharge relay & closed left main relay!\n");
}
// -- end --

// -- Right precharge --
static void precharge_right_init() {
	safe_precharge_motor(&RIGHT_CONTROLLER);
	printf("Closed right precharge relay!\n");
}

static void right_mc_init() {
	safe_enable_mc(&RIGHT_CONTROLLER);
	printf("Opened right precharge relay & closed right main relay!\n");
}
// -- end --

static void wait(struct StateMachine *sm, int seconds) {
	printf("Sleeping for %d second(s)...\n", seconds);
	//Sleep(seconds * 1000);
	raise_event(PRECHARGE_TIMEOUT);
}

static bool is_voltage_good() {
	static int voltage = 0;
	if (voltage >= 5) {
		voltage = 0;
		return true;
	} else {
		voltage++;
		return false;
	}
}

void init_precharge_sm() {
	// -- Set up left motor controller precharge --
	precharging_left.enter = precharge_left_init;
	add_transition(&precharging_left, &(struct TransitionRule) { PRECHARGE_TIMEOUT, ELSE, { wait, 1 } });
	add_transition(&precharging_left, &(struct TransitionRule) { PRECHARGE_TIMEOUT, is_voltage_good, { change_state, &left_mc_enabled } });

	left_mc_enabled.enter = left_mc_init;
	add_transition(&left_mc_enabled, &(struct TransitionRule) { MOTOR_CONTROLLER_ENABLED, NO_GUARD, { change_state, &precharging_right } });
	// -- end --

	// -- Set up right motor controller precharge --
	precharging_right.enter = precharge_right_init;
	add_transition(&precharging_right, &(struct TransitionRule) { PRECHARGE_TIMEOUT, ELSE, { wait, 1 } });
	add_transition(&precharging_right, &(struct TransitionRule) { PRECHARGE_TIMEOUT, is_voltage_good, { change_state, &right_mc_enabled } });

	right_mc_enabled.enter = right_mc_init;
	add_transition(&right_mc_enabled, &(struct TransitionRule) { MOTOR_CONTROLLER_ENABLED, NO_GUARD, { raise_action_event, PRECHARGE_COMPLETE } });
	// -- end --

	// Initial state
	change_state(&sm, &precharging_left);
}

struct StateMachine *get_precharge_sm() {
	return &sm;
}