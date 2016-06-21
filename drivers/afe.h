#pragma once
#include "spi.h"
#include <stdint.h>
#include <stdbool.h>

#define AFE_DEVADDR_MASTER                          0

#define AFE_CELL_VOLTAGE_1                          0x00            // D11 to D0, read
#define AFE_CELL_VOLTAGE_2                          0x01            // D11 to D0, read
#define AFE_CELL_VOLTAGE_3                          0x02            // D11 to D0, read
#define AFE_CELL_VOLTAGE_4                          0x03            // D11 to D0, read
#define AFE_CELL_VOLTAGE_5                          0x04            // D11 to D0, read
#define AFE_CELL_VOLTAGE_6                          0x05            // D11 to D0, read

#define AFE_AUX_ADC_1                               0x06            // D11 to D0, read
#define AFE_AUX_ADC_2                               0x07            // D11 to D0, read
#define AFE_AUX_ADC_3                               0x08            // D11 to D0, read
#define AFE_AUX_ADC_4                               0x09            // D11 to D0, read
#define AFE_AUX_ADC_5                               0x0A            // D11 to D0, read
#define AFE_AUX_ADC_6                               0x0B            // D11 to D0, read

// cell balance registers
#define CB1_OUTPUT_ENBL                             (1 << 2)        // D02
#define CB2_OUTPUT_ENBL                             (1 << 3)        // D03
#define CB3_OUTPUT_ENBL                             (1 << 4)        // D04
#define CB4_OUTPUT_ENBL                             (1 << 5)        // D05
#define CB5_OUTPUT_ENBL                             (1 << 6)        // D06
#define CB6_OUTPUT_ENBL                             (1 << 7)        // D07


struct Threshold {
  const uint16_t low;
  const uint16_t high;
};

struct TempThreshold {
  int16_t high;
  int16_t low;
};

struct AFEConfig {
  const struct SPIConfig *spi_config;     // spi configuration
  const int thermistor[4096];             // thermistor[adcval] = temp
  const struct IOMap *cnvst;              // conversion start
  const struct IOMap *pd;                 // power down pin
  uint8_t crc_table[256];                 // set by afe_init
  uint8_t devices;                        // set by afe_init
  struct TempThreshold d_temp;
  struct TempThreshold c_temp;
  bool crc_error;                         // crc read calculation status
};

struct ConversionResult {
  uint16_t vin[48];
  int16_t aux[48];
};

uint8_t afe_init(struct AFEConfig *ac);

uint8_t afe_read_register(struct AFEConfig *afe, uint8_t devaddr, uint8_t regaddr);

bool afe_set_cbx(struct AFEConfig *afe, uint16_t devaddr, uint8_t cells);

uint32_t afe_read_all_conversions(struct AFEConfig *afe, struct ConversionResult *cr);
