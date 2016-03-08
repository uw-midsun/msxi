#pragma once
#include "spi.h"

// CAN driver
// This uses an interrupt-driven SPI driver, so CAN messages cannot be processed in an ISR.
// Look at the protocol doc for an explanation of our ID system.

#define NO_RESET_PIN { 0, 0 }

struct CANConfig {
  const struct SPIConfig *spi;
  struct IOMap interrupt_pin;
  struct IOMap reset_pin; // Optional reset pin
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
    float data_f[2];
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

// Returns whether we processed anything
bool can_process_interrupt(const struct CANConfig *can,
                           struct CANMessage *msg, struct CANError *error);
