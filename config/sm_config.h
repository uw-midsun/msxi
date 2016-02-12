#pragma once

// State machine event config

#define SM_EVENT_OFFSET(source) ((source) << 6)

typedef enum {
  CAN_INTERRUPT = 1,
  POLLED_INPUT,
  ISR_INPUT,
  SPECIAL_INPUT,
} EventSource;
