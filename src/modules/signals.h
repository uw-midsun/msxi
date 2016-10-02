#pragma once
#include "sm/event_queue.h"
#include "drivers/can.h"
#include "drivers/io_map.h"

// Lights + Horn processor

#define MAX_SIGNAL_PACKETS 6

struct Signal {
  EventID e;
  uint16_t can_id;
  struct IOMap led;
};

struct SignalConfig {
  const struct CANConfig *can;
  struct Signal packets[MAX_SIGNAL_PACKETS];
};

void signals_init(const struct SignalConfig *signals);

// Processes the provided event and sends the appropriate CAN packets if any signals match
void signals_process(const struct SignalConfig *signals, const struct Event *e);
