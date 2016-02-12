#pragma once
#include "config.h"
#include "can/config.h"
#include "drivers/spi.h"
#include "drivers/adc12.h"

// Themis config

static const struct SPIConfig spi_b0 = {
  .data_out = { GPIO_PORT_P3, GPIO_PIN1 },
  .data_in = { GPIO_PORT_P3, GPIO_PIN2 },
  .clock_out = { GPIO_PORT_P3, GPIO_PIN3 },
  .cs = { GPIO_PORT_P3, GPIO_PIN0 },
  .clock_freq = 500000,
  .port = SPI_B0
};

const struct CANConfig can = {
  .spi = &spi_b0,
  .interrupt_pin = { GPIO_PORT_P2, GPIO_PIN7 },
  .rxb0 = {
    .mask = CAN_DEVICE_MASK,
    .filter = {
      DEVICE_LEFT_MC,
      DEVICE_RIGHT_MC
    }
  },
  .rxb1 = {
    .mask = CAN_IGNORE_ALL,
    .filter = { }
  }
};

static const struct ADC12Config adc12 = {
  .buffers = {
    [ADC12_MEM0] = {
      .pin = { GPIO_PORT_P6, GPIO_PIN0 },
      .source = INCH_0
    },
    [ADC12_MEM1] = {
      .pin = { GPIO_PORT_P6, GPIO_PIN1 },
      .source = INCH_1
    }
  },
  .count = 2,
  .continuous = true
};

struct InputConfig input = {
  .adc = &adc12,
  .polled = {
    {
      .event = CRUISE_DIR_POS,
      .input = { GPIO_PORT_P6, GPIO_PIN3 }
    }, {
      .event = CRUISE_DIR_NEG,
      .input = { GPIO_PORT_P6, GPIO_PIN2 }
    }
  },
  .isr = {
    {
      .event = SIG_L_TOGGLE,
      .input = { GPIO_PORT_P1, GPIO_PIN7 }
    }, {
      .event = SIG_R_TOGGLE,
      .input = { GPIO_PORT_P1, GPIO_PIN1 }
    }, {
      .event = HAZARD_TOGGLE,
      .input = { GPIO_PORT_P1, GPIO_PIN3 }
    }, {
      .event = HORN_TOGGLE,
      .input = { GPIO_PORT_P1, GPIO_PIN4 }
    }, {
      .event = IGNITION_TOGGLE,
      .input = { GPIO_PORT_P1, GPIO_PIN0 }
    }, {
      .event = CRUISE_TOGGLE,
      .input = { GPIO_PORT_P1, GPIO_PIN5 }
    }
  },
  .brake = {
    .edge = BRAKE_EDGE,
    .mech = { GPIO_PORT_P2, GPIO_PIN6 },
    .gain = {
      { GPIO_PORT_P2, GPIO_PIN0 },
      { GPIO_PORT_P2, GPIO_PIN1 },
      { GPIO_PORT_P2, GPIO_PIN2 },
      { GPIO_PORT_P2, GPIO_PIN3 },
      { GPIO_PORT_P2, GPIO_PIN4 },
      { GPIO_PORT_P2, GPIO_PIN5 }
    },
    .regen = {
      .event = BRAKE_CHANGE,
      .calibration = { // TODO
        .high = 4095,
        .low = 0
      },
      .input = ADC12_MEM0
    }
  },
  .throttle = {
    .pot = {
      .event = THROTTLE_CHANGE,
      .calibration = { // TODO
        .high = 4095,
        .low = 0
      },
      .input = ADC12_MEM1
    },
    .dir = {
      .forward = { GPIO_PORT_P6, GPIO_PIN5 },
      .backward = { GPIO_PORT_P6, GPIO_PIN4 }
    }
  }
};

const struct SignalConfig signals = {
  .can = &can,
  .packets = {
    {
      .e = SIG_L_TOGGLE,
      .can_id = THEMIS_SIG_LEFT
    }, {
      .e = SIG_R_TOGGLE,
      .can_id = THEMIS_SIG_RIGHT
    }, {
      .e = HAZARD_TOGGLE,
      .can_id = THEMIS_SIG_HAZARD
    }, {
      .e = BRAKE_EDGE,
      .can_id = THEMIS_SIG_BRAKE
    }, {
      .e = HORN_TOGGLE,
      .can_id = THEMIS_HORN
    }
  }
};

struct MCStateConfig mc_state = {
  .can = &can,
  .mc = {
    {
      .packet = {
        {
          .id = LEFT_MC_VELOCITY
        }, {
          .id = LEFT_MC_BUS
        }
      }
    }, {
      .packet = {
        {
          .id = RIGHT_MC_VELOCITY
        }, {
          .id = RIGHT_MC_BUS
        }
      }
    }
  }
};
