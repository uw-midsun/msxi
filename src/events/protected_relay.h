#pragma once
#include "drivers/relay.h"

// Event-protected relay
// Functions the same as a normal relay, but raises success and failure events.

typedef enum {
  RELAY_SWITCHED,
  RELAY_FAIL
} RelayEvent;

bool protected_relay_set_state(const struct Relay *relay, const RelayState state);
