#pragma once
#include "drivers/relay.h"

// Event-protected relay
// Functions the same as a normal relay, but raises success and failure events.

typedef enum {
  RELAY_SWITCHED,
  RELAY_FAIL
} RelayEvent;

typedef enum {
  RELAY_SOLAR,
  RELAY_BATTERY,
} RelayID;

bool protected_relay_set_state(const struct Relay *relay, const RelayState state);

bool protected_mc_
