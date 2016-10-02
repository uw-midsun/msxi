#pragma once
#include "sm_config.h"
#include "sm/state_machine.h"
#include "drivers/can.h"

// General failure event generator
// Raises general failure events from sub-SM's for the main SM to handle
//  and broadcasts the failure on CAN.

typedef enum {
  FAILURE_OCCURRED = PROTECTED_EVENT_ID(EVENT_FAILURE)
} FailEvent;

// The CAN interface should already be initialized.
void fail_init(const struct CANConfig *can);

void fail_add_rule(struct State *state, EventID e, EventDataFunc fn);

// Failure data event handlers

void fail_handle_relay(struct StateMachine *sm, uint64_t code);

void fail_handle_killswitch(struct StateMachine *sm, uint64_t code);

void fail_handle_heartbeat(struct StateMachine *sm, uint64_t code);

void fail_handle_mc(struct StateMachine *sm, uint64_t code);

void fail_handle_dcdc(struct StateMachine *sm, uint64_t code);
