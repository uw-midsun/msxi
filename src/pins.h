/*
  pins.h - Titus Chow
  
  Pin configuration - Should be &(struct IOMap) { port, pins }.
  This is included whenever io_map is used.

  Note: This pin config is for Chaos V2.
*/
#ifndef PINS_H_
#define PINS_H_
#include "gpio.h"

#define GPIO_PIN_ALL		GPIO_PIN0 + GPIO_PIN1 + GPIO_PIN2 + GPIO_PIN3 + \
							GPIO_PIN4 + GPIO_PIN5 + GPIO_PIN6 + GPIO_PIN7

// CAN1

// SPI1
#define SPI1_SCK			&(struct IOMap) { GPIO_PORT_P3, GPIO_PIN0 }
#define SPI1_SIMO			&(struct IOMap) { GPIO_PORT_P3, GPIO_PIN4 }
#define SPI1_SOMI			&(struct IOMap) { GPIO_PORT_P3, GPIO_PIN5 }

// Relays
#define BATTERY_RELAY		&(struct IOMap) { GPIO_PORT_P3, GPIO_PIN2 }
#define BATTERY_STATUS		&(struct IOMap) { GPIO_PORT_P3, GPIO_PIN7 }

#define SOLAR_RELAY			&(struct IOMap) { GPIO_PORT_P4, GPIO_PIN0 }
#define SOLAR_STATUS		&(struct IOMap) { GPIO_PORT_P4, GPIO_PIN1 }

// Due to the way motor controllers are declared, these are IOMaps without the compound literal.
#define LEFT_MC_RELAY		{ GPIO_PORT_P4, GPIO_PIN2 }
#define LEFT_MC_STATUS		{ GPIO_PORT_P4, GPIO_PIN3 }

#define RIGHT_MC_RELAY		{ GPIO_PORT_P4, GPIO_PIN4 }
#define RIGHT_MC_STATUS		{ GPIO_PORT_P4, GPIO_PIN5 }

#define CHG_LEFT_RELAY		{ GPIO_PORT_P4, GPIO_PIN7 }
#define DCHG_LEFT_RELAY		{ GPIO_PORT_P5, GPIO_PIN4 }

#define CHG_RIGHT_RELAY		{ GPIO_PORT_P5, GPIO_PIN5 }
#define DCHG_RIGHT_RELAY	{ GPIO_PORT_P7, GPIO_PIN2 }

// Heartbeat
#define PLUTUS_HEARTBEAT	&(struct IOMap) { GPIO_PORT_P4, GPIO_PIN6 }

// ADC (Voltage measurement)
// Allows measurement of precharge/discharge circuits
#define ENABLE_MEASUREMENT	&(struct IOMap) { GPIO_PORT_P6, GPIO_PIN4 }
// Remember to modify mcu_voltage's memory buffers if these pins are changed!
#define MC_VOLTAGES			&(struct IOMap) { GPIO_PORT_P6, GPIO_PIN0 + GPIO_PIN1 + \
															GPIO_PIN2 + GPIO_PIN3 }
/*#define CHG_LEFT_VOLTAGE	&(struct IOMap) { GPIO_PORT_P6, GPIO_PIN0 }
#define DCHG_LEFT_VOLTAGE	&(struct IOMap) { GPIO_PORT_P6, GPIO_PIN1 }
#define CHG_RIGHT_VOLTAGE	&(struct IOMap) { GPIO_PORT_P6, GPIO_PIN2 }
#define DCHG_RIGHT_VOLTAGE	&(struct IOMap) { GPIO_PORT_P6, GPIO_PIN3 } */
#define POWER_STATUS		&(struct IOMap) { GPIO_PORT_P7, GPIO_PIN7 }

// Debug LEDs - TODO: rename to more descriptive names
#define DEBUG_LED_0			&(struct IOMap) { GPIO_PORT_P8, GPIO_PIN0 }
#define DEBUG_LED_1			&(struct IOMap) { GPIO_PORT_P8, GPIO_PIN1 }
#define DEBUG_LED_2			&(struct IOMap) { GPIO_PORT_P8, GPIO_PIN2 }
#define DEBUG_LED_3			&(struct IOMap) { GPIO_PORT_P8, GPIO_PIN3 }
#define DEBUG_LED_4			&(struct IOMap) { GPIO_PORT_P8, GPIO_PIN4 }
#define DEBUG_LED_5			&(struct IOMap) { GPIO_PORT_P8, GPIO_PIN5 }
#define DEBUG_LED_7			&(struct IOMap) { GPIO_PORT_P8, GPIO_PIN7 }
#define DEBUG_LED_PORT8		&(struct IOMap) { GPIO_PORT_P8, GPIO_PIN_ALL }

#endif /* PINS_H_ */
