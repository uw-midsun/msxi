#pragma once
#include "drivers/io_map.h"
#include "sm_config.h"

// Switch input event generator
// All switches are wired to be active-low. Power-related switches should be latching.
// Power events are generated with the select switch's state as the data value.

struct SwitchInput {
  struct IOMap pin;
  IOState state;
};

struct InputConfig {
  struct SwitchInput power;       // Power toggle button
  struct SwitchInput killswitch;  // Killswitch
  struct IOMap select;            // Running/Charging selection switch
  struct IOMap power_led;         // Power button LED
};

typedef enum {
  POWER_ON = PROTECTED_EVENT_ID(EVENT_INPUT),
  POWER_OFF,
  EMERGENCY_STOP
} InputEvent;

// Initializes switches.
void input_init(struct InputConfig *input);

// Polls the state of switches and fire events on changes.
void input_poll(uint16_t elapsed_ms, void *context);

// Input Guards - Use this with POWER_ON to determine which mode to enter.
bool input_is_charge(uint64_t data);

bool input_is_run(uint64_t data);
