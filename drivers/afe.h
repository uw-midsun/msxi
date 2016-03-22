#pragma once
#include "spi.h"
#include <stdint.h>
#include <stdbool.h>

// driver for the AD7280A Lithium Ion Battery Monitoring System

// register map
#define INPUT_1                         0x04
#define INPUT_2                         0x08
#define INPUT_3                         0x10
#define INPUT_4                         0x20
#define INPUT_5                         0x30
#define INPUT_6                         0x40

// cell balance
#define AFE_CB1_TIMER                   0x15            // D7 to D0,  read/write
#define AFE_CB2_TIMER                   0x16            // D7 to D0,  read/write
#define AFE_CB3_TIMER                   0x17            // D7 to D0,  read/write
#define AFE_CB4_TIMER                   0x18            // D7 to D0,  read/write
#define AFE_CB5_TIMER                   0x19            // D7 to D0,  read/write
#define AFE_CB6_TIMER                   0x1A            // D7 to D0,  read/write

// cell timer registers (71.5s resolution, each bit is 71.5s)
// used by CBX and PD timers
#define TIMER_71_5s                     (1 << 3)
#define TIMER_143s                      (1 << 4)
#define TIMER_286s                      (1 << 5)
#define TIMER_572s                      (1 << 6)
#define TIMER_1144s                     (1 << 7)

// cell balance registers
// D0 is a reserved bit
#define CB1_OUTPUT_ENBL                 (1 << 2)        // D02
#define CB2_OUTPUT_ENBL                 (1 << 3)        // D03
#define CB3_OUTPUT_ENBL                 (1 << 4)        // D04
#define CB4_OUTPUT_ENBL                 (1 << 5)        // D05
#define CB5_OUTPUT_ENBL                 (1 << 6)        // D06
#define CB6_OUTPUT_ENBL                 (1 << 7)        // D07

// other configuration
// todo: maybe remove this
#define AFE_DEV_MAX_CHAIN               8               // maximum number of devices in daisy chain
#define AFE_DEV_IN_CHAIN                6               // number of devices in the chain
#define AFE_CELLS_PER_DEVICE            6               // number of cells per AD7280A

#define AFE_DEVICEADDR_ALL              0x1F            // address all devices
#define AFE_DEVICEADDR_MASTER           0               // master device in daisy chain

// todo: maybe remove this
#define CELL_THRESH_RES                 16              // 16mV
#define AUX_THRESH_RES                  16              // 19mV


// structures
struct Threshold {
  uint16_t high;
  uint16_t low;
};


struct AFEConfig {
  const struct SPIConfig *spi_config;     // spi configuration
  const struct IOMap cnvst;               // pin configuration
  const uint8_t slaves;                   // slave devices in daisy chain
  uint8_t crc_table[256];                 // crc-8 lookup table
  bool crc_error;                         // crc read calculation status
  const struct Threshold v_charge;
  const struct Threshold v_discharge;
  const struct Threshold temp;
};


// initialize devices in daisy chain
uint32_t afe_init(struct AFEConfig *ac);


// read back the data at a register
// todo: maybe move this to ad7280a.h
uint32_t afe_read_register(struct AFEConfig *afe, uint16_t device_addr, uint16_t register_addr);


// read the AUX ADC (temperature) conversion on one channel
// aux_adc between 0 and 5
uint16_t afe_aux_conversion(struct AFEConfig *afe, uint16_t device_addr, uint16_t aux_adc);


// read the Voltage ADC conversion on one channel
// cell between 0 and 5
uint16_t afe_voltage_conversion(struct AFEConfig *afe, uint16_t device_addr, uint16_t cell);


// enable/disable balancing for the cells specified
bool afe_set_cbx(struct AFEConfig *afe, uint16_t device_addr, uint8_t cells);

// set CBX using timers
bool afe_set_cbx_timer(struct AFEConfig *afe, uint16_t device_addr, uint8_t cbx_timer, uint8_t duration);

bool afe_set_thresh(struct AFEConfig *afe, bool charge);


// apply a known voltage to verify the operation of the ADC and reference buffer
// todo: maybe move this to ad7280a.h
void afe_self_test(struct AFEConfig *afe);
