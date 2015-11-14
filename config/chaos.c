#include "config.h"
#include "can_config.h"

// Config for Chaos

const struct SPIConfig spi_a0 = {
  .data_out = { GPIO_PORT_P3, GPIO_PIN4 },
  .data_in = { GPIO_PORT_P3, GPIO_PIN5 },
  .clock_out = { GPIO_PORT_P3, GPIO_PIN0 },
  .cs = { GPIO_PORT_P1, GPIO_PIN2 },
  .clock_freq = 500000,
  .port = SPI_A0
};

const struct CANConfig can = {
  .spi = &spi_a0,
  .interrupt_pin = { GPIO_PORT_P1, GPIO_PIN5 },
  .rxb0 = {
    .mask = CAN_DEVICE_MASK,
    .filter = {
      CAN_DEVICE_ID(DEVICE_THEMIS),
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

const struct Relay relay_battery = {
  .relay = { GPIO_PORT_P3, GPIO_PIN2 },
  .status = { GPIO_PORT_P3, GPIO_PIN7 }
}, relay_solar = {
  .relay = { GPIO_PORT_P4, GPIO_PIN0 },
  .status = { GPIO_PORT_P4, GPIO_PIN1 }
};

const struct MotorController mc_left = {
  .enable = {
    .relay = { GPIO_PORT_P4, GPIO_PIN2 },
    .status = { GPIO_PORT_P4, GPIO_PIN3 }
  },
  .charge = {
    .relay = { GPIO_PORT_P4, GPIO_PIN7 },
    .status = NO_STATUS_PIN
  },
  .discharge = {
    .relay = { GPIO_PORT_P5, GPIO_PIN4 },
    .status = NO_STATUS_PIN
  },
  .charge_index = MC_CHG_LEFT,
  .discharge_index = MC_DCHG_LEFT
};

const struct MotorController mc_right = {
  .enable = {
    .relay = { GPIO_PORT_P4, GPIO_PIN4 },
    .status = { GPIO_PORT_P4, GPIO_PIN5 }
  },
  .charge = {
    .relay = { GPIO_PORT_P5, GPIO_PIN5 },
    .status = NO_STATUS_PIN
  },
  .discharge = {
    .relay = { GPIO_PORT_P7, GPIO_PIN2 },
    .status = NO_STATUS_PIN
  },
  .charge_index = MC_CHG_RIGHT,
  .discharge_index = MC_DCHG_RIGHT
};

const struct ADC12Config adc12_a = {
  .enable = { GPIO_PORT_P6, GPIO_PIN4 },
  .buffers = {
    [PWR_STATUS] = {
      .pin = { GPIO_PORT_P7, GPIO_PIN7 },
      .source = ADC12_A_INPUT_A15
    },
    [MC_CHG_LEFT] = {
      .pin = { GPIO_PORT_P6, GPIO_PIN0 },
      .source = ADC12_A_INPUT_A0
    },
    [MC_DCHG_LEFT] = {
      .pin = { GPIO_PORT_P6, GPIO_PIN1 },
      .source = ADC12_A_INPUT_A1
    },
    [MC_CHG_RIGHT] = {
      .pin = { GPIO_PORT_P6, GPIO_PIN2 },
      .source = ADC12_A_INPUT_A2
    },
    [MC_DCHG_RIGHT] = {
      .pin = { GPIO_PORT_P6, GPIO_PIN3 },
      .source = ADC12_A_INPUT_A3
    }
  }
};

const struct IOMap plutus_heartbeat = { GPIO_PORT_P4, GPIO_PIN6 };

const struct SMDebugConfig sm_debug = {
  .can = &can,
  .leds = { // TODO: verify this is a row of LEDs LSB-first
    { GPIO_PORT_P8, GPIO_PIN7 },
    { GPIO_PORT_P8, GPIO_PIN6 },
    { GPIO_PORT_P8, GPIO_PIN5 },
    { GPIO_PORT_P8, GPIO_PIN4 },
    { GPIO_PORT_P8, GPIO_PIN3 },
    { GPIO_PORT_P8, GPIO_PIN2 },
    { GPIO_PORT_P8, GPIO_PIN1 },
    { GPIO_PORT_P8, GPIO_PIN0 }
  }
};
