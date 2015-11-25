#pragma once
#include "sm/state_machine.h"
#include "sm_config.h"

// Main state machine - handles moving between general states
// Off -> Running | Charging (-> Failure) -> Off

struct StateMachine *main_get_sm(void);
