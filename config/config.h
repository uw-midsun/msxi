#pragma once
#include "drivers/can.h"
#include "modules/input.h"
#include "modules/signals.h"
#include "modules/mc_state.h"

// Themis config

extern const struct CANConfig can;

extern struct InputConfig input;

extern const struct SignalConfig signals;

extern struct MCStateConfig mc_state;
