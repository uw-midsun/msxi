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
#include "drivers/motor_controller.h"
#include "sm/state_machine.h"
#include "precharge.h"
#include <stdio.h>

static struct State precharging_left, left_mc_enabled,
          precharging_right, right_mc_enabled;
static struct StateMachine sm = { .default_state = &precharging_left, .init = init_precharge_sm };

// safe_precharge_motor(controller) begins the precharge process, throwing PRECHARGE_FAIL
//   if any relays do not change successfully.
static void safe_precharge_motor(const struct MotorController *controller) {
  if (begin_precharge(controller)) {
    raise_event(PRECHARGE_TIMEOUT);
  } else {
    raise_event(PRECHARGE_FAIL);
  }
}

// safe_precharge_motor(controller) ends the precharge process, throwing PRECHARGE_FAIL
//   if any relays do not change successfully.
static void safe_enable_mc(const struct MotorController *controller) {
  if (end_precharge(controller)) {
    raise_event(MOTOR_CONTROLLER_ENABLED);
  } else {
    raise_event(PRECHARGE_FAIL);
  }
}

// -- Left precharge --
static void precharge_left_init() {
  safe_precharge_motor(LEFT_MOTORCONTROLLER);
}

static void left_mc_init() {
  safe_enable_mc(LEFT_MOTORCONTROLLER);
}
// -- end --

// -- Right precharge --
static void precharge_right_init() {
  safe_precharge_motor(RIGHT_MOTORCONTROLLER);
}

static void right_mc_init() {
  safe_enable_mc(RIGHT_MOTORCONTROLLER);
}
// -- end --

static void wait(struct StateMachine *sm, uint16_t seconds) {
  // TODO: add delay code
  raise_event(PRECHARGE_TIMEOUT);
}

static bool is_left_voltage_good() {
  return (get_precharge_voltage(LEFT_MOTORCONTROLLER) > SAFE_PRECHARGE_THRESHOLD);
}

static bool is_right_voltage_good() {
  return (get_precharge_voltage(RIGHT_MOTORCONTROLLER) > SAFE_PRECHARGE_THRESHOLD);
}

void init_precharge_sm() {
  // -- Set up left motor controller precharge --
  init_state(&precharging_left, precharge_left_init);
  add_transition(&precharging_left, make_data_rule(PRECHARGE_TIMEOUT, ELSE, wait, 1));
  add_guarded_state_transition(&precharging_left, PRECHARGE_TIMEOUT, is_left_voltage_good, &left_mc_enabled);

  left_mc_enabled.enter = left_mc_init;
  add_state_transition(&left_mc_enabled, MOTOR_CONTROLLER_ENABLED, &precharging_right);
  // -- end --

  // -- Set up right motor controller precharge --
  precharging_right.enter = precharge_right_init;
  add_transition(&precharging_right, make_data_rule(PRECHARGE_TIMEOUT, ELSE, wait, 1));
  add_guarded_state_transition(&precharging_right, PRECHARGE_TIMEOUT, is_right_voltage_good, &right_mc_enabled);

  right_mc_enabled.enter = right_mc_init;
  add_event_rule(&right_mc_enabled, MOTOR_CONTROLLER_ENABLED, PRECHARGE_COMPLETE);
  // -- end --
}

struct StateMachine *get_precharge_sm() {
  return &sm;
}
