#include "driverlib.h"
#include "ad7280a.h"
#include "afe.h"

bool AD7280A_init(AD7280AConfig *ac) {
  // initialize CRC table into RAM
  prv_crc8_build_table(ac);

  SPIConfig spi_config = {
    .data_out = { GPIO_PORT_P3, GPIO_PIN4 },
    .data_in = { GPIO_PORT_P3, GPIO_PIN5 },
    .clock_out = { GPIO_PORT_3, GPIO_PIN0 },
    .cs = { GPIO_PORT_3, GPIO_PIN3 },
    .clock_freq = AD7280A_SPI_CLK,
    .base_addr = USCI_A0_BASE
  };

  ac->spi_config = &spi_config;

  spi_init(ac->spi_config);

  // initialize devices in daisy chain (see Example 1: Initializing the AD7280A in datasheet)
  // 0x01C2B6E2
  AD7280A_write(ac, AD7280A_DEVICEADDR_MASTER, AD7280A_CONTROL_LB, true, CTRL_LB_DSY_CHN_RDBCK_ENBL | CTRL_LB_LOCK_DEVICE_ADDR);

  // 0x038716CA
  AD7280A_write(ac, AD7280A_DEVICEADDR_MASTER, AD7280A_READ, true, READ_CONTROL_LB);

  // 0xF800030A
  AD7280A_write(ac, AD7280A_DEVICEADDR_ALL, AD7280A_CELL_VOLTAGE_1, false, READ_CELL_VOLTAGE_1);

  // wait

  ac->crc_error = false;

  return true;
}


static void prv_crc8_build_table(AD7280AConfig *ac) {
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

    ac->crc_table[i] = crc;
  }
}


uint8_t AD7280A_crc8_calculate(AD7280AConfig *ac, uint32_t val) {
  uint8_t crc = ac->crc_table[val >> 16 & 0xFF];

  crc = ac->crc_table[crc ^ (val >> 8 & 0xFF)];

  return crc ^ (val & 0xFF);
}


bool prv_crc8_check(AD7280AConfig *ac, uint32_t val) {
  uint8_t crc = AD7280A_crc8_calculate(ac, val >> 10);
  ac->crc_error = (crc == ((val >> 2) & 0xFF));

  return (crc == ((val >> 2) & 0xFF));
}


uint8_t AD7280A_write(AD7280AConfig *ac, uint16_t device_addr, uint16_t register_addr, bool all, uint16_t data) {
  // flip the bits that crc-8 needs, then "or" the other bits on AFTER
  uint32_t msg = (device_addr << 27 | register_addr << 21 | ((data & 0xFF) << 13) | (all << 12));

  if (register_addr == AD7280A_CONTROL_LB) { // D4 is reserved :)
    msg |= (1 << 4);
  }

  msg |= (AD7280A_crc8_calculate(ac, msg >> 11) << 3) | AD7280A_WRITE_BIT_PATTERN;

  return prv_transfer_32_bits(ac, msg);
}


uint32_t AD7280A_read(AD7280ACONFIG *ac, uint16_t device_addr, uint16_t register_addr) {
  // refer to the Example 2 in datasheet
  // we never have to read from a single device
  // write register address 0x00 to the read register
  AD7280A_write(ac, AD7280A_DEVICEADDR_MASTER, AD7280A_READ, true, READ_CELL_VOLTAGE_1);

  // set bits [D15:D12]
  AD7280A_write(ac, AD7280A_DEVICEADDR_MASTER, AD7280A_CONTROL_HB, true, READ_CELL_VOLTAGE_1);

  // program CNVST control register
  AD7280A_write(ac, AD7280A_DEVICEADDR_MASTER, AD7280A_CNVST_CONTROL, true, CNVST_IN_PULSE);

  // wait

  uint32_t reply = AD7280A_write(ac, AD7280A_DEVICEADDR_ALL, AD7280A_CELL_VOLTAGE_1, false, AD7280A_CELL_VOLTAGE_1);

  // check returned value against crc computation
  if (prv_crc8_check(ac, reply)) {
    return reply;
  }

  return 0; // read error flag will have been raised
}


void AD7280A_self_test(AD7280AConfig *ac) {
  // see Example 7: Self-Test Conversion on All Parts from datasheet

  // select self-test conversion
  AD7280A_write(ac, AD7280A_DEVICEADDR_MASTER, AD7280A_CONTROL_HB, true, CTRL_HB_CONV_IN_SELF_TEST);       // 0x01B81092

  // enable the daisy-chain register read operation on all parts
  AD7280A_write(ac, AD7280A_DEVICEADDR_MASTER, AD7280A_CONTROL_LB, true, CTRL_LB_DSY_CHN_RDBCK_ENBL | CTRL_LB_LOCK_DEVICE_ADDR);

  // write the self-test conversion register to the read registers
  AD7280A_write(ac, AD7280A_DEVICEADDR_MASTER, AD7280A_READ, true, READ_SELF_TEST);

  // program the CNVST control register, to allow a single CNVST pulse
  AD7280A_write(ac, AD7280A_DEVICEADDR_MASTER, AD7280A_CNVST_CONTROL, true, CNVST_IN_PULSE);

  // initiate conversions
  // wait...


  // gate CNVST
  AD7280A_write(ac, AD7280A_DEVICEADDR_MASTER, AD7280A_CNVST_CONTROL, false, CNVST_IN_UNGATED);

  // apply low CS pulse
  AD7280A_write(ac, AD7280A_DEVICEADDR_ALL, AD7280A_CELL_VOLTAGE_1, false, READ_CELL_VOLTAGE_1);
}


static uint32_t prv_transfer_32_bits(AD7280AConfig *ac, uint32_t msg) {
  // write commands are 32 bits long
  union Data {
    uint32_t data;
    uint8_t part[4];
  } buffer;

  buffer.data = msg;

  spi_write(ac->spi_config, buffer.part[3]);
  spi_write(ac->spi_config, buffer.part[2]);
  spi_write(ac->spi_config, buffer.part[1]);
  spi_write(ac->spi_config, buffer.part[0]);

  union Data {
    uint32_t data;
    uint8_t part[4];
  } receive;

  receive.part[3] = spi_read(ac->spi_config);
  receive.part[2] = spi_read(ac->spi_config);
  receive.part[1] = spi_read(ac->spi_config);
  receive.part[0] = spi_read(ac->spi_config);

  uint32_t reply = receive.data;

  if (prv_crc8_check(ac, reply)) {
    return reply;
  }

  return 0; // error flag will have been raised
}
