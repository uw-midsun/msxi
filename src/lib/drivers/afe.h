#pragma once
#include <stdint.h>
#include <stdbool.h>
#include "spi.h"

// Driver for the AD7280A Lithium Ion Battery Monitoring System


// Register map
#define AD7280A_CELL_VOLTAGE_1              0x00          // D11 to D0, read
#define AD7280A_CELL_VOLTAGE_2              0x01          // D11 to D0, read
#define AD7280A_CELL_VOLTAGE_3              0x02          // D11 to D0, read
#define AD7280A_CELL_VOLTAGE_4              0x03          // D11 to D0, read
#define AD7280A_CELL_VOLTAGE_5              0x04          // D11 to D0, read
#define AD7280A_CELL_VOLTAGE_6              0x05          // D11 to D0, read
#define AD7280A_AUX_ADC_1                   0x06          // D11 to D0, read
#define AD7280A_AUX_ADC_2                   0x07          // D11 to D0, read
#define AD7280A_AUX_ADC_3                   0x08          // D11 to D0, read
#define AD7280A_AUX_ADC_4                   0x09          // D11 to D0, read
#define AD7280A_AUX_ADC_5                   0x0A          // D11 to D0, read
#define AD7280A_AUX_ADC_6                   0x0B          // D11 to D0, read
#define AD7280A_SELF_TEST                   0x0C          // D11 to D0, read
#define AD7280A_CONTROL_HB                  0x0D          // D15 to D8, read/write
#define AD7280A_CONTROL_LB                  0x0E          // D7 to D0,  read/write
#define AD7280A_CELL_OVERVOLTAGE            0x0F          // D7 to D0,  read/write
#define AD7280A_CELL_UNDERVOLTAGE           0x10          // D7 to D0,  read/write
#define AD7280A_AUX_ADC_OVERVOLTAGE         0x11          // D7 to D0,  read/write
#define AD7280A_AUX_ADC_UNDERVOLTAGE        0x12          // D7 to D0,  read/write
#define AD7280A_ALERT                       0x13          // D7 to D0,  read/write
#define AD7280A_CELL_BALANCE                0x14          // D7 to D0,  read/write
#define AD7280A_CB1_TIMER                   0x15          // D7 to D0,  read/write
#define AD7280A_CB2_TIMER                   0x16          // D7 to D0,  read/write
#define AD7280A_CB3_TIMER                   0x17          // D7 to D0,  read/write
#define AD7280A_CB4_TIMER                   0x18          // D7 to D0,  read/write
#define AD7280A_CB5_TIMER                   0x19          // D7 to D0,  read/write
#define AD7280A_CB6_TIMER                   0x1A          // D7 to D0,  read/write
#define AD7280A_PD_TIMER                    0x1B          // D7 to D0,  read/write
#define AD7280A_READ                        0x1C          // D7 to D0,  read/write
#define AD7280A_CNVST_CONTROL               0x1D          // D7 to D0,  read/write


// 32-bit write command to frame 32 SCLKs to read back the desired voltage
#define AD7280A_TXVAL                       0xF800030A    // see AD7280A datasheet


// number of bits that CRC-8 uses when computing a checksum
#define AD7280A_CRC_NUMBITS_READ            22            // CRC bits,  read
#define AD7280A_CRC_NUMBITS_WRITE           21            // CRC bits,  write

/**
 * AD7280 uses the following CRC-8 polynomial (Big-Endian), with order 8
 *   P(x) = x^8 + x^5 + x^3 + x^2 + x^1 + x^0
 *        = 0b100101111
 *        = 0x2F
 */
#define AD7280A_CRC_POLYNOMIAL_ORDER        8
#define AD7280A_CRC_POLYNOMIAL              0x2F
#define AD7280A_HIGHBIT                     (1 << (AD7280A_CRC_POLYNOMIAL_ORDER - 1))


// misc bits
#define AD7280A_ADC_12_BIT_RES              4095          // AD7280A uses a 12 bit ADC
#define AD7280A_WRITE_BIT_PATTERN           0x2           // required Bits[D2:D0]

// other configuration
#define AD7280A_DEV_MAX_CHAIN               8             // maximum number of devices in daisy chain
#define AD7280A_DEV_IN_CHAIN                6             // number of devices in the chain
#define AD7280A_CELLS_PER_DEVICE            6             // number of cells per AD7280A

