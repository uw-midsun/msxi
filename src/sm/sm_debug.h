#pragma once
#include "drivers/led.h"
#include "drivers/can.h"
#include "sm/state_machine.h"

// State Machine debug messages
// Changes debug LEDs and sends a CAN message

struct SMDebugConfig {
  const struct CANConfig *can;
  struct IOMap state_leds[10]; // LEDs should be provided LSB first
  struct IOMap event_leds[5];
  struct IOMap fault_leds[5];
};

// Initializes the LEDs. The CAN interface should already be initialized.
void sm_debug_init(const struct SMDebugConfig *debug);

// Initialize the state machine framework with this debug function
void sm_debug_alert(const struct StateMachine *sm, const struct Event *e);
