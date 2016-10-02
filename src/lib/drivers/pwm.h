#pragma once

#include "io_map.h"
#include "msp430.h"

#define SCALING_FACTOR 100

struct PWMConfig {
  struct IOMap pin_rising;
  struct IOMap pin_falling;
};


void pwm_init(const struct PWMConfig *pwm);

int32_t pwm_calculate_duty_cycle(void);

