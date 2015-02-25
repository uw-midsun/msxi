/*
  relay.h - Titus Chow

  This is responsible for managing the car's relays.
  Since relays are mechanical parts, they can fail. Thus, the results of these functions must be verified when called.

  Relays have two components:
    1) The relay itself (open when pin low, closed when pin high) (required)
	2a) The status pin used to verify relay state (low when relay open, high when closed) (optional) 
	2b) Its index (todo: enum?) in the ADC buffer.

*/
#ifndef RELAY_H_
#define RELAY_H_
#include "io_map.h"
#include <stdbool.h>

typedef enum {
	STATUS_PIN,
	VOLTAGE_INDEX,
	NO_STATUS
} RelayType;

struct Relay {
	struct IOMap relay;
	RelayType type;
	union {
		struct IOMap status;
		int index;
	};
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

#endif