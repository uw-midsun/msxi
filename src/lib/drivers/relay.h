/*
  relay.h - Titus Chow

  This is responsible for managing the car's relays.
  Since relays are mechanical parts, they can fail. Thus, the results of these functions must be verified when called.

  Relays have two components:
    1) The relay itself (open when pin low, closed when pin high) (required)
  2) The status pin used to verify relay state (low when relay open, high when closed) (optional)
    If the status port is 0, it is assumed that the relay does not have a status pin.

*/
#pragma once
#include "io_map.h"
#include <stdbool.h>

#define NO_STATUS 0
#define NO_STATUS_PIN (struct IOMap) { NO_STATUS, 0 }

struct Relay {
  struct IOMap relay;
  struct IOMap status;
};

typedef enum {
  OPEN,
  CLOSED,
  UNKNOWN
} RelayState;

// init_relay(relay) initializes the relay, setting pin directions.
void init_relay(const struct Relay *relay);

// open_relay(relay) opens the specified relay, verifying its state.
bool open_relay(const struct Relay *relay);

// close_relay(relay) closes the specified relay, verifying its state.
bool close_relay(const struct Relay *relay);

// get_relay_status(relay) returns the state of the relay based on its status pin.
RelayState get_relay_status(const struct Relay *relay);
