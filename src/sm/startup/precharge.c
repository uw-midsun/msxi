#include "drivers/motor_controller.h"
#include "sm/state_machine.h"
#include "precharge.h"
#include <stdio.h>

// This is a state machine for motor controller precharging.
// Hook PRECHARGE_COMPLETE to leave the state machine and PRECHARGE_FAIL to handle errors.

// Its control flow looks like this:
//  enter() -> initialize state machine
//  Precharging Left:
//  enter() -> Close precharge relay, Timeout.
//  on Timeout -> Guard:
//    If voltage is above threshold, move to Left Motor Controller Enabled.
//  Else, continue waiting.
// Left Motor Controller Enabled:
//  enter() -> Close main relay, open precharge relay, move to Precharging Right.
// Precharging Right: same as Left, raise PRECHARGE_COMPLETE.
// On error: raise PRECHARGE_FAIL.

// TODO: make more modular
/*
static struct State precharging_left, left_mc_enabled,
              precharging_right, right_mc_enabled;
static struct StateMachine sm = { .default_state = &precharging_left, .init = precharge_sm_init };

// Begins the precharge process, throwing PRECHARGE_FAIL if any relays do not change successfully.
static void prv_safe_precharge_motor(const struct MotorController *controller) {
  if (begin_precharge(controller)) {
    event_raise(PRECHARGE_TIMEOUT);
  } else {
    event_raise(PRECHARGE_FAIL);
  }
}

// Ends the precharge process, throwing PRECHARGE_FAIL if any relays do not change successfully.
static void prv_safe_enable_mc(const struct MotorController *controller) {
  if (end_precharge(controller)) {
    event_raise(MOTOR_CONTROLLER_ENABLED);
  } else {
    event_raise(PRECHARGE_FAIL);
  }
}

// -- Left precharge --
static void prv_precharge_left_init() {
  prv_safe_precharge_motor(LEFT_MOTORCONTROLLER);
}

static void prv_left_mc_init() {
  prv_safe_enable_mc(LEFT_MOTORCONTROLLER);
}
// -- end --

// -- Right precharge --
static void prv_precharge_right_init() {
  prv_safe_precharge_motor(RIGHT_MOTORCONTROLLER);
}

static void prv_right_mc_init() {
  prv_safe_enable_mc(RIGHT_MOTORCONTROLLER);
}
// -- end --

static void prv_wait(struct StateMachine *sm, uint16_t seconds) {
  // TODO: add delay code/abstract
  event_raise(PRECHARGE_TIMEOUT);
}

static bool prv_is_left_voltage_good() {
  return (get_precharge_voltage(LEFT_MOTORCONTROLLER) > SAFE_PRECHARGE_THRESHOLD);
}

static bool prv_is_right_voltage_good() {
  return (get_precharge_voltage(RIGHT_MOTORCONTROLLER) > SAFE_PRECHARGE_THRESHOLD);
}

void precharge_sm_init() {
  // -- Set up left motor controller precharge --
  state_init(&precharging_left, prv_precharge_left_init);
  state_add_transition(&precharging_left,
                       transitions_make_data_rule(PRECHARGE_TIMEOUT, ELSE, prv_wait, 1));
  state_add_guarded_state_transition(&precharging_left, PRECHARGE_TIMEOUT,
                                     prv_is_left_voltage_good, &left_mc_enabled);

  left_mc_enabled.enter = prv_left_mc_init;
  state_add_state_transition(&left_mc_enabled, MOTOR_CONTROLLER_ENABLED, &precharging_right);
  // -- end --

  // -- Set up right motor controller precharge --
  precharging_right.enter = prv_precharge_right_init;
  state_add_transition(&precharging_right,
                       transitions_make_data_rule(PRECHARGE_TIMEOUT, ELSE, prv_wait, 1));
  state_add_guarded_state_transition(&precharging_right, PRECHARGE_TIMEOUT,
                                     prv_is_right_voltage_good, &right_mc_enabled);

  right_mc_enabled.enter = prv_right_mc_init;
  state_add_event_rule(&right_mc_enabled, MOTOR_CONTROLLER_ENABLED, PRECHARGE_COMPLETE);
  // -- end --
}*/

struct StateMachine *precharge_sm_get_info() {
  return NULL;
  //return &sm;
}
