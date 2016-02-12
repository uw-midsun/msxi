#pragma once
#include <stdint.h>

// Speed conversions (NOT VELOCITY -> DOES NOT PRESERVE SIGN)

typedef enum {
  SPEED_KMPH,
  SPEED_MPH,
  NUM_SPEED_UNITS
} SpeedUnit;

// Sets the unit to do conversions in
void speed_set_unit(SpeedUnit unit);

// Returns the provided speed (in m/s) in the specified units (rounded to the nearest integer)
uint8_t speed_convert(const float *speed_ms);

// Returns the provided speed (in the specified unit) in m/s
float speed_to(const uint8_t speed_unit);
