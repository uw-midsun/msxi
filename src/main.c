#include "config.h"
#include "wdt_a.h"
#include "drivers/timer.h"
#include "drivers/led.h"
#include <stdio.h>

static struct ConversionResult cr = {
  .vin = { 0 },
  .aux = { 0 }
};

static int32_t current = 0;
static uint8_t devices = 0;
static uint32_t vtotal = 0;
static uint32_t v_avg = 0;

// todo: move these functions?
// return true if the voltage is in the given range
//   [vthresh.low, vthresh.high)
static bool check_in_range(uint16_t voltage, struct Threshold vthresh) {
  return (voltage >= vthresh.low && voltage < vthresh.high);
}

static void prv_send_temp(uint8_t device_input, int cell_temp) {
  const struct CANMessage can_temp = {
    .id = PLUTUS_TEMP_READING,
    .data_u16 = {
      device_input,
      cell_temp
    }
  };

  can_transmit(&can, &can_temp);
}

void prv_send_balance(uint8_t device_input, uint8_t cells_to_balance) {
  const struct CANMessage can_balance = {
    .id = PLUTUS_BALANCE_STRING,
    .data_u16 = {
      device_input,
      cells_to_balance >> 2
    }
  };

  can_transmit(&can, &can_balance);
}

static void prv_send_current(int16_t current) {
  const struct CANMessage can_current = {
    .id = PLUTUS_CURRENT_READING,
    .data_u16 = {
      current
    }
  };
  can_transmit(&can, &can_current);
}

static void prv_send_num_devices(uint8_t devices) {
  const struct CANMessage can_devices = {
    .id = PLUTUS_NUM_DEVICES,
    .data_u8 = {
      devices
    }
  };

  can_transmit(&can, &can_devices);
}

static void prv_send_voltage(uint8_t device_input, uint16_t cell_voltage) {
  const struct CANMessage can_voltage = {
    .id = PLUTUS_VOLTAGE_READING,
    .data_u16 = {
      device_input,
      cell_voltage
    }
  };

  can_transmit(&can, &can_voltage);
}

static void send_can_messages(uint16_t elapsed_ms, void* context) {
  uint8_t device, input;
  for (device = 0; device < afe.devices; ++device) {
    for (input = 0; input < 6; ++input) {
      uint8_t num = device * 6 + input;
      uint16_t cell_voltage = cr.vin[num];
      if (device + 1 == afe.devices && input == 4) {
        cell_voltage += cr.vin[device * 6 + input + 1];
        ++input;
      }
      prv_send_voltage(num, cell_voltage);
    }
    for (input = 0; input < 3; ++input) {
      uint8_t num = device * 3 + input;
      // todo: we skip the third device readings because of a hardware issue
      if (device == 2) {
        continue;
      }
      int16_t cell_temp = cr.aux[device * 6 + input];
      prv_send_temp(num, cell_temp);
    }
  }

  prv_send_current(current);
}

static void get_afe_results(uint16_t elapsed_ms, void* context) {
  uint8_t device = 0;
  for (device = 0; device < afe.devices; ++device) {
    afe_set_cbx(&afe, device, 0);
  }
  __delay_cycles(10000);

  vtotal = afe_read_all_conversions(&afe, &cr);
  v_avg = vtotal / (afe.devices * 6 - 1);
}

