#pragma once
#include "sm/state_machine.h"
#include "sm_config.h"

// Charge SM
// Enable solar and LV power -> DCDC -> Off

typedef enum {
  CHARGING_ENABLED = STATEMACHINE_ID(SM_CHARGING),
  CHARGING_COMPLETE
} ChargeEvent;

struct StateMachine *charge_get_sm(void);

EventID charge_get_exit_event(void);
