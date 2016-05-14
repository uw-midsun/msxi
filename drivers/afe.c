#include "ad7280a.h"
#include "afe.h"
#include "spi.h"

// helper function to propagate crc8 lookup table
void prv_crc8_build_table(struct AFEConfig *afe) {
  uint16_t i, k;

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

// calculate the crc on the message
static uint8_t prv_crc8_calculate(struct AFEConfig *afe, uint32_t val) {
  uint8_t crc = afe->crc_table[(uint16_t)(val >> 16 & 0xFF)];

  crc = afe->crc_table[crc ^ (val >> 8 & 0xFF)];

  return crc ^ (val & 0xFF);
}

// return true if it is a valid message that was read back
// sets crc_error to true if an error occurred
bool prv_crc8_check(struct AFEConfig *afe, uint32_t val) {
  // calculate crc on bits 31:10
  uint8_t crc = prv_crc8_calculate(afe, val >> 10);
  // check if crc matches the 8 bits we received (D9:D2)
  afe->crc_error = (crc != ((val >> 2) & 0xFF));

  return (crc == ((val >> 2) & 0xFF));
}

// transfer 32 bits of data to the afe
uint32_t prv_transfer_32_bits(struct AFEConfig *afe, uint32_t msg) {
  union {
    uint32_t data;
    uint8_t part[4];
  } buffer;

  buffer.data = msg;

  union {
    uint32_t data;
    uint8_t part[4];
  } reply;

  // MSP430 uses little Endian
  // ie. passing in 0xF800030A results in part = { 0A, 03, 00, F8 }
  // we want to send F8 first
  spi_select(afe->spi_config);

  reply.part[3] = spi_exchange(afe->spi_config, buffer.part[3]);
  reply.part[2] = spi_exchange(afe->spi_config, buffer.part[2]);
  reply.part[1] = spi_exchange(afe->spi_config, buffer.part[1]);
  reply.part[0] = spi_exchange(afe->spi_config, buffer.part[0]);

  spi_deselect(afe->spi_config);

  return reply.data;
}

// sent data to devices (we don't care about return value)
// devaddr is an address LSB first (ie. passed through prv_reverse_bits)
static void prv_write(struct AFEConfig *afe,
                      uint8_t devaddr, uint8_t regaddr, bool all, uint16_t data) {
  uint32_t msg = (uint32_t)devaddr << 27 | (uint32_t)regaddr << 21 |
                 ((uint32_t)data & 0xFF) << 13 |
                 (uint32_t)all << 12;

  if (regaddr == AFE_CONTROL_LB) { // D4 is reserved :)
    msg |= ((uint32_t)1 << 17);
  }

  msg |= (prv_crc8_calculate(afe, msg >> 11) << 3) |
         AFE_WRITE_BIT_PATTERN;

  prv_transfer_32_bits(afe, msg);
}

static uint32_t prv_read_32_bits(struct AFEConfig *afe) {
  // to read data back, we send the 0xF800030A command
  return prv_transfer_32_bits(afe, AFE_READ_TRANSFER_VALUE);
}

// device addresses are 5 bits configured LSB first
static uint8_t prv_reverse_bits(uint8_t devaddr) {
  uint8_t reply = 0;
  // kral's yoloswaggins bitfield hack
  // 0 0 0 0 0 0 0 1
  //               d0
  union {
    struct {
      bool d0 : 1;
      bool d1 : 1;
      bool d2 : 1;
      bool d3 : 1;
      bool d4 : 1;
      bool d5 : 1;
      bool d6 : 1;
      bool d7 : 1;
    };
    uint8_t data;
  } address;

  address.data = devaddr;
  reply = (address.d0 << 4) | (address.d1 << 3) |
          (address.d2 << 2) | (address.d3 << 1) | (address.d4 << 0);

  return reply;
}

static uint16_t prv_convert_voltage(uint32_t msg) {
  uint32_t adcval = (msg >> 11) & 0xFFF;

  // table 7: adc returns value in [1000mV, 5000mV]
  return ((adcval * 4000) / 4095) + 1000;
}

static uint16_t prv_convert_temp(uint32_t msg) {
  uint16_t adcval = (msg >> 11) & 0xFFF;

  return adcval;
}

// initialize daisy chain
uint8_t afe_init(struct AFEConfig *afe) {
  //assert(afe->spi_config->clock_freq <= AFE_SPI_MAX_CLK);

  prv_crc8_build_table(afe);

  spi_init(afe->spi_config);

  // software reset chip
  prv_write(afe, AFE_DEVADDR_MASTER, AFE_CONTROL_LB, true,
            CTRL_LB_DAISY_CHAIN_RB_ENB | CTRL_LB_LOCK_DEVICE_ADDR |
            CTRL_LB_SW_RESET);

  // Example 1: Initializing the AD7280A in datasheet
  // lock to the new device addresses
  // prv_transfer_32_bits(afe, 0x01C2B6E2);
  prv_write(afe, AFE_DEVADDR_MASTER, AFE_CONTROL_LB, true,
            CTRL_LB_DAISY_CHAIN_RB_ENB | CTRL_LB_LOCK_DEVICE_ADDR);

  // set the read register to CONTROL_LB
  // prv_transfer_32_bits(afe, 0x038716CA);
  prv_write(afe, AFE_DEVADDR_MASTER, AFE_READ, true, READ_CONTROL_LB);

  int n;
  for (n = 0; n <= AFE_MAX_DAISY_CHAIN; ++n) {
    uint32_t val = prv_read_32_bits(afe);

    // if we receive an empty SPI frame, then no more devices are powered
    // and there are (n - 1) slave devices
    if (val == 0) {
      break;
    }

    if (!prv_crc8_check(afe, val)) {
      break;
    }

    // device address data is stored in D31:27, LSB first
    // so slave 1 has address 10000
    if (n != prv_reverse_bits(val >> 27)) {
      break;
    }
  }

  afe->devices = n;
  return n;
}

// read data from a device register
uint8_t afe_read_register(struct AFEConfig *afe, uint8_t devaddr, uint8_t readreg) {
  // flip bits of devaddr, so caller can refer to devices 0, 1, ...
  devaddr = prv_reverse_bits(devaddr);

  // refer to Example 6 in datasheet
  // todo: either Example 6 (step 1) or Example 4 (step 2) is wrong,
  // the same operation has different 32-bit write commands (Peter warned us)

  // turn off read operation on all parts
  // prv_transfer_32_bits(afe, 0x01A6151A);
  prv_write(afe, AFE_DEVADDR_MASTER, AFE_CONTROL_HB, true,
            CTRL_HB_CONV_INPUT_ALL | CTRL_HB_CONV_RSLT_READ_DSBL);

  // enable read operation on selected device
  prv_write(afe, devaddr, AFE_CONTROL_HB, false,
            CTRL_HB_CONV_INPUT_ALL | CTRL_HB_CONV_RSLT_READ_ALL);

  // configure read register with register to read from
  prv_write(afe, devaddr, AFE_READ, false, readreg);

  uint32_t reply = prv_read_32_bits(afe);

  if (!prv_crc8_check(afe, reply)) {
    // crc error flag will be raised
    return 0;
  }

  // device address is bits D31:27
  // register address is bits D26:21
  // read registers are just the register << 2, so we shift 2 to check if the register matches
  if (((reply >> 27) != devaddr) || (((reply >> 21) & 0x3F) != (readreg >> 2))) {
    // todo: maybe change this error, or think about returning values in pointer?
    return 0;
  }

  // data bits are D20:D13
  return (reply >> 13) & 0xFF;
}

// enable cell balancing for the given cells
bool afe_set_cbx(struct AFEConfig *afe, uint16_t devaddr, uint8_t cells) {
  devaddr = prv_reverse_bits(devaddr);

  prv_write(afe, devaddr, AFE_CELL_BALANCE, false, cells);

  return afe->crc_error;
}

// read back all VIN and AUX inputs on all channels
uint32_t afe_read_all_conversions(struct AFEConfig *afe, struct ConversionResult *cr) {
  // see Example 2 in datasheet
  // prv_transfer_32_bits(afe, 0x038011CA);
  prv_write(afe, AFE_DEVADDR_MASTER, AFE_READ, true, READ_CELL_VOLTAGE_1);

  // prv_transfer_32_bits(afe, 0x01A0131A);
  prv_write(afe, AFE_DEVADDR_MASTER, AFE_CONTROL_HB, true,
            CTRL_HB_CONV_RSLT_READ_ALL | CTRL_HB_CONV_INPUT_ALL);

  // prv_transfer_32_bits(afe, 0x03A0546A);
  prv_write(afe, AFE_DEVADDR_MASTER, AFE_CNVST_CONTROL, false, CNVST_IN_PULSE);

  io_set_state(afe->cnvst, IO_LOW);
  __delay_cycles(50000);
  io_set_state(afe->cnvst, IO_HIGH);

  // read VIN
  // even if every value is 13 bits, then we need at most
  // 8 * (6 * 2^13) => 19 bits of storage
  uint8_t dev, input;
  uint32_t reply, vtotal;
  for (dev = 0; dev < afe->devices; ++dev) {
    for (input = 0; input < 6; ++input) {
      reply = prv_read_32_bits(afe);

      cr->vin[dev * 6 + input] = prv_convert_voltage(reply);
      vtotal += cr->vin[dev * 6 + input];
    }
  }
  // read AUX
  for (dev = 0; dev < afe->devices; ++dev) {
    for (input = 0; input < 6; ++input) {
      reply = prv_read_32_bits(afe);

      cr->aux[dev * 6 + input] = prv_convert_temp(reply);
    }
  }

  return vtotal;
}
