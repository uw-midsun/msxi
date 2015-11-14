#pragma once
#include "drivers/io_map.h"
#include "sm_config.h"

// Switch input event generator
// All switches are wired to be active-low. Power-related switches should be latching.
// Power events are generated with the select switch's state as the data value.

struct SwitchInput {
  struct IOMap power;      // Power toggle button
  struct IOMap select;     // Running/Charging selection switch
  struct IOMap killswitch; // Killswitch
};

typedef enum {
  POWER_ON = PROTECTED_EVENT_ID(EVENT_INPUT),
  POWER_OFF,
  EMERGENCY_STOP
} InputEvent;

// Initializes switches and interrupts.
void input_init(const struct SwitchInput *input);

// Call this in the appropriate port's ISR.
void input_interrupt(void);

// Input Guards - Use this with POWER_ON to determine which mode to enter.
bool input_is_charge(uint64_t data);

bool input_is_run(uint64_t data);
