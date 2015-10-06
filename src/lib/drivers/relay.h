#pragma once
#include "io_map.h"
#include <stdbool.h>

// This is responsible for managing the car's relays.
// Since relays are mechanical parts, they can fail. Thus, the results of these functions must be verified when called.

// Relays have two components:
//  1) The relay itself (open when pin low, closed when pin high) (required)
//  2) The status pin used to verify relay state (low when relay open, high when closed) (optional)
//     If the status port is 0, it is assumed that the relay does not have a status pin.



#define NO_STATUS_PORT 0
#define NO_STATUS_PIN (struct IOMap) { NO_STATUS_PORT, 0 }

struct Relay {
  struct IOMap relay;
  struct IOMap status;
};

typedef enum {
  RELAY_OPEN,
  RELAY_CLOSED,
  RELAY_UNKNOWN
} RelayState;

void relay_init(const struct Relay *relay);

// Sets the state of the relay, then verifies it using its status pin.
// Returns true if unknown.
bool relay_set_state(const struct Relay *relay, const RelayState state);

// Returns the state of the relay based on its status pin.
RelayState relay_get_status(const struct Relay *relay);
