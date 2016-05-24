#include "speed.h"
#include "math.h"

// Default to operating in km/h
static SpeedUnit speed_unit = SPEED_KMPH;

void speed_set_unit(SpeedUnit unit) {
  speed_unit = unit;
}

SpeedUnit speed_get_unit() {
  return speed_unit;
}

#pragma CODE_SECTION(speed_convert, ".run_from_ram")
// Returns the provided speed (in m/s) in the specified units (rounded to the nearest integer)
uint8_t speed_convert(const float *speed_ms) {
  static const float CONVERSION[NUM_SPEED_UNITS] = {
    [SPEED_KMPH] = 3.6f,
    [SPEED_MPH] = 2.23694f
  };

  // TODO: profile fabsf(*speed_ms) -> if not too slow, use to ensure it always returns speed
  return (uint8_t)((*speed_ms * CONVERSION[speed_unit]) + 0.5f);
}

#pragma CODE_SECTION(speed_to, ".run_from_ram")
// Returns the provided speed (in the specified unit) in m/s
float speed_to(const uint8_t speed_unit) {
  static const float CONVERSION[NUM_SPEED_UNITS] = {
    [SPEED_KMPH] = 0.277778f,
    [SPEED_MPH] = 0.44704f
  };

  return speed_unit * CONVERSION[speed_unit];
}
