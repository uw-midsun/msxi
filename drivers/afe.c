#include "ad7280a.h"
#include "afe.h"
#include "spi.h"

/**
 * Computes the XOR remainder for each of the 256 possible CRC-8 bytes, and
 *   stores it in the crc_table field within the AD7280AConfig
 * @param   afe         pointer to the AD7280AConfig
 */
static void prv_crc8_build_table(struct AFEConfig *afe) {
  uint8_t i, k;
  for (i = 0; i < 256; ++i) {
    uint8_t crc = i;

    // xor division, going through each bit, to compute the remainder term (crc)
    for (k = 0; k < 8; ++k) {
      uint8_t bit = crc & AFE_HIGHBIT;
      crc <<= 1;

      if (bit) {
        crc ^= AFE_CRC_POLYNOMIAL;
      }
    }

    afe->crc_table[i] = crc;
  }
}


/**
 * CRC calculation for read AND write operations on the bits for a read/write:
 *   - 21 bits for a write
 *   - 22 bits for a read
 * @param   ac          pointer to the AFEConfig
 * @param   message     only the data bits that the CRC is computed on
 * @return: crc remainder term after xor division
 */
static uint8_t prv_crc8_calculate(struct AFEConfig *afe, uint32_t val) {
  uint8_t crc = afe->crc_table[val >> 16 & 0xFF];

  crc = afe->crc_table[crc ^ (val >> 8 & 0xFF)];

  return crc ^ (val & 0xFF);
}


static bool prv_crc8_check(struct AFEConfig *afe, uint32_t val) {
  uint8_t crc = prv_crc8_calculate(afe, val >> 10);
  afe->crc_error = (crc == ((val >> 2) & 0xFF));

  return (crc == ((val >> 2) & 0xFF));
}


static uint32_t prv_transfer_32_bits(struct AFEConfig *afe, uint32_t msg) {
  // write commands are 32 bits long
  union {
    uint32_t data;
    uint8_t part[4];
  } buffer;

  buffer.data = msg;

  // MSP430 uses little Endian
  spi_transmit(afe->spi_config, buffer.part[3]);
  spi_transmit(afe->spi_config, buffer.part[2]);
  spi_transmit(afe->spi_config, buffer.part[1]);
  spi_transmit(afe->spi_config, buffer.part[0]);

  union {
    uint32_t data;
    uint8_t part[4];
  } receive;

  // MSP430 uses little Endian
  receive.part[3] = spi_receive(afe->spi_config);
  receive.part[2] = spi_receive(afe->spi_config);
  receive.part[1] = spi_receive(afe->spi_config);
  receive.part[0] = spi_receive(afe->spi_config);

  uint32_t reply = receive.data;

  if (prv_crc8_check(afe, reply)) {
    return reply;
  }

  // error flag will have been raised
  return 0;
}


/**
 * CRC Write
 * computes the CRC-8 checksum and then performs the write command on the
 *   device on the SPI bus
 * @param   ac                pointer to the AFEConfig
 * @param   device_addr       the device address
 * @param   register_addr     the register address to write data to
 * @param   data              the data to write to the register
 */
static uint32_t prv_write(struct AFEConfig *afe, uint16_t device_addr, uint16_t register_addr, bool all, uint16_t data) {
  // flip the bits that crc-8 needs, then "or" the other bits on AFTER
  uint32_t msg = ((uint32_t)device_addr << 27 | (uint32_t)register_addr << 21 | ((data & 0xFF) << 13) | ((uint32_t)all << 12));

  if (register_addr == AFE_CONTROL_LB) { // D4 is reserved :)
    msg |= (1 << 4);
  }

  msg |= (prv_crc8_calculate(afe, msg >> 11) << 3) | AFE_WRITE_BIT_PATTERN;

  return prv_transfer_32_bits(afe, msg);
}


bool afe_init(struct AFEConfig *afe) {
  // initialize CRC table into RAM
  prv_crc8_build_table(afe);

  // initialize SPI
  bool status = spi_init(afe->spi_config);

  // Example 1: Initializing the AD7280A in datasheet
  // initialize devices in daisy chain
  prv_write(afe, AFE_DEVICEADDR_MASTER, AFE_CONTROL_LB, true, CTRL_LB_DSY_CHN_RDBCK_ENBL | CTRL_LB_LOCK_DEVICE_ADDR);

  // configure the read register for all devices
  prv_write(afe, AFE_DEVICEADDR_MASTER, AFE_READ, true, READ_CONTROL_LB);

  // read master address by writing to the highest available address
  uint32_t reply = prv_write(afe, AFE_DEVICEADDR_ALL, AFE_CELL_VOLTAGE_1, false, READ_CELL_VOLTAGE_1);

  // read back all slave device ids
  for (int i = 0; i < afe->devices; ++i) {
    reply = prv_write(afe, AFE_DEVICEADDR_ALL, AFE_CELL_VOLTAGE_1, false, READ_CELL_VOLTAGE_1);
  }

  // initialize crc_error
  afe->crc_error = false;

  return status;
}


