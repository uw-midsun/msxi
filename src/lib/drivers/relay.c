#include "relay.h"

// TODO: find out if we need a delay between changing a relay and checking its status pin.

void relay_init(const struct Relay *relay) {
  // Assume relays are open-low and we want it to start low
  io_set_state(&relay->relay, IO_LOW);
  io_set_dir(&relay->relay, PIN_OUT);
  if (relay->status.port != NO_STATUS_PORT) {
    io_set_dir(&relay->status, PIN_IN);
  }
}

bool relay_set_state(const struct Relay *relay, const RelayState state) {
  io_set_state(&relay->relay, (state == RELAY_OPEN) ? IO_LOW : IO_HIGH);
  return (relay_get_status(relay) == state) || (relay_get_status(relay) == RELAY_UNKNOWN);
}

RelayState relay_get_status(const struct Relay *relay) {
  if (relay->status.port == NO_STATUS_PORT) {
    return RELAY_UNKNOWN;
  } else {
    return (RelayState)io_get_state(&relay->status);
  }
}
