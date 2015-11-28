#pragma once
#include "spi.h"
#include <stdint.h>
#include <stdbool.h>

// Driver for the AD7280A Lithium Ion Battery Monitoring System

// register map
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
#define CB1_OUTPUT_ENBL                 (1 << 2)    // D02
#define CB2_OUTPUT_ENBL                 (1 << 3)    // D03
#define CB3_OUTPUT_ENBL                 (1 << 4)    // D04
#define CB4_OUTPUT_ENBL                 (1 << 5)    // D05
#define CB5_OUTPUT_ENBL                 (1 << 6)    // D06
#define CB6_OUTPUT_ENBL                 (1 << 7)    // D07


// other configuration
#define AFE_DEV_MAX_CHAIN               8               // maximum number of devices in daisy chain
#define AFE_DEV_IN_CHAIN                6               // number of devices in the chain
#define AFE_CELLS_PER_DEVICE            6               // number of cells per AD7280A

#define AFE_DEVICEADDR_ALL              0x1F            // address all devices
#define AFE_DEVICEADDR_MASTER           0               // master device in daisy chain


// structures
struct Threshold {
  uint16_t voltage_high;
  uint16_t voltage_low;
  //int current_high;
  //int current_low;
  uint16_t temp_high;
  uint16_t temp_low;
};


struct AFEConfig {
  struct SPIConfig *spi_config;         // spi configuration
  struct IOMap cnvst;                   // pin configuration
  uint8_t devices;                      // devices in daisy chain (1 - 8)
  uint8_t crc_table[256];               // crc-8 lookup table
  bool crc_error;                       // crc read calculation status
  uint8_t rdbck_delay;                  // readback delay (usually)
  struct Threshold *charge;
  struct Threshold *discharge;
};



/**
 * Initialize communications with the devices in daisy-chain, and generates
 *   and stores the lookup table for CRC-8 use in the device's RAM
 * @param   ac                pointer to the AFEConfig
 * @return                    true if SPI is correctly initialized
 */
bool afe_init(struct AFEConfig *ac);



/**
 * Read back the data at a register
 * @param  afe                pointer to the AFEConfig
 * @param  device_addr        the device address
 * @param  register_addr      the register address to write data to
 * @return                    the value read back from the register
 */
uint32_t afe_read_register(struct AFEConfig *afe, uint16_t device_addr, uint16_t register_addr);


/**
 * Enable/Disable balancing for the cells specified
 * @param  afe                pointer to the AFEConfig
 * @param  device_addr        the device address
 * @param  cells              cells (XOR'd together)
 * @return                    true on success
 */
bool afe_set_cbx(struct AFEConfig *afe, uint16_t device_addr, uint8_t cells);


/**
 * Set the CBX timer
 * @param  afe                pointer to the AFEConfig
 * @param  device_addr        device ad
 * @param  cbx_timer          an individual cell timer register
 * @param  duration           Timer duration until the cell stops balancing
 * @return                    true on success
 */
bool afe_set_cbx_timer(struct AFEConfig *afe, uint16_t device_addr, uint8_t cbx_timer, uint8_t duration);


/**
 * Set the thresholds to the AFE registers
 * @param  afe                pointer to the AFEConfig
 * @param  thresh             pointer to the Threshold to write
 * @return                    true on success
 */
bool afe_set_thresh(struct AFEConfig *afe, struct Threshold *thresh);


/**
 * Apply a known voltage to verify the operation of the ADC and reference buffer
 *   for all parts in the daisy chain
 * @param   ac          pointer to the AFEConfig
 */
void afe_self_test(struct AFEConfig *ac);
