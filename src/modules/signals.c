#include "signals.h"

void signals_init(const struct SignalConfig *signals) {
  uint8_t i;
  for (i = 0; i < MAX_SIGNAL_PACKETS; i++) {
    if (signals->packets[i].led.port != 0) {
      // TODO: move this check into io_state
      io_set_state(&signals->packets[i].led, IO_LOW);
      io_set_dir(&signals->packets[i].led, PIN_OUT);
    }
  }
}

void signals_process(const struct SignalConfig *signals, const struct Event *e) {
  uint8_t i;
  for (i = 0; i < MAX_SIGNAL_PACKETS; i++) {
    if (signals->packets[i].e == e->id) {
      struct CANMessage msg = {
        .id = signals->packets[i].can_id,
        .data = e->data
      };

      if (signals->packets[i].led.port != 0) {
        io_set_state(&signals->packets[i].led, e->data);
      }

      can_transmit(signals->can, &msg);
      return;
    }
  }
}
