#pragma once
#include "spi.h"

// CAN driver
// Look at the protocol doc for an explanation of our ID system.

#define CAN_DEVICE_MASK 0x7E0
#define CAN_MESSAGE_MASK 0x1F
#define CAN_ID(device, message) (((device) << 5) | (message))

struct CANConfig {
  struct SPIConfig *spi;
  struct IOMap interrupt_pin;
  struct {
    uint16_t mask;
    uint16_t filter[2];
  } rxb0;
  struct {
    uint16_t mask;
    uint16_t filter[4];
  } rxb1;
};

struct CANMessage {
  union {
    uint64_t data;
    uint32_t data_u32[2];
    uint16_t data_u16[4];
    uint8_t data_u8[8];
  };
  uint16_t id;
  bool rtr;
};

struct CANError {
  union {
    uint8_t flags;
    struct { // Little endian
      bool error_warning:1;
      bool rx_warning:1;
      bool tx_warning:1;
      bool rx_passive:1;
      bool tx_passive:1;
      bool tx_bus_off:1;
      bool rx0_overflow:1;
      bool rx1_overflow:1;
    };
  };
  uint8_t tx_error_count;
  uint8_t rx_error_count;
};

void can_init(const struct CANConfig *can);

void can_transmit(const struct CANConfig *can, const struct CANMessage *msg);

void can_receive(const struct CANConfig *can, struct CANMessage *msg);

void can_check_error(const struct CANConfig *can, struct CANError *error);

void can_process_interrupt(const struct CANConfig *can,
                           struct CANMessage *msg, struct CANError *error);
