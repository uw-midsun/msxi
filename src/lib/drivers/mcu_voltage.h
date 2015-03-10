/*
 * mcu_voltage.h
 * Titus Chow
 *
 */

#ifndef MCU_VOLTAGE_H_
#define MCU_VOLTAGE_H_
#include <stdint.h>

typedef enum {
	PWR_STATUS,
	MCU_CHG1,
	MCU_DCHG1,
	MCU_CHG2,
	MCU_DCHG2
} VoltageIndex;

void mcu_voltage_init(void);
uint16_t get_voltage(VoltageIndex pin);

#endif /* MCU_VOLTAGE_H_ */
