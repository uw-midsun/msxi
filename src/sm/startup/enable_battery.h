#pragma once

//  This is a state machine that handles battery status and control.
//  It fires BATTERY_ENABLED when complete.

#define BATTERY_VOLTAGE PWR_STATUS

// Bounds for PWR_STATUS in mV - roughly 11V to 13V
// Values computed using LTSpice
#define DCDC_UPPER_THRESHOLD 1760
#define DCDC_LOWER_THRESHOLD 1500

// Arbitrary event id offset - not necessary, but makes debugging easier.
#define BATTERY_EVENT_OFFSET 1100

typedef enum {
  PLUTUS_ENABLED = BATTERY_EVENT_OFFSET,
  BATTERY_TIMEOUT,
  BATTERY_ENABLED,
  BATTERY_FAIL
} BatteryEvent;

void battery_sm_init();

struct StateMachine *battery_sm_get_info();
