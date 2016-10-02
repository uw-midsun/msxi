#include "adc12.h"
#include <msp430.h>
#include "adc12_f247_shim.h"

#define ADC_SCALE(x) (uint16_t)(((x)*(uint32_t)3300)/4095) //0 to 4095 -> 0 to 3300mV
#define LAST_INDEX (ADC12_MEM_MAX - 1)
#define ADC12_INTERRUPT (1 << (LAST_INDEX)) // ADC12IEx
#define ADC12_INTERRUPT_VECTOR (((LAST_INDEX) * 2) + 6) // ADC12IFGx

// Modified by ADC interrupts
static volatile uint16_t results[ADC12_MEM_MAX];
static volatile uint8_t *ADC12_MCTL[ADC12_MEM_MAX] = {
  &ADC12MCTL0, &ADC12MCTL1, &ADC12MCTL2, &ADC12MCTL3, &ADC12MCTL4
};
static volatile uint16_t *ADC12_MEM[ADC12_MEM_MAX] = {
  &ADC12MEM0, &ADC12MEM1, &ADC12MEM2, &ADC12MEM3, &ADC12MEM4
};

void adc12_init(const struct ADC12Config *adc) {
  // Enable pins as inputs for ADC
  uint16_t i;
  for (i = 0; i < adc->count; i++) {
    io_set_peripheral_dir(&adc->buffers[i].pin, PIN_IN);
  }

  // Initialize ADC12_A module:
  // -> Requires ~184? cycles with a 158kOhm input + 5 MHz clock (28.2.5.3)
  // Enable Multiple Sampling
  ADC12CTL0 = ADC12SHT0_8 | ADC12MSC | ADC12ON;

  const uint16_t sample_mode = (adc->continuous) ? ADC12CONSEQ_3 : ADC12CONSEQ_1;
  ADC12CTL1 = ADC12SHP | sample_mode;

  // Configure Memory Buffers
  for (i = 0; i < adc->count; i++) {
    const uint8_t end_of_sequence = (i == LAST_INDEX) << 7;
    *ADC12_MCTL[i] = end_of_sequence | adc->buffers[i].source;
  }

  // Enable memory buffer interrupt
  ADC12IFG &= ~ADC12_INTERRUPT;
  ADC12IE |= ADC12_INTERRUPT;

  ADC12CTL0 |= ADC12ENC;

  if (adc->continuous) {
    ADC12CTL0 |= ADC12SC;
  }
}

uint16_t adc12_sample(const struct ADC12Config *adc, const ADC12Index pin) {
  if (!adc->continuous) {
    ADC12CTL0 |= ADC12SC;

    // Enter LPM4 while waiting for interrupt
    __bis_SR_register(LPM4_bits);
  }

  // TODO: do we really need to toggle interrupts? Seems to be the correct size for atomic access
  uint16_t pin_voltage = ADC_SCALE(results[pin]);

  return pin_voltage;
}

uint16_t adc12_raw(const struct ADC12Config *adc, const ADC12Index pin) {
  if (!adc->continuous) {
      ADC12CTL0 |= ADC12SC;

      // Enter LPM4 while waiting for interrupt
      __bis_SR_register(LPM4_bits);
    }

    // TODO: do we really need to toggle interrupts? Seems to be the correct size for atomic access
    uint16_t pin_voltage = results[pin];

    return pin_voltage;
}

#pragma vector=ADC12_VECTOR
__interrupt void ADC12ISR(void) {
  uint16_t i;
  switch (__even_in_range(ADC12IV, 34)) {
    case ADC12_INTERRUPT_VECTOR:
      for (i = 0; i < ADC12_MEM_MAX; i++) {
        // Move results, IFG is cleared
        results[i] = *ADC12_MEM[i]; // ADC12_A_MEMORY_i
      }

      // Exit LPM4 after interrupt processes
      __bic_SR_register_on_exit(LPM4_bits);
      break;
    default: break;
  }
}
