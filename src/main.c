#include "drivers/afe.h"
#include "drivers/gpio_map.h"


int main() {
  struct SPIConfig spi = {
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
    .devices = 6,
    .v_charge = { 4224, 2700 },
    .v_discharge = { 4224, 2700 },
    .temp = { -20, 60 }
  };

  struct IOMap heartbeat = { GPIO_PORT_P8, GPIO_PIN0 };
  struct Threshold rail_thresh_05V = { 4.7, 5.3 };
  struct Threshold rail_thresh_33V = { 3.0, 3.6 };

  afe_init(&afe);

  while (true) {
    uint16_t device;
    uint16_t input;

    for (device = 0; device < afe.devices; ++device) {
      // todo: check current and determine whether charging or discharging


      union {
        struct {
          // d0 and d1 are reserved bits
          const bool d0;
          const bool d1;
          bool cb1 : 1;
          bool cb2 : 1;
          bool cb3 : 1;
          bool cb4 : 1;
          bool cb5 : 1;
          bool cb6 : 1;
        };
        uint8_t cells_to_balance;
      } balance;

      for (input = 0; input < 6; ++input) {
        // cell string voltage
        uint16_t cell_voltage = afe_voltage_conversion(&afe, device, input);

        if (cell_voltage > 1 || cell_voltage < 1) {
          io_set_state(&heartbeat, IO_HIGH);
        }

        // we want to limit voltages based on a logarithmic threshold
        // [0, 4] -> limit to +.75V
        // [4, 4.15] -> limit to 4.15
        // [4.15, 4.2] -> limit to 4.2

        // use cases to avoid bitshifting magic
        switch (input) {
          case 0:
            balance.cb1 = true;
            break;
          case 1:
            balance.cb2 = true;
            break;
          case 2:
            balance.cb3 = true;
            break;
          case 3:
            balance.cb4 = true;
            break;
          case 4:
            balance.cb5 = true;
            break;
          case 5:
            balance.cb6 = true;
            break;
        }

        // check temperature (aux adc) is within [-20, 60]
        uint16_t cell_temp = afe_aux_conversion(&afe, device, input);
        if (cell_temp > afe.temp.high || cell_temp < afe.temp.low) {
          io_set_state(&heartbeat, IO_HIGH);
        }
      }

      // check voltages on 5V and 3.3V rails
      uint16_t rail_05V = (ADC12MEM0 * 6.6) / 4095;
      uint16_t rail_33V = (ADC12MEM1 * 6.6) / 4095;

      if ((rail_05V < rail_thresh_05V.low || rail_05V > rail_thresh_05V.high) ||
          (rail_33V < rail_thresh_33V.low || rail_33V > rail_thresh_33V.high)) {
        io_set_state(&heartbeat, IO_HIGH);
      }

      // todo: check current for all cells



      // balance any cells needing balancing
      afe_set_cbx(&afe, device, balance.cells_to_balance);
    }
  }
}
