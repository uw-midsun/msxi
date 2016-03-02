#include "can/config.h"
#include "config.h"

const struct SPIConfig spi_b1 = {
  .data_out = { GPIO_PORT_P4, GPIO_PIN4 },
  .data_in = { GPIO_PORT_P4, GPIO_PIN5 },
  .clock_out = { GPIO_PORT_P4, GPIO_PIN0 },
  .cs = { GPIO_PORT_P4, GPIO_PIN0 },
  .clock_freq = 500000,
  .port = SPI_B1
};

const struct CANConfig can = {
  .spi = &spi_b1,
  .interrupt_pin = { GPIO_PORT_P4, GPIO_PIN7 },
  .rxb0 = {
    .mask = CAN_DEVICE_MASK,
    .filter = {
    	DEVICE_THEMIS,
    	CAN_FULL_MASK
    }
  },
  .rxb1 = {
    .mask = CAN_FULL_MASK,
    .filter = {
      CAN_FULL_MASK,
      CAN_FULL_MASK,
      CAN_FULL_MASK,
      CAN_FULL_MASK
    }
  }
};

const struct IOMap can_interrupt = {GPIO_PORT_P4, GPIO_PIN7};

const struct IOMap left_turn_lights[MAX_LIGHTS] = {
  {GPIO_PORT_P1, GPIO_PIN7},
};

const struct IOMap right_turn_lights[MAX_LIGHTS] = {
  {GPIO_PORT_P1, GPIO_PIN5},
};

const struct IOMap running_lights[MAX_LIGHTS] = {
  {GPIO_PORT_P1, GPIO_PIN6},
};

const struct IOMap break_lights[MAX_LIGHTS] = {
  {GPIO_PORT_P1, GPIO_PIN0},
  {GPIO_PORT_P1, GPIO_PIN2},
};
