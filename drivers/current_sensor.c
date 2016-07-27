#include "msp430.h"
#include "pwm.h"
#include "current_sensor.h"

// Merge this into peripheral init
void current_sensor_init(const struct PWMConfig *pwm) {
  pwm_init(pwm);
}

// Call this to get the current value in cA (10^-2)
// Output is in cA due to int16 overflow size
int32_t current_sensor_read(void) {
  int32_t dc = 0;
  dc = pwm_calculate_duty_cycle();
  //int32_t cc = ((dc - 50 * SCALING_FACTOR) * 243) / 100;
  int32_t cc = 2 * dc - 10000; // dc = xx.xx
  return cc;
}
