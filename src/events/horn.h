#pragma once
#include "drivers/io_map.h"
#include "drivers/can.h"
#include "sm/state_machine.h"
#include "sm_config.h"

// CAN processor for horn messages

typedef enum {
  CAN_INTERRUPT = PROTECTED_EVENT_ID(EVENT_HORN),
} HornEvent;

// Enables the CAN interrupt. The CAN interface should already be initialized.
void horn_init(const struct CANConfig *can, const struct IOMap *horn);

// Add this as an event rule for CAN_INTERRUPT.
// It will sound the horn if a horn message is received.
void horn_process_message(struct StateMachine *sm, uint16_t ignored);

// Call this in the appropriate port's ISR.
void horn_interrupt(void);
