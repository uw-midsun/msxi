#include "driverlib.h"
#include "ad7280a.h"
#include "afe.h"
// add error thresholds to AFEConfig as structs


/**
 * Computes the XOR remainder for each of the 256 possible CRC-8 bytes, and
 *   stores it in the crc_table field within the AD7280AConfig
 * @param   afe         pointer to the AD7280AConfig
 */
static void prv_crc8_build_table(AFEConfig *afe) {
  for (int i = 0; i < 256; ++i) {
    uint8_t crc = i;

    // xor division, going through each bit, to compute the remainder term (crc)
    for (int k = 0; k < 8; ++k) {
      uint8_t bit = crc & AD7280A_HIGHBIT;
      crc <<= 1;

      if (bit) {
        crc ^= AD7280A_CRC_POLYNOMIAL;
      }
    }

    afe->crc_table[i] = crc;
  }
}


static uint32_t prv_transfer_32_bits(AFEConfig *afe, uint32_t msg) {
  // write commands are 32 bits long
  typedef union {
    uint32_t data;
    uint8_t part[4];
  } buffer;

  buffer.data = msg;

  // MSP430 uses little Endian
  spi_transmit(afe->spi_config, buffer.part[3]);
  spi_transmit(afe->spi_config, buffer.part[2]);
  spi_transmit(afe->spi_config, buffer.part[1]);
  spi_transmit(afe->spi_config, buffer.part[0]);

  typedef union {
    uint32_t data;
    uint8_t part[4];
  } receive;

  // MSP430 uses little Endian
  receive.part[3] = spi_receive(afe->spi_config);
  receive.part[2] = spi_receive(afe->spi_config);
  receive.part[1] = spi_receive(afe->spi_config);
  receive.part[0] = spi_receive(afe->spi_config);

  uint32_t reply = receive.data;

  if (prv_crc8_check(ac, reply)) {
    return reply;
  }

  // error flag will have been raised
  return 0;
}


static bool prv_crc8_check(AFEConfig *afe, uint32_t val) {
  uint8_t crc = afe_crc8_calculate(ac, val >> 10);
  afe->crc_error = (crc == ((val >> 2) & 0xFF));

  return (crc == ((val >> 2) & 0xFF));
}


bool afe_init(AFEConfig *afe) {
  // initialize CRC table into RAM
  prv_crc8_build_table(ac);

  // initialize SPI
  spi_init(afe->spi_config);

  // Example 1: Initializing the AD7280A in datasheet
  // initialize devices in daisy chain
  // 0x01C2B6E2
  afe_write(ac, AD7280A_DEVICEADDR_MASTER, AD7280A_CONTROL_LB, true, CTRL_LB_DSY_CHN_RDBCK_ENBL | CTRL_LB_LOCK_DEVICE_ADDR);

  // 0x038716CA
  afe_write(ac, AD7280A_DEVICEADDR_MASTER, AD7280A_READ, true, READ_CONTROL_LB);

  // 0xF800030A
  afe_write(ac, AD7280A_DEVICEADDR_ALL, AD7280A_CELL_VOLTAGE_1, false, READ_CELL_VOLTAGE_1);

  // wait till changes propagate
  // __delay_cycles()


  afe->crc_error = false;

  return true;
}


uint8_t afe_crc8_calculate(AFEConfig *afe, uint32_t val) {
  uint8_t crc = afe->crc_table[val >> 16 & 0xFF];

  crc = afe->crc_table[crc ^ (val >> 8 & 0xFF)];

  return crc ^ (val & 0xFF);
}


uint32_t afe_write(AFEConfig *afe, uint16_t device_addr, uint16_t register_addr, bool all, uint16_t data) {
  // flip the bits that crc-8 needs, then "or" the other bits on AFTER
  uint32_t msg = (device_addr << 27 | register_addr << 21 | ((data & 0xFF) << 13) | (all << 12));

  if (register_addr == AD7280A_CONTROL_LB) { // D4 is reserved :)
    msg |= (1 << 4);
  }

  msg |= (afe_crc8_calculate(ac, msg >> 11) << 3) | AD7280A_WRITE_BIT_PATTERN;

  return prv_transfer_32_bits(ac, msg);
}


