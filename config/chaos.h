/*
  pins.h - Titus Chow
  
  Pin configuration - Should be { port, pins }.
  This is included whenever io_map is used.

  Use IOMAP(map) to convert a pin configuration to the format &(struct IOMAP) { port, pins }

  Note: This pin config is for Chaos V2.
*/
#ifndef PINS_H_
#define PINS_H_
#include "gpio.h"

// Due to how we're using the macros, we can't wrap "map" in brackets.
// Thus, it's up to the user not to call IOMAP(map) with other arguments.
#define IOMAP(map)      &(struct IOMap) map
#define GPIO_PIN_ALL    GPIO_PIN0 + GPIO_PIN1 + GPIO_PIN2 + GPIO_PIN3 + \
              GPIO_PIN4 + GPIO_PIN5 + GPIO_PIN6 + GPIO_PIN7

// CAN1

// SPI1
#define SPI1_SCK      { GPIO_PORT_P3, GPIO_PIN0 }
#define SPI1_SIMO      { GPIO_PORT_P3, GPIO_PIN4 }
#define SPI1_SOMI      { GPIO_PORT_P3, GPIO_PIN5 }

// Relays
#define BATTERY_RELAY    { GPIO_PORT_P3, GPIO_PIN2 }
#define BATTERY_STATUS    { GPIO_PORT_P3, GPIO_PIN7 }

#define SOLAR_RELAY      { GPIO_PORT_P4, GPIO_PIN0 }
#define SOLAR_STATUS    { GPIO_PORT_P4, GPIO_PIN1 }

#define LEFT_MC_RELAY    { GPIO_PORT_P4, GPIO_PIN2 }
#define LEFT_MC_STATUS    { GPIO_PORT_P4, GPIO_PIN3 }

#define RIGHT_MC_RELAY    { GPIO_PORT_P4, GPIO_PIN4 }
#define RIGHT_MC_STATUS    { GPIO_PORT_P4, GPIO_PIN5 }

#define CHG_LEFT_RELAY    { GPIO_PORT_P4, GPIO_PIN7 }
#define DCHG_LEFT_RELAY    { GPIO_PORT_P5, GPIO_PIN4 }

#define CHG_RIGHT_RELAY    { GPIO_PORT_P5, GPIO_PIN5 }
#define DCHG_RIGHT_RELAY  { GPIO_PORT_P7, GPIO_PIN2 }

// Heartbeat
#define PLUTUS_HEARTBEAT  { GPIO_PORT_P4, GPIO_PIN6 }

// ADC (Voltage measurement)
// Allows measurement of precharge/discharge circuits
#define ENABLE_MEASUREMENT  { GPIO_PORT_P6, GPIO_PIN4 }
// Remember to modify mcu_voltage's memory buffers if these pins are changed!
#define MC_VOLTAGES      { GPIO_PORT_P6, GPIO_PIN0 + GPIO_PIN1 + \
                      GPIO_PIN2 + GPIO_PIN3 }
/*#define CHG_LEFT_VOLTAGE  { GPIO_PORT_P6, GPIO_PIN0 }
#define DCHG_LEFT_VOLTAGE  { GPIO_PORT_P6, GPIO_PIN1 }
#define CHG_RIGHT_VOLTAGE  { GPIO_PORT_P6, GPIO_PIN2 }
#define DCHG_RIGHT_VOLTAGE  { GPIO_PORT_P6, GPIO_PIN3 } */
#define POWER_STATUS    { GPIO_PORT_P7, GPIO_PIN7 }

// Debug LEDs - TODO: rename to more descriptive names
#define DEBUG_LED_0      { GPIO_PORT_P8, GPIO_PIN0 }
#define DEBUG_LED_1      { GPIO_PORT_P8, GPIO_PIN1 }
#define DEBUG_LED_2      { GPIO_PORT_P8, GPIO_PIN2 }
#define DEBUG_LED_3      { GPIO_PORT_P8, GPIO_PIN3 }
#define DEBUG_LED_4      { GPIO_PORT_P8, GPIO_PIN4 }
#define DEBUG_LED_5      { GPIO_PORT_P8, GPIO_PIN5 }
#define DEBUG_LED_7      { GPIO_PORT_P8, GPIO_PIN7 }
#define DEBUG_LED_PORT8    { GPIO_PORT_P8, GPIO_PIN_ALL }

#endif /* PINS_H_ */
