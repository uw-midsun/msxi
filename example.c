#include "msp430.h"
//#include "QmathLib.h"
#include "ring_buffer.h"
#include "pwm.h"

void main(void) {
  WDTCTL = WDTPW | WDTHOLD;

  //const struct PWMConfig pwm = {{4, 0}, {4, 1}};
  const struct PWMConfig pwm = {{1, 2}, {1, 3}};

  pwm_init(&pwm);
  P1SEL = BIT2 | BIT3;

  __enable_interrupt();

  __bis_SR_register(GIE);

  double dc;
  int a;
  while (true) {
	  dc = pwm_calculate_duty_cycle();
	  a = dc + 1;
	  if (a > 10) {
		  pwm_init(&pwm);
	  }
  }
}
