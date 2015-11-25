#include "heartbeat.h"
#include "sm/event_queue.h"
#include "timer_a.h"

static const struct IOMap *pin = NULL;

void heartbeat_init(const struct IOMap *heartbeat_pin) {
  pin = heartbeat_pin;
  io_set_dir(pin, PIN_IN);

  // Good heartbeat = high
  io_configure_interrupt(pin, true, EDGE_RISING);
}

void heartbeat_fire_event(void) {
  // Raise a heartbeat state event - high means good
  EventID e = (io_get_state(pin) == IO_HIGH) ? HEARBEAT_GOOD : HEARTBEAT_BAD;
  event_raise_isr(e, 0);
}


void heartbeat_interrupt(void) {
  if (io_process_interrupt(pin)) {
    heartbeat_fire_event();
    io_toggle_interrupt_edge(pin);
  }
}
