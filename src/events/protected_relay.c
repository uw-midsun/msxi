#include "protected_relay.h"
#include "sm/event_queue.h"

bool protected_relay_set_state(const struct Relay *relay, const RelayState state) {
  bool switched = relay_set_state(relay, state);

  EventID e = (switched) ? RELAY_SWITCHED : RELAY_FAIL;
  event_raise(e, relay->id);

  return switched;
}
