#include "can.h"
#include "mcp2515.h"
#include <stdio.h>

struct TXBuffer {
  uint8_t id;
  uint8_t data;
  uint8_t rts;
};

struct RXBuffer {
  uint8_t id;
  uint8_t data;
};

static void prv_reset(const struct CANConfig *can) {
  spi_select(can->spi);
  spi_transmit(can->spi, MCP_RESET);
  spi_deselect(can->spi);
}

// Reads the specified number of consecutive registers into data.
static void prv_read(const struct CANConfig *can, uint8_t address, uint8_t *data, uint8_t bytes) {
  spi_select(can->spi);
  spi_transmit(can->spi, MCP_READ);
  spi_transmit(can->spi, address);

  uint16_t i;
  for (i = 0; i < bytes; i++) {
    data[i] = spi_receive(can->spi);
  }

  spi_deselect(can->spi);
}

// Writes the specified number of consecutive registers, beginning from address.
// Note that each register is 1 byte.
static void prv_write(const struct CANConfig *can,
                      uint8_t address, const uint8_t *data, uint8_t bytes) {
  spi_select(can->spi);
  spi_transmit(can->spi, MCP_WRITE);

  spi_transmit(can->spi, address);
  spi_transmit_array(can->spi, data, bytes);

  spi_deselect(can->spi);
}

// Modifies the specified address with the given mask applied to data.
static void prv_bit_modify(const struct CANConfig *can,
                           uint8_t address, uint8_t mask, uint8_t data) {
  spi_select(can->spi);
  uint8_t payload[] = { MCP_BIT_MODIFY, address, mask, data };
  spi_transmit_array(can->spi, payload, 4);
  spi_deselect(can->spi);
}

static void prv_write_id(const struct CANConfig *can, uint8_t address, uint16_t id) {
  // IDs are stored 8 bits in SIDH, 3 bits in SIDL so we shift the id and break it into pieces.
  uint8_t id_parts[2] = {(id >> 3), (id << 5)};
  prv_write(can, address, id_parts, 2);
}

void can_init(const struct CANConfig *can) {
  spi_init(can->spi);

  if (can->reset_pin.port != 0) {
    // Reset pin - Active-low
    io_set_dir(&can->reset_pin, PIN_OUT);
    io_set_state(&can->reset_pin, IO_HIGH);
  }

  prv_reset(can);

  // Set to Config mode, CLKOUT /4
  prv_bit_modify(can, CANCTRL, OPMODE_MASK | CLKOUT_MASK, OPMODE_CONFIG | CLKPRE_4);

  // TODO: rewrite with bit_modify?

  static const uint8_t registers[] = {
    0x05, // CNF3: 250 kbps -> PS2 Length = 5
    BTLMODE_CNF3 | SAMPLE_3X | (0x06 << 3) | 0x01, // PS1 Length = 6, PRSEG Length = 1
    0x01, // CNF1: BRP = 1
    EFLAG | RX1IE | RX0IE, // CANINTE: Enable error and receive interrupts
    0x00, // CANINTF: clear all IRQ flags
    0x00 // EFLG: clear all error flags
  };
  prv_write(can, CNF3, registers, 6);

  // Set up acceptance masks and filters
  prv_write_id(can, RXM0SIDH, can->rxb0.mask);
  prv_write_id(can, RXF0SIDH, can->rxb0.filter[0]);
  prv_write_id(can, RXF1SIDH, can->rxb0.filter[1]);

  prv_write_id(can, RXM1SIDH, can->rxb1.mask);
  prv_write_id(can, RXF2SIDH, can->rxb1.filter[0]);
  prv_write_id(can, RXF3SIDH, can->rxb1.filter[1]);
  prv_write_id(can, RXF4SIDH, can->rxb1.filter[2]);
  prv_write_id(can, RXF5SIDH, can->rxb1.filter[3]);

  // Normal mode
  prv_bit_modify(can, CANCTRL, OPMODE_MASK, OPMODE_NORMAL);

  io_set_dir(&can->interrupt_pin, PIN_IN);
  io_configure_interrupt(&can->interrupt_pin, true, EDGE_FALLING);

  // For debug
  uint8_t control_registers[8] = { 0 };
  prv_read(can, CNF3, control_registers, 8);
  _nop();
}

static void prv_transmit_buffer(const struct CANConfig *can,
                                const struct TXBuffer *buffer, const struct CANMessage *msg) {
  // Load ID
  spi_select(can->spi);
  spi_transmit(can->spi, MCP_LOAD_TX | buffer->id);
  // IDs are stored 8 bits in SIDH, 3 bits in SIDL so we shift the id and break it into pieces.
  // Disable extended ID, then set the RTR bit and data length.
  uint8_t id[] = {
      (msg->id >> 3), // SIDH
      (msg->id << 5), // SIDL
      0x00, // EID8
      0x00, // EID0
      (msg->rtr << RTR_SHIFT) | 8 // DLC
  };
  spi_transmit_array(can->spi, id, 5);
  spi_deselect(can->spi);

  // Load data
  spi_select(can->spi);
  spi_transmit(can->spi, MCP_LOAD_TX | buffer->data);

  // Can't use transmit_array due to endianness
  int i;
  for (i = 7; i >= 0; i--) {
    spi_transmit(can->spi, msg->data_u8[i]);
  }

  spi_deselect(can->spi);

  // Send message
  spi_select(can->spi);
  spi_transmit(can->spi, MCP_RTS | buffer->rts);
  spi_deselect(can->spi);

  // For debug
  uint8_t registers[14] = { 0 };
  prv_read(can, TXB0CTRL, registers, 14);
  _nop();
}

