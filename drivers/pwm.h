#pragma once

#include "io_map.h"
#include "msp430.h"

struct PWMConfig {
  struct IOMap pin_rising;
  struct IOMap pin_falling;
};


void pwm_init(const struct PWMConfig *pwm);

double pwm_calculate_duty_cycle(void);

