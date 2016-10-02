#include "heartbeat.h"
#include "sm/event_queue.h"
#include "timer_a.h"

static const struct IOMap *pin = NULL;
static IOState state;

void heartbeat_init(const struct IOMap *heartbeat_pin) {
  pin = heartbeat_pin;
  io_set_resistor_dir(pin, PIN_IN, RESISTOR_PULLUP);

  state = io_get_state(pin);
}

void heartbeat_timer_cb(uint16_t elapsed_ms, void *context) {
  bool forced = context;
  IOState heartbeat_state = io_get_state(pin);

  if (heartbeat_state != state || forced) {
    // Raise a heartbeat state event - high means good
    state = heartbeat_state;
    event_raise((state == IO_LOW) ? HEARTBEAT_GOOD : HEARTBEAT_BAD, 0);
  }
}
