/*
  precharge.h - Titus Chow

  This is a state machine that controls motor controller precharging.
  To use this as a composite state, create a state with
    enter -> init_precharge_sm
	sub_sm -> get_precharge_sm()
	and add a transition rule that handles PRECHARGE_COMPLETE to escape the state machine.

*/
#ifndef PRECHARGE_H_
#define PRECHARGE_H_

// 3100mV -> ~95% of voltage difference (assuming 1:1 ratio between 120V to 3.3V conversion)
// TODO: get actual safe voltage of circuit
#define SAFE_VOLTAGE_THRESHOLD 3100

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

#endif
