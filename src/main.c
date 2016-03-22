#include "drivers/afe.h"
#include "drivers/gpio_map.h"
#include "drivers/adc12.h"

int main() {
  const struct SPIConfig spi = {
    .data_out = { GPIO_PORT_P5, GPIO_PIN6 },
    .data_in = { GPIO_PORT_P5, GPIO_PIN7 },
    .clock_out = { GPIO_PORT_P3, GPIO_PIN6 },
    .cs = { GPIO_PORT_P5, GPIO_PIN5 },
    .clock_freq = 1000000,
    .port = SPI_A1
  };

  struct AFEConfig afe = {
    .spi_config = &spi,
    .cnvst = { GPIO_PORT_P10, GPIO_PIN0 },
    .slaves = 5,
    .v_charge = { 4224, 2700 },
    .v_discharge = { 4224, 2700 },
    .temp = { -20, 60 }
  };

  static const struct ADC12Config adc12 = {
    .buffers = {
      [ADC12_MEM0] = {
        .pin = { GPIO_PORT_P6, GPIO_PIN0 }, // 5V rail
        .source = ADC12INCH_0
      },
      [ADC12_MEM1] = {
        .pin = { GPIO_PORT_P6, GPIO_PIN1 }, // 3.3V rail
        .source = ADC12INCH_1
      }
    },
    .count = 2,
    .continuous = true
  };

  struct IOMap heartbeat = { GPIO_PORT_P8, GPIO_PIN0 };
  struct Threshold rail_thresh_05V = { 4.7, 5.3 }; // 4.7V -> something, 5.3V -> something
  struct Threshold rail_thresh_33V = { 3.0, 3.6 }; // 3.0V -> something, 3.6V -> something

  _enable_interrupt();

  afe_init(&afe);
  io_set_state(&heartbeat, IO_HIGH);
  adc12_init(&adc12);

  while (true) {
    uint16_t device;
    uint16_t input;

    for (device = 0; device < afe.devices; ++device) {
      // todo: check current and determine whether charging or discharging
      int overvoltage, undervoltage;
      uint8_t cells_to_balance = 0;

      for (input = 0; input < 6; ++input) {
        // cell string voltage
        uint16_t cell_voltage = afe_voltage_conversion(&afe, device, input);

        if (cell_voltage > overvoltage || cell_voltage < undervoltage) {
          io_set_state(&heartbeat, IO_LOW);
        }

        // we want to limit voltages based on a logarithmic threshold
        // [0, 4] -> limit to +.75V
        // [4, 4.15] -> limit to 4.15
        // [4.15, 4.2] -> limit to 4.2

        // balance string uses flags with bits corresponding to the battery
        // d0 and d1 are reserved, so bit 2 + cell gives us the correct bit
        //if (cell_voltage > ...) {
        //  cells_to_balance |= 1 << (2 + input);
        //}

        // check temperature (aux adc) is within [-20, 60]
        uint16_t cell_temp = afe_aux_conversion(&afe, device, input);
        if (cell_temp > afe.temp.high || cell_temp < afe.temp.low) {
          io_set_state(&heartbeat, IO_LOW);
        }
      }

      // check voltages on 5V and 3.3V rails
      uint16_t rail_05V = adc12_sample(&adc12, ADC12_MEM0);
      uint16_t rail_33V = adc12_sample(&adc12, ADC12_MEM1);

      if ((rail_05V < rail_thresh_05V.low || rail_05V > rail_thresh_05V.high) ||
          (rail_33V < rail_thresh_33V.low || rail_33V > rail_thresh_33V.high)) {
        io_set_state(&heartbeat, IO_LOW);
      }

      // todo: check current for all cells using PWM value
      //if(((current > 75) || (current < -58)) && (current > -125) && (current < 125)) {
      //    io_set_state(&heartbeat, IO_LOW);
      //}


      // balance any cells needing balancing
      afe_set_cbx(&afe, device, cells_to_balance);
    }
  }
}

#pragma vector=

