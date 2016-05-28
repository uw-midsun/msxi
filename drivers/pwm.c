#include "msp430.h"
#include "QmathLib.h"
#include "ring_buffer.h"
#include "pwm.h"

volatile struct RingBuffer falling = {0};
volatile struct RingBuffer rising = {0};

void pwm_init(const struct PWMConfig *pwm)
{
  ring_buffer_init(&falling);
  ring_buffer_init(&rising);

  io_set_dir(&pwm->pin_rising, PIN_IN);
  io_set_dir(&pwm->pin_falling, PIN_IN);

  // Second line of the next few blocks are for launchpad

  // Timer B capture to CCR0 on rising max priority
  //TB0CCTL0 = CCIE | CAP | CM_1 | SCS | CCIS_0;
  TA0CCTL1 = CCIE | CAP | CM_1 | SCS | CCIS_0;

  // Timer B capture to CCR1 on falling 2nd highest priority
  //TB0CCTL1 = CCIE | CAP | CM_2 | SCS | CCIS_0;
  TA0CCTL2 = CCIE | CAP | CM_2 | SCS | CCIS_0;

  // Start the clock use 8 divisions to improve reliability
  //TB0CTL = TBSSEL_2 | MC_2 | TBCLR | ID_3;
  TA0CTL = TASSEL_2 | MC_2 | TACLR | ID_3;

  P1SEL = BIT2 | BIT3;
  __enable_interrupt();

  __bis_SR_register(GIE);
}

uint16_t pwm_calculate_duty_cycle(void)
{
  uint8_t i;
  uint8_t clock_reset = 0;
  uint16_t rising_values[BUFFER_SIZE];
  uint16_t falling_values[BUFFER_SIZE];
  uint32_t wavelengths = 0;
  uint32_t high_amplitude = 0;

  //TODO: Replace with Qmath
  //double duty_cycle = 0;
  volatile uint32_t duty_cycle = 0;

  // Read using memcpy to maximize speed
  //__disable_interrupt();
  memcpy(rising_values, &rising.buffer, 16 * sizeof(uint16_t));
  memcpy(falling_values, &falling.buffer, 16 * sizeof(uint16_t));
  //ring_buffer_read(&rising, rising_values);
  //ring_buffer_read(&falling, falling_values);
  //__enable_interrupt();

  // calculate the wavelength and amplitude and divide to get the duty cycle
  for (i = 1; i < BUFFER_SIZE - 1; i++) {
    if (rising_values[i + 1] > rising_values[i] &&
      falling_values[i + 1] > falling_values[i] ) {
      //wavelengths = rising_values[i + 1] - rising_values[i];
      wavelengths += (uint32_t) rising_values[i + 1] - (uint32_t) rising_values[i];
      //high_amplitude = falling_values[i] - rising_values[i];
      high_amplitude += (uint32_t) falling_values[i] - (uint32_t) rising_values[i];
      //duty_cycle += (double) high_amplitude / wavelengths;
    } else {
      clock_reset += 1;
    }
  }
  //duty_cycle = duty_cycle / (BUFFER_SIZE - 2 - clock_reset) * 100;
  //high_amplitude = high_amplitude * 100;
  duty_cycle = (high_amplitude * 100) / wavelengths;
  return duty_cycle;
}

#pragma vector=TIMER0_A1_VECTOR
__interrupt void TIMER0_A1_ISR(void) {
  if (TA0IV == 0x02) {
    ring_buffer_push(&rising, TA0CCR1);
  } else {
    ring_buffer_push(&falling, TA0CCR2);
  }
}

// ISR for Timer B on Plutus

/*
#pragma vector=TIMER0_B0_VECTOR
__interrupt void TB0CCR0_ISR(void) {
  ring_buffer_push(&rising, TB0CCR0);
}

#pragma vector=TIMER0_B1_VECTOR
__interrupt void TB0CCR1_ISR(void) {
  ring_buffer_push(&falling, TB0CCR1);
}
*/
