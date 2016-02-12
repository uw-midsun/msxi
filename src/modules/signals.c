#include "signals.h"

void signals_process(const struct SignalConfig *signals, const struct Event *e) {
  uint8_t i;
  for (i = 0; i < MAX_SIGNAL_PACKETS; i++) {
    if (signals->packets[i].e == e->id) {
      struct CANMessage msg = {
        .id = signals->packets[i].can_id,
        .data = e->data
      };

      can_transmit(signals->can, &msg);
      return;
    }
  }
}
