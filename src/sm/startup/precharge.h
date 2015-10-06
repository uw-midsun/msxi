#pragma once

// This is a state machine that controls motor controller precharging.
// It fires PRECHARGE_COMPLETE when complete.

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

void precharge_sm_init();

struct StateMachine *precharge_sm_get_info();