uint16_t afe_read_conversion(struct AFEConfig *afe, uint16_t device_addr, uint16_t register_addr) {
  // Example 4 in datasheet
  // write the register address to be read from to the read register
  prv_write(afe, device_addr, AFE_READ, false, register_addr);

  // turn off read operation on all parts
  prv_write(afe, AFE_DEVICEADDR_MASTER, AFE_CONTROL_HB, true, CTRL_HB_CONV_RSLT_READ_DSBL);

  // wait (100us?)
  __delay_cycles(100);

  // enable read operation on the selected device
  prv_write(afe, device_addr, AFE_CONTROL_HB, false, CTRL_HB_CONV_RSLT_READ_ALL | CTRL_HB_CONV_IN_ALL);

  // wait (100us?)
  __delay_cycles(100);

  // allow conversions to be initiated using CNVST pin on selected part
  prv_write(afe, device_addr, AFE_CNVST_CONTROL, false, CNVST_IN_PULSE);

  // conversions through falling edge
  io_set_state(&afe->cnvst, IO_LOW);

  // wait (100us?)
  __delay_cycles(100);

  // bring back high
  io_set_state(&afe->cnvst, IO_HIGH);

  // wait (300us?)
  __delay_cycles(300);

  // gate CNVST signals
  prv_write(afe, AFE_DEVICEADDR_MASTER, AFE_CNVST_CONTROL, true, CNVST_IN_GATED);

  // apply a low CS pulse to read back each conversion result
  uint32_t reply = prv_write(afe, AFE_DEVICEADDR_ALL, AFE_CELL_VOLTAGE_1, true, AFE_CELL_VOLTAGE_1);

  if (prv_crc8_check(afe, reply)) {
    // extract data (D22 - 11) out from reply
    reply = (reply >> 11) & 0xFFF;
    return reply;
  } else {
    // crc_error flag will have been raised
    return 0;
  }
}


uint32_t afe_read_register(struct AFEConfig *afe, uint16_t device_addr, uint16_t register_addr) {
  // refer to Example 6 in datasheet
  // turn off read operation on all parts
  prv_write(afe, AFE_DEVICEADDR_MASTER, AFE_CONTROL_HB, true, CTRL_HB_CONV_RSLT_READ_DSBL);

  // enable read operation on the selected device
  prv_write(afe, device_addr, AFE_CONTROL_HB, false, CTRL_HB_CONV_RSLT_READ_ALL);

  // configure read register with the address to be read from, and read
  uint32_t reply = prv_write(afe, device_addr, AFE_READ, false, register_addr);

  // check value against crc, raise crc_error flag if needed
  if (prv_crc8_check(afe, reply)) {
    return reply;
  }

  return 0;
}


bool afe_set_cbx(struct AFEConfig *afe, uint16_t device_addr, uint8_t cells) {
  uint32_t tmp = prv_write(afe, device_addr, AFE_CELL_BALANCE, false, cells);

  // check write-acknowledge bit (D11)?
  return true;
}


bool afe_set_cbx_timer(struct AFEConfig *afe, uint16_t device_addr, uint8_t cbx_timer, uint8_t duration) {
  // uses a 71.5s resolution CBx timer
  prv_write(afe, device_addr, cbx_timer, false, duration);

  // check write-acknowledge bit (D11)?
  return true;
}


bool afe_set_thresh(struct AFEConfig *afe, struct Threshold *thresh) {
  // write undervoltage threshold
  prv_write(afe, AFE_DEVICEADDR_ALL, AFE_CELL_UNDERVOLTAGE, true, thresh->voltage_low);

  // write overvoltage threshold
  prv_write(afe, AFE_DEVICEADDR_ALL, AFE_CELL_OVERVOLTAGE, true, thresh->voltage_high);

  // set adc under-temperature threshold
  prv_write(afe, AFE_DEVICEADDR_ALL, AFE_AUX_ADC_UNDERVOLTAGE, true, thresh->temp_low);

  // set adc over-temperature threshold
  prv_write(afe, AFE_DEVICEADDR_ALL, AFE_AUX_ADC_OVERVOLTAGE, true, thresh->temp_high);
  return true;
}


void afe_self_test(struct AFEConfig *afe) {
  // see Example 7: Self-Test Conversion on All Parts from datasheet

  // select self-test conversion
  // 0x01B81092
  prv_write(afe, AFE_DEVICEADDR_MASTER, AFE_CONTROL_HB, true, CTRL_HB_CONV_IN_SELF_TEST);

  // enable the daisy-chain register read operation on all parts
  prv_write(afe, AFE_DEVICEADDR_MASTER, AFE_CONTROL_LB, true, CTRL_LB_DSY_CHN_RDBCK_ENBL | CTRL_LB_LOCK_DEVICE_ADDR);

  // write the self-test conversion register to the read registers
  prv_write(afe, AFE_DEVICEADDR_MASTER, AFE_READ, true, READ_SELF_TEST);

  // program the CNVST control register, to allow a single CNVST pulse
  prv_write(afe, AFE_DEVICEADDR_MASTER, AFE_CNVST_CONTROL, true, CNVST_IN_PULSE);

  // initiate conversions
  // wait...


  // gate CNVST
  prv_write(afe, AFE_DEVICEADDR_MASTER, AFE_CNVST_CONTROL, false, CNVST_IN_GATED);

  // apply low CS pulse
  prv_write(afe, AFE_DEVICEADDR_ALL, AFE_CELL_VOLTAGE_1, false, READ_CELL_VOLTAGE_1);
}
