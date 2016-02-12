#pragma once
#include "sm/event_queue.h"
#include "drivers/can.h"

// Lights + Horn processor

#define MAX_SIGNAL_PACKETS 5

struct Signal {
  EventID e;
  uint16_t can_id;
};

struct SignalConfig {
  const struct CANConfig *can;
  struct Signal packets[MAX_SIGNAL_PACKETS];
};

// Processes the provided event and sends the appropriate CAN packets if any signals match
void signals_process(const struct SignalConfig *signals, const struct Event *e);
