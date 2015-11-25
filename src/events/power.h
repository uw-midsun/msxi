#pragma once
#include "drivers/adc12.h"
#include "sm_config.h"

// DC-DC event generator

struct PowerConfig {
  const struct ADC12Config *adc;
  struct {
    uint16_t high;
    uint16_t low;
  } target;
  ADC12Index measure;
};

typedef enum {
  POWER_BAD = PROTECTED_EVENT_ID(EVENT_POWER),
  POWER_GOOD
} PowerEvent;

// Valid state transition action - blocking DC-DC status check
// Pass the PowerConfig to this action.
void power_status_check(void *dcdc);
