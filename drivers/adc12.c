#include "adc12.h"

#define ADC_SCALE(x) (uint16_t)(((x)*(uint32_t)3300)/4096) //0 to 4095 -> 0 to 3300mV
#define LAST_INDEX (PIN_COUNT - 1)
#define ADC12_INTERRUPT (1 << (LAST_INDEX)) // ADC12IEx
#define ADC12_INTERRUPT_VECTOR (((LAST_INDEX) * 2) + 6) // ADC12IFGx

// Modified by ADC interrupts
static volatile uint16_t results[PIN_COUNT];

void adc12_init(const struct ADC12Config *adc) {
  // Enable pins as inputs for ADC
  uint16_t i;
  for (i = 0; i < PIN_COUNT; i++) {
    io_set_peripheral_dir(&adc->buffers[i].pin, PIN_IN);
  }

  // Initialize ADC12_A module
  // 5 MHz oscillator from UCS
  // 12-bit resolution (default)
  ADC12_A_init(ADC12_A_BASE,
               ADC12_A_SAMPLEHOLDSOURCE_SC,
               ADC12_A_CLOCKSOURCE_ADC12OSC,
               ADC12_A_CLOCKDIVIDER_1);

  ADC12_A_enable(ADC12_A_BASE);

  // -> Requires ~184? cycles with a 158kOhm input + 5 MHz clock (28.2.5.3)
  // Enable Multiple Sampling
  ADC12_A_setupSamplingTimer(ADC12_A_BASE,
                             ADC12_A_CYCLEHOLD_256_CYCLES,
                             ADC12_A_CYCLEHOLD_4_CYCLES,
                             ADC12_A_MULTIPLESAMPLESENABLE);


  // Configure Memory Buffers
  for (i = 0; i < PIN_COUNT; i++) {
    const uint8_t end_of_sequence = (i == LAST_INDEX) ?
                                     ADC12_A_ENDOFSEQUENCE : ADC12_A_NOTENDOFSEQUENCE;
    ADC12_A_memoryConfigure(ADC12_A_BASE,
                            i, // ADC12_A_MEMORY_i
                            adc->buffers[i].source,
                            ADC12_A_VREFPOS_AVCC,
                            ADC12_A_VREFNEG_AVSS,
                            end_of_sequence);
  }

  // Enable memory buffer interrupt
  ADC12_A_clearInterrupt(ADC12_A_BASE, ADC12_INTERRUPT);
  ADC12_A_enableInterrupt(ADC12_A_BASE, ADC12_INTERRUPT);
}

uint16_t adc12_sample(const struct ADC12Config *adc, const ADC12Index pin) {
  // Start sampling + conversion cycle
  ADC12_A_startConversion(ADC12_A_BASE,
                          ADC12_A_MEMORY_0,
                          ADC12_A_SEQOFCHANNELS);

  // Enter LPM4 while waiting for interrupt
  __bis_SR_register(LPM4_bits);

  // Prevent interrupt from suddenly changing data.
  // Is this necessary? Interrupt shouldn't be called again.
  //ADC12_A_disableInterrupt(ADC12_A_BASE, ADC12IE4);

  uint16_t pin_voltage = ADC_SCALE(results[pin]);

  // Reenable interrupts
  // Do we need to clear interrupts before reenabling?
  //ADC12_A_enableInterrupt(ADC12_A_BASE, ADC12IE4);

  return pin_voltage;
}

#pragma vector=ADC12_VECTOR
__interrupt void ADC12ISR(void) {
  uint16_t i;
  switch (__even_in_range(ADC12IV, 34)) {
    case ADC12_INTERRUPT_VECTOR:
      for (i = 0; i < PIN_COUNT; i++) {
        // Move results, IFG is cleared
        results[i] = ADC12_A_getResults(ADC12_A_BASE, i); // ADC12_A_MEMORY_i
      }

      // Exit LPM4 after interrupt processes
      __bic_SR_register_on_exit(LPM4_bits);
      break;
    default: break;
  }
}
