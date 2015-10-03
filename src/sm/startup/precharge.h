/*
  precharge.h - Titus Chow

  This is a state machine that controls motor controller precharging.
  To use this as a composite state, create a state with
    enter -> init_precharge_sm
  sub_sm -> get_precharge_sm()
  and add a transition rule that handles PRECHARGE_COMPLETE to escape the state machine.

*/
#pragma once

// 1180mV -> 90~95% of voltage difference
// Based on calculation: 120V -> 9mV:13.5mV -> ~1250mV
#define SAFE_PRECHARGE_THRESHOLD 1180

// Arbitrary event id offset - not necessary, but makes debugging easier.
#define PRECHARGE_EVENT_OFFSET 1200

typedef enum {
  PRECHARGE_TIMEOUT = PRECHARGE_EVENT_OFFSET,
  MOTOR_CONTROLLER_ENABLED,
  PRECHARGE_COMPLETE,
  PRECHARGE_FAIL
} PrechargeEvent;

// init_precharge_sm() sets up the precharge state machine.
void init_precharge_sm();

// get_precharge_sm() returns a pointer to the state machine for use in event handling.
struct StateMachine *get_precharge_sm();
