#include "msp430.h"
#include "QmathLib.h"
#include "pwm.h"
#include "current_sensor.h"

// Merge this into peripheral init
void current_sensor_init(void)
{
  // First is for plutus, second is for launchpad
  //const struct PWMConfig pwm = {{4, 0}, {4, 1}};
  const struct PWMConfig pwm = {{1, 2}, {1, 3}};

  pwm_init(&pwm);
}

// Call this to get the current value
int16_t current_sensor_read(void)
{
  volatile int16_t dc = 0;
  dc = pwm_calculate_duty_cycle();
  dc = (dc - 50 * SCALING_FACTOR) * 2;
  return dc;
}