static uint8_t prv_read_status(const struct CANConfig *can) {
  spi_select(can->spi);
  spi_transmit(can->spi, MCP_READ_STATUS);
  uint8_t status = spi_receive(can->spi);
  spi_deselect(can->spi);

  return status;
}

void can_transmit(const struct CANConfig *can, const struct CANMessage *msg) {
  static const struct TXBuffer txb0 = {
    .id = LOAD_TXB0SIDH,
    .data = LOAD_TXB0D0,
    .rts = RTS_TXB0
  }, txb1 = {
    .id = LOAD_TXB1SIDH,
    .data = LOAD_TXB1D0,
    .rts = RTS_TXB1
  }, txb2 = {
    .id = LOAD_TXB2SIDH,
    .data = LOAD_TXB2D0,
    .rts = RTS_TXB2
  };

  uint8_t status = (prv_read_status(can) & (STATUS_TX0REQ | STATUS_TX1REQ | STATUS_TX2REQ));
  struct TXBuffer const *txbuffer = NULL;

  if ((status & STATUS_TX0REQ) == 0) {
    // TX0 free
    txbuffer = &txb0;
  } else if ((status & STATUS_TX1REQ) == 0) {
    // TX1 free
    txbuffer = &txb1;
  } else {
    // TX2 is probably free. Should probably check.
    txbuffer = &txb2;
  }

  prv_transmit_buffer(can, txbuffer, msg);
}

static void prv_receive_buffer(const struct CANConfig *can,
                               const struct RXBuffer *buffer, struct CANMessage *msg) {
  spi_select(can->spi);
  spi_transmit(can->spi, MCP_READ_RX | buffer->id);

  // IDs are stored 8 bits in SIDH, 3 bits in SIDL so we need to shift it back.
  union ID {
    uint16_t data;
    uint8_t part[2];
  } id;

  // Little endian
  id.part[1] = spi_receive(can->spi); // SIDH
  id.part[0] = spi_receive(can->spi); // SIDL

  // Shift the 11 bit ID back
  msg->id = (id.data >> 5);

  spi_receive(can->spi); // EID8
  spi_receive(can->spi); // EID0

  msg->rtr = ((spi_receive(can->spi) & RTR_FRAME) != 0); // DLC

  spi_deselect(can->spi);
  spi_select(can->spi);
  spi_transmit(can->spi, MCP_READ_RX | buffer->data);

  // Little endian
  int i;
  for (i = 7; i >= 0; i--) {
    msg->data_u8[i] = spi_receive(can->spi);
  }

  spi_deselect(can->spi);
}

void can_receive(const struct CANConfig *can, struct CANMessage *msg) {
  static const struct RXBuffer rxb0 = {
    .id = READ_RXB0SIDH,
    .data = READ_RXB0D0
  }, rxb1 = {
    .id = READ_RXB1SIDH,
    .data = READ_RXB1D0
  };

  uint8_t status = prv_read_status(can);

  // Clear the message (in case of error)
  msg->id = 0x00;
  msg->data = 0x00;
  msg->rtr = false;

  if (status & STATUS_RX0IF) {
    // RX0 has a message
    prv_receive_buffer(can, &rxb0, msg);
  } else if (status & STATUS_RX1IF) {
    // RX1 has a message
    prv_receive_buffer(can, &rxb1, msg);
  }
}

void can_check_error(const struct CANConfig *can, struct CANError *error) {
  prv_read(can, EFLG, &error->flags, 1);
  prv_read(can, TEC, &error->tx_error_count, 1);
  prv_read(can, REC, &error->rx_error_count, 1);

  uint8_t clear = 0x00;
  prv_write(can, EFLG, &clear, 1);
  prv_write(can, TEC, &clear, 1);
  prv_write(can, REC, &clear, 1);
}

bool can_process_interrupt(const struct CANConfig *can,
                           struct CANMessage *msg, struct CANError *error) {
  // TODO: flag checking is already handled in can_receive. Can we remove it from here?
  uint8_t flags;
  prv_read(can, CANINTF, &flags, 1);

  if (flags & EFLAG) {
    can_check_error(can, error);
  }

  if (flags & (RX0IE | RX1IE)) {
    can_receive(can, msg);
  }

  return (flags & (EFLAG | RX0IE | RX1IE)) != 0;
}