int main() {
  WDT_A_hold(WDT_A_BASE);
  // 10kHz / 32k => 0.3125 Hz = 3.2s
//  WDT_A_initWatchdogTimer(WDT_A_BASE, WDT_A_CLOCKSOURCE_VLOCLK, WDT_A_CLOCKDIVIDER_512K);
//  WDT_A_start(WDT_A_BASE);

  __enable_interrupt();

  io_set_dir(&heartbeat, PIN_OUT);
  io_set_state(&heartbeat, IO_HIGH);

  io_set_dir(&spst, PIN_OUT);
  io_set_state(&spst, IO_HIGH);

  int k;
  for (k = 0; k < 8; ++k) {
    led_init(&leds[k]);
  }

  io_set_dir(&alert, PIN_IN);

  // rails
  io_set_peripheral_dir(&rail_5000mV, PIN_IN);
  io_set_peripheral_dir(&rail_3300mV, PIN_IN);

  devices = afe_init(&afe);

  for (k = 0; k < 3; ++k) {
    LEDState state = (((devices >> k) & 1) == 0) ? LED_OFF : LED_ON;
    led_set_state(&leds[k], state);
  }

  can_init(&can);
  prv_send_num_devices(devices);

  adc12_init(&adc12);
  current_sensor_init(&pwm);

  struct TempThreshold temperature = afe.d_temp;

  // every 5s
  timer_delay_periodic(5000, send_can_messages, NULL);
  timer_delay_periodic(3000, get_afe_results, NULL);
  vtotal = afe_read_all_conversions(&afe, &cr);
  v_avg = vtotal / (afe.devices * 6 - 1);

  timer_init();
  uint8_t device, input;
  while (true) {
    WDT_A_resetTimer(WDT_A_BASE);

    // find minimum voltage for balancing
    uint16_t min_voltage = 65535;
    for (device = 0; device < afe.devices; ++device) {
      for (input = 0; input < 6; ++input) {
        uint16_t cell_voltage = cr.vin[device * 6 + input];
        // todo: we're adding the last two VIN inputs together and skipping below
        // because of an unknown hardware issue
        if (device + 1 == afe.devices && input == 4) {
          cell_voltage = cr.vin[device * 6 + input + 1];
          ++input;
        }

        if (cell_voltage > 3200) {
          if (cell_voltage < min_voltage) {
            min_voltage = cell_voltage;
          }
        }
      }
    }

    for (device = 0; device < afe.devices; ++device) {
      uint8_t cells_to_balance = 0;
      for (input = 0; input < 6; ++input) {
        uint16_t cell_voltage = cr.vin[device * 6 + input];
        if (device + 1 == afe.devices && input == 4) {
          // todo: we're adding the last two VIN inputs together and skipping below
          // because of an unknown hardware issue
          cell_voltage += cr.vin[device * 6 + input + 1];
        }

        if (!check_in_range(cell_voltage, danger_threshold)) {
          io_set_state(&heartbeat, IO_LOW);
          io_set_state(&spst, IO_LOW);

          led_set_state(&leds[4], LED_ON);
        }

        // balance string uses flags with bits corresponding to the battery
        // d0 and d1 are reserved, so bit 2 + cell gives us the correct bit
        // we limit to be within 20mV of the lowest cell
        if (cell_voltage > min_voltage + 20) {
          cells_to_balance |= 1 << (2 + input);
        }

        if (device + 1 == afe.devices && input == 4) {
          // todo: we're adding the last two VIN inputs together and skipping below
          // because of an unknown hardware issue
          input = 5;
        }
      }
      afe_set_cbx(&afe, device, cells_to_balance);

      for (input = 0; input < 3; ++input) {
        // todo: we're throwing away the third module's AUX inputs because
        // of a hardware problem
        if (device == 2) {
          continue;
        }
        int16_t cell_temp = cr.aux[device * 6 + input];
        if (cell_temp > temperature.high || cell_temp < temperature.low) {
          io_set_state(&heartbeat, IO_LOW);
          io_set_state(&spst, IO_LOW);

          led_set_state(&leds[5], LED_ON);
        }
      }
    }

    // check currents
    current = current_sensor_read();
    if (current > 150) { // discharge
      led_set_state(&leds[3], LED_OFF);
      temperature = afe.d_temp;
    } else { // charge
      led_set_state(&leds[3], LED_ON);
      temperature = afe.c_temp;
    }

    // check rails
    uint32_t rail_5000mV = (adc12_raw(&adc12, ADC12_MEM0) * (uint32_t)6600) / 4095;
    uint32_t rail_3300mV = (adc12_raw(&adc12, ADC12_MEM1) * (uint32_t)6600) / 4095;

    if (!check_in_range(rail_5000mV, rail_thresh_5000mV) ||
        !check_in_range(rail_3300mV, rail_thresh_3300mV)) {
      io_set_state(&heartbeat, IO_LOW);
      io_set_state(&spst, IO_LOW);

      led_set_state(&leds[6], LED_ON);
    }

    // 1A -> 100cA
    if (current > 150) { // discharge
      // 70A
      if (current > 7000) {
        io_set_state(&heartbeat, IO_LOW);
        io_set_state(&spst, IO_LOW);

        led_set_state(&leds[7], LED_ON);
      }
    }
    else { // charge is negative
      if (current < -2000) { // -20A
        io_set_state(&heartbeat, IO_LOW);
        io_set_state(&spst, IO_LOW);

        led_set_state(&leds[7], LED_ON);
      }
    }

    timer_process();
  }
}

#pragma vector = PORT1_VECTOR
__interrupt void PORT1_ISR(void) {
  const struct IOMap interrupt_pin = { GPIO_PORT_P1, GPIO_PIN0 };
  io_process_interrupt(&interrupt_pin);
}
