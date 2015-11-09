#pragma once
#include "drivers/io_map.h"

// Switch input event generator
// All switches are wired to be active-low. Power-related switches should be latching.
// Power events are generated with the select switch's state as the data value.

struct SwitchInput {
  struct IOMap power;      // Power toggle button
  struct IOMap select;     // Running/Charging selection switch
  struct IOMap killswitch; // Killswitch
};

typedef enum {
  POWER_ON,
  POWER_OFF,
  EMERGENCY_STOP
} InputEvent;

typedef enum {
  POWER_CHARGE = IO_LOW,
  POWER_RUN = IO_HIGH
} PowerSelect;

// Initializes switches and interrupts
void input_init(const struct SwitchInput *input);

// Guards
bool input_is_charge(uint64_t data);

bool input_is_run(uint64_t data);
