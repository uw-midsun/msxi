#include "msp430.h"
#include "QmathLib.h"
#include "ring_buffer.h"
#include "pwm.h"

void main(void) {
  WDTCTL = WDTPW | WDTHOLD;
  uint8_t i = 0;
  //const struct PWMConfig pwm = {{4, 0}, {4, 1}};
  const struct PWMConfig pwm = {{1, 2}, {1, 3}};

  pwm_init(&pwm);

  volatile float dc;
  while (true) {
	  dc = pwm_calculate_duty_cycle();
	  for (i = 0; i < 200; i++){
		  __delay_cycles(6000);
	  }
  }
}
