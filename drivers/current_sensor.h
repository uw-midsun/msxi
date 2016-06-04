#pragma once

#include "msp430.h"
#include "pwm.h"

void current_sensor_init(const struct PWMConfig *pwm);

// Returns the amperage from the current sensor
int16_t current_sensor_read(void);
