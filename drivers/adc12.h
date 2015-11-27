#pragma once
#include "io_map.h"
#include "adc12_a.h"
#include <stdint.h>

// Driver for ADC12_A

typedef enum {
  PWR_STATUS = 0,
  MC_CHG_LEFT,
  MC_DCHG_LEFT,
  MC_CHG_RIGHT,
  MC_DCHG_RIGHT,
  PIN_COUNT
} ADC12Index;

struct ADC12BufferConfig {
  struct IOMap pin;
  uint8_t source;
};

struct ADC12Config {
  struct ADC12BufferConfig buffers[PIN_COUNT];
};

void adc12_init(const struct ADC12Config *adc);

uint16_t adc12_sample(const struct ADC12Config *adc, const ADC12Index pin);
