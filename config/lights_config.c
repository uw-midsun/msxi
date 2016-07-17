#include "can/config.h"
#include "config.h"

struct LightConfig light_config = {
  .spi = {
    .data_out = { GPIO_PORT_P3, GPIO_PIN1 },
    .data_in = { GPIO_PORT_P3, GPIO_PIN2 },
    .clock_out = { GPIO_PORT_P3, GPIO_PIN3 },
    .cs = { GPIO_PORT_P3, GPIO_PIN0 },
    .clock_freq = 500000,
    .port = SPI_B0
  },
  .can =
  {
    .spi = &light_config.spi,
    .interrupt_pin = { GPIO_PORT_P2, GPIO_PIN7 },
    .reset_pin = NO_RESET_PIN,
    .rxb0 = {
      .mask = 0,
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
  },
  .signals =
  {
    {
      .can_msg_id = THEMIS_SIG_LEFT,
      .type = LIGHT_BLINK,
      .signal_pins =
      {
        {GPIO_PORT_P1, GPIO_PIN7}, // Front left
        {GPIO_PORT_P1, GPIO_PIN0}, // Brake left
        {GPIO_PORT_P1, GPIO_PIN1},
      },
    },
    {
      .can_msg_id = THEMIS_SIG_RIGHT,
      .type = LIGHT_BLINK,
      .signal_pins = {
        {GPIO_PORT_P1, GPIO_PIN5}, // Front right
        {GPIO_PORT_P1, GPIO_PIN2}, // Brake right
        {GPIO_PORT_P1, GPIO_PIN3},
      },
    },
    {
      .can_msg_id = THEMIS_SIG_BRAKE,
      .type = LIGHT_BRAKE,
      .signal_pins = {
        {GPIO_PORT_P1, GPIO_PIN0}, // Brake left
        {GPIO_PORT_P1, GPIO_PIN1},
        {GPIO_PORT_P1, GPIO_PIN2}, // Brake right
        {GPIO_PORT_P1, GPIO_PIN3},
        {GPIO_PORT_P1, GPIO_PIN4}, // Brake center
      },
    },
    {
      .can_msg_id = THEMIS_SIG_RUNNING,
      .type = LIGHT_RUNNING,
      .signal_pins = {
        {GPIO_PORT_P1, GPIO_PIN6}
      },
    },
    {
      .can_msg_id = THEMIS_SIG_HAZARD,
      .type = LIGHT_BLINK,
      .signal_pins = {
          {GPIO_PORT_P1, GPIO_PIN7}, // Front left
          {GPIO_PORT_P1, GPIO_PIN0}, // Brake left
          {GPIO_PORT_P1, GPIO_PIN1},
          {GPIO_PORT_P1, GPIO_PIN5}, // Front right
          {GPIO_PORT_P1, GPIO_PIN2}, // Brake right
          {GPIO_PORT_P1, GPIO_PIN3},
          {GPIO_PORT_P1, GPIO_PIN4}
      },
    },
  },
};