uint8_t afe_read_conversions(AFEConfig *afe, uint16_t device_addr, uint16_t register_addr) {
  // Example 4 in datasheet
  // write the register address to be read from to the read register
  afe_write(afe, device_addr, AD7280A_READ, false, register_addr);

  // turn off read operation on all parts
  afe_write(afe, AD7280A_DEVICEADDR_MASTER, AD7280A_CONTROL_HB, true, CTRL_HB_CONV_RSLT_READ_DSBL);

  // wait (100us?)
  __delay_cycles(100);

  // enable read operation on the selected device
  afe_write(afe, device_addr, AD7280A_CONTROL_HB, false, CTRL_HB_CONV_RSLT_READ_ALL | CTRL_HB_CONV_IN_ALL);

  // wait (100us?)
  __delay_cycles(100);

  // allow conversions to be initiated using CNVST pin on selected part
  afe_write(afe, device_addr, AD7280A_CNVST_CONTROL, false, CNVST_IN_PULSE);

  // conversions through falling edge
  io_set_state(&afe->cnvst, IO_LOW);

  // wait (100us?)
  __delay_cycles(100);

  // bring back high
  io_set_state(&afe->cnvst, IO_HIGH);

  // wait (300us?)
  __delay_cycles(300);

  // gate CNVST signals
  afe_write(afe, AD7280A_DEVICEADDR_MASTER, AD7280A_CNVST_CONTROL, true, CNVST_IN_GATED);

  // apply a low CS pulse to read back each conversion result
  uint32_t reply = afe_write(afe, AD7280A_DEVICEADDR_ALL, AD7280A_CELL_VOLTAGE_1, true, AD7280A_CELL_VOLTAGE_1);

  if (prv_crc8_check(afe, reply)) {
    // extract data (D22 - 11) out from reply
    reply = (reply >> 11 ) & 0xFFF;
    return reply;
  } else {
    // crc_error flag will have been raised
    return 0;
  }
}


uint32_t afe_read_register(AFEConfig *afe, uint16_t device_addr, uint16_t register_addr) {
  // refer to Example 6 in datasheet
  // turn off read operation on all parts
  afe_write(afe, AD7280A_DEVICEADDR_MASTER, AD7280A_CONTROL_HB, true, CTRL_HB_CONV_RSLT_READ_DSBL);

  // enable read operation on the selected device
  afe_write(afe, device_addr, AD7280A_CONTROL_HB, false, CTRL_HB_CONV_RSLT_READ_ALL);

  // configure read register with the address to be read from, and read
  uint32_t reply = afe_write(afe, device_addr, AD7280A_READ, false, register_addr);

  // check returned value against crc computation
  if (prv_crc8_check(ac, reply)) {
    return reply;
  }

  return 0; // crc_error flag will have been raised
}


void afe_self_test(AFEConfig *afe) {
  // see Example 7: Self-Test Conversion on All Parts from datasheet

  // select self-test conversion
  // 0x01B81092
  afe_write(ac, AD7280A_DEVICEADDR_MASTER, AD7280A_CONTROL_HB, true, CTRL_HB_CONV_IN_SELF_TEST);

  // enable the daisy-chain register read operation on all parts
  afe_write(ac, AD7280A_DEVICEADDR_MASTER, AD7280A_CONTROL_LB, true, CTRL_LB_DSY_CHN_RDBCK_ENBL | CTRL_LB_LOCK_DEVICE_ADDR);

  // write the self-test conversion register to the read registers
  afe_write(ac, AD7280A_DEVICEADDR_MASTER, AD7280A_READ, true, READ_SELF_TEST);

  // program the CNVST control register, to allow a single CNVST pulse
  afe_write(ac, AD7280A_DEVICEADDR_MASTER, AD7280A_CNVST_CONTROL, true, CNVST_IN_PULSE);

  // initiate conversions
  // wait...


  // gate CNVST
  afe_write(ac, AD7280A_DEVICEADDR_MASTER, AD7280A_CNVST_CONTROL, false, CNVST_IN_GATED);

  // apply low CS pulse
  afe_write(ac, AD7280A_DEVICEADDR_ALL, AD7280A_CELL_VOLTAGE_1, false, READ_CELL_VOLTAGE_1);
}