#define AD7280A_DEVICEADDR_ALL              0x1F          // address all devices
#define AD7280A_DEVICEADDR_MASTER           0             // master device in daisy chain

// SPI configuration
// according to the datasheet, the recommended SCLK to ensure correct operation of daisy-chain interface is 1MHz
#define AD7280A_SPI_MAX_CLK                 1000000     // clock speed in Hz
// when reading back from a single device in a stack of AD7280A's, the SCLK must be lower than
//   1 MHz to read back register data up the chain
#define AD7280A_SPI_CLK                     1000000     // clock speed in Hz

// other configuration
#define AD7280A_DEV_MAX_CHAIN               8             // maximum number of devices in daisy chain
#define AD7280A_DEV_IN_CHAIN                6             // number of devices in the chain
#define AD7280A_CELLS_PER_DEVICE            6             // number of cells per AD7280A

// error thresholds
// discharge
#define AD7280A_ERR_DCHRG_THRESH_TMP_H      55            //
#define AD7280A_ERR_DCHRG_THRESH_TMP_L      -20           //

// charge
#define AD7280A_ERR_CHRG_THRESH_TMP_H       50
#define AD7280A_ERR_CHRG_THRESH_TMP_L       0

// charge + discharge
#define AD7280A_ERR_THRESH_OCUR_H           125
#define AD7280A_ERR_THRESH_OCUR_L           75
#define AD7280A_ERR_THRESH_UCUR_H           -58
#define AD7280A_ERR_THRESH_UCUR_L           -125
#define AD7280A_ERR_CHRG_THRESH_VLT_H       1             // change
#define AD7280A_ERR_CHRG_THRESH_VLT_L       1

// structures
typedef struct ad7280a_config {
  SPIConfig *spi_config;                                  // spi configuration
  uint8_t crc_table[256];                                 // crc-8 lookup table
  bool crc_error;                                         // crc read calculation status
} AD7280AConfig;



/**
 * Initialize communications with the devices in daisy-chain, and generates
 *   and stores the lookup table for CRC-8 use in the device's RAM
 * @param   ac          pointer to the AD7280AConfig
 */
bool AD7280A_init(AD7280AConfig *ac);


/**
 * Computes the XOR remainder for each of the 256 possible CRC-8 bytes, and
 *   stores it in the crc_table field within the AD7280AConfig
 * @param   ac          pointer to the AD7280AConfig
 */
static void prv_crc8_build_table(AD7280AConfig *ac);


/**
 * CRC calculation for read AND write operations on the bits for a read/write:
 *   - 21 bits for a write
 *   - 22 bits for a read
 * @param   ac          pointer to the AD7280AConfig
 * @param   message     only the data bits that the CRC is computed on
 * @return: crc remainder term after xor division
 */
uint8_t AD7280A_crc8_calculate(AD7280AConfig *ac, uint32_t val);


/**
 * Verify the CRC obtained from a read command
 * @param   ac          pointer to the AD7280AConfig
 * @param   val         data received from the AD7280A during a read command
 */
bool prv_crc8_check(AD7280AConfig *ac, uint32_t val);


/**
 * CRC Write
 * computes the CRC-8 checksum and then performs the write command on the
 *   device on the SPI bus
 * @param   ac                pointer to the AD7280AConfig
 * @param   device_addr       the device address
 * @param   register_addr     the register address to write data to
 * @param   data              the data to write to the register
 */
uint8_t AD7280A_write(AD7280AConfig *ac, uint16_t device_addr, uint16_t register_addr, bool all, uint16_t data);



static uint32_t prv_transfer_32_bits(AD7280AConfig *ac, uint32_t msg);


/**
 * Reads back the data at the register address from the given device
 */
uint32_t AD7280A_read(AD7280ACONFIG *ac, uint16_t device_addr, uint16_t register_addr);


/**
 * Converts ADC value to voltage
 * Possibly static? do i need this even
 */
float AD7280A_convert_value(uint16_t data);


/**
 * Apply a known voltage to verify the operation of the ADC and reference buffer
 *   for all parts in the daisy chain
 * @param   ac          pointer to the AD7280AConfig
 */
void AD7280A_self_test(AD7280AConfig *ac);
