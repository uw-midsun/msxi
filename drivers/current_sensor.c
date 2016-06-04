#include "msp430.h"
#include "QmathLib.h"
#include "pwm.h"
#include "current_sensor.h"


// Merge this into peripheral init
void current_sensor_init(const struct PWMConfig *pwm)
{
  pwm_init(&pwm);
}

// Call this to get the current value in cA (10^-2)
// Output is in cA due to int16 overflow size
int16_t current_sensor_read(void)
{
  volatile int16_t dc = 0;
  dc = pwm_calculate_duty_cycle();
  dc = (dc - 50 * SCALING_FACTOR) * 2;
  return dc;
}
