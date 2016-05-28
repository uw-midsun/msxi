#include "msp430.h"
#include "QmathLib.h"
#include "current_sensor.h"

void main(void) {
  WDTCTL = WDTPW | WDTHOLD;
  uint8_t i = 0;

  current_sensor_init();

  volatile int16_t A;
  while (true) {
    A = current_sensor_read();
	for (i = 0; i < 200; i++){
	  __delay_cycles(6000);
	}
  }
}
