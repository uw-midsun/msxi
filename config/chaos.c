#include "can/config.h"
#include "config.h"

// Config for Chaos

const struct SPIConfig spi_a0 = {
  .data_out = { GPIO_PORT_P3, GPIO_PIN4 },
  .data_in = { GPIO_PORT_P3, GPIO_PIN5 },
  .clock_out = { GPIO_PORT_P3, GPIO_PIN0 },
  .cs = { GPIO_PORT_P1, GPIO_PIN0 },
  .clock_freq = 500000,
  .port = SPI_A0
};

const struct CANConfig can = {
  .spi = &spi_a0,
  .interrupt_pin = { GPIO_PORT_P1, GPIO_PIN1 },
  .reset_pin = { GPIO_PORT_P1, GPIO_PIN2 },
  .rxb0 = {
    .mask = CAN_FULL_MASK,
    .filter = {
      THEMIS_HORN,
      CAN_FULL_MASK
    }
  },
  .rxb1 = {
    .mask = CAN_FULL_MASK,
    .filter = { }
  }
};

const struct ADC12Config adc12_a = {
  .buffers = {
    [ADC12_MEM0] = {
      .pin = { GPIO_PORT_P7, GPIO_PIN7 },
      .source = ADC12INCH_15
    },
    [ADC12_MEM1] = {
      .pin = { GPIO_PORT_P6, GPIO_PIN0 },
      .source = ADC12INCH_0
    },
    [ADC12_MEM2] = {
      .pin = { GPIO_PORT_P6, GPIO_PIN1 },
      .source = ADC12INCH_1
    },
    [ADC12_MEM3] = {
      .pin = { GPIO_PORT_P6, GPIO_PIN2 },
      .source = ADC12INCH_2
    },
    [ADC12_MEM4] = {
      .pin = { GPIO_PORT_P6, GPIO_PIN3 },
      .source = ADC12INCH_3
    }
  },
  .count = 5,
  .continuous = false
};

const struct Relay relay_battery = {
  .relay = { GPIO_PORT_P3, GPIO_PIN6 },
  .status = NO_STATUS_PIN
}, relay_solar = {
  .relay = { GPIO_PORT_P4, GPIO_PIN0 },
  .status = NO_STATUS_PIN
};

const struct MotorController mc_left = {
  .main = {
    .relay = { GPIO_PORT_P4, GPIO_PIN2 },
    .status = { GPIO_PORT_P4, GPIO_PIN3 }
  },
  .charge = {
    .relay = { GPIO_PORT_P5, GPIO_PIN4 },
    .status = NO_STATUS_PIN
  },
  .discharge = {
    .relay = { GPIO_PORT_P5, GPIO_PIN5 },
    .status = NO_STATUS_PIN
  },
  .charge_index = ADC12_MEM1,
  .discharge_index = ADC12_MEM2
};

const struct MotorController mc_right = {
  .main = {
    .relay = { GPIO_PORT_P4, GPIO_PIN4 },
    .status = { GPIO_PORT_P4, GPIO_PIN5 }
  },
  .charge = {
    .relay = { GPIO_PORT_P5, GPIO_PIN6 },
    .status = NO_STATUS_PIN
  },
  .discharge = {
    .relay = { GPIO_PORT_P5, GPIO_PIN7 },
    .status = NO_STATUS_PIN
  },
  .charge_index = ADC12_MEM3,
  .discharge_index = ADC12_MEM4
};

const struct MCConfig mc_config = {
  .adc = &adc12_a,
  .enable_measure = { GPIO_PORT_P5, GPIO_PIN1 },
  .num_mc = 2 ,
  .mc = {
    &mc_left,
    &mc_right
  }
};

const struct IOMap plutus_heartbeat = { GPIO_PORT_P1, GPIO_PIN5 },
                   horn = { GPIO_PORT_P7, GPIO_PIN6 },
                   enable_lv = { GPIO_PORT_P1, GPIO_PIN3 },
                   mppt_enable = { GPIO_PORT_P2, GPIO_PIN1 };

const struct SMDebugConfig sm_debug = {
  .can = &can,
  .state_leds = {
    { GPIO_PORT_P2, GPIO_PIN4 },
    { GPIO_PORT_P2, GPIO_PIN5 },
    { GPIO_PORT_P2, GPIO_PIN6 },
    { GPIO_PORT_P2, GPIO_PIN7 },
    { GPIO_PORT_P3, GPIO_PIN1 },
    { GPIO_PORT_P3, GPIO_PIN2 },
    { GPIO_PORT_P3, GPIO_PIN3 },
    { GPIO_PORT_P7, GPIO_PIN2 },
    { GPIO_PORT_P7, GPIO_PIN3 },
    { GPIO_PORT_P8, GPIO_PIN0 },
  },
  .event_leds = {
    { GPIO_PORT_P8, GPIO_PIN1 },
    { GPIO_PORT_P8, GPIO_PIN2 },
    { GPIO_PORT_P8, GPIO_PIN3 },
    { GPIO_PORT_P8, GPIO_PIN4 },
    { GPIO_PORT_P8, GPIO_PIN5 },
  },
  .fault_leds = {
    { GPIO_PORT_P8, GPIO_PIN6 },
    { GPIO_PORT_P8, GPIO_PIN7 },
    { GPIO_PORT_P9, GPIO_PIN0 },
    { GPIO_PORT_P9, GPIO_PIN1 },
    { GPIO_PORT_P9, GPIO_PIN4 },
  }
};

struct InputConfig input = {
  .power = { { GPIO_PORT_P6, GPIO_PIN7 } },
  .killswitch = { { GPIO_PORT_P6, GPIO_PIN5 } },
  .power_led = { GPIO_PORT_P11, GPIO_PIN1 },
  .select = { GPIO_PORT_P6, GPIO_PIN4 },
};
