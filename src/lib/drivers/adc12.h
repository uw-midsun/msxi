#pragma once
#include "io_map.h"
#include <stdint.h>

// Driver for ADC12_A

typedef enum {
  ADC12_MEM0 = 0,
  ADC12_MEM1,
  ADC12_MEM2,
  ADC12_MEM3,
  ADC12_MEM4,
  ADC12_MEM_MAX
} ADC12Index;

struct ADC12BufferConfig {
  struct IOMap pin;
  uint8_t source;
};

struct ADC12Config {
  struct ADC12BufferConfig buffers[ADC12_MEM_MAX];
  uint8_t count;
  bool continuous;
};

void adc12_init(const struct ADC12Config *adc);

uint16_t adc12_sample(const struct ADC12Config *adc, const ADC12Index pin);
