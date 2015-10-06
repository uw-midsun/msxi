#pragma once
#include <stdint.h>

// Measures the voltage of:
// P7.7 - PWR_STATUS  (A15)
// P6.0 - MCU_CHG1    (A0)
// P6.1 - MCU_DCHG1   (A1)
// P6.2 - MCU_CHG2    (A2)
// P6.3 - MCU_DCHG2   (A3)

// TODO: make this more modular

typedef enum {
  PWR_STATUS,
  MCU_CHG1,
  MCU_DCHG1,
  MCU_CHG2,
  MCU_DCHG2
} VoltageIndex;

void mcu_voltage_init(void);

uint16_t mcu_voltage_sample(VoltageIndex pin);
