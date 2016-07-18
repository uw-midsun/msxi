#include "heartbeat.h"
#include "sm/event_queue.h"
#include "timer_a.h"

static const struct IOMap *pin = NULL;

void heartbeat_init(const struct IOMap *heartbeat_pin) {
  pin = heartbeat_pin;
  io_set_resistor_dir(pin, PIN_IN, RESISTOR_PULLDOWN);

  // Good heartbeat = high
  io_configure_interrupt(pin, true, EDGE_RISING);
}

void heartbeat_timer_cb(uint16_t elapsed_ms, void *context) {
  // Raise a heartbeat state event - high means good
  EventID e = (io_get_state(pin) == IO_HIGH) ? HEARBEAT_GOOD : HEARTBEAT_BAD;
  event_raise_isr(HEARBEAT_GOOD, 0);
}


void heartbeat_interrupt(void) {
  if (io_process_interrupt(pin)) {
    heartbeat_timer_cb(0, NULL);
    io_toggle_interrupt_edge(pin);
  }
}
