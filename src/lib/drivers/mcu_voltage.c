#include "mcu_voltage.h"
#include "io_map.h"
#include "adc12_a.h"

#define ADC_SCALE(x) ((x)*3300)/4095 //0 to 4095 -> 0 to 3300mV

// Modified by ADC interrupts
static volatile uint16_t results[5];

void mcu_voltage_init(void) {
  // Enable pins as inputs for ADC
  io_set_peripheral_dir(IOMAP(POWER_STATUS), PIN_IN);
  io_set_peripheral_dir(IOMAP(MC_VOLTAGES), PIN_IN);

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


  // Configure Memory Buffers:
  // Map input A15 (PWR_STATUS) to buffer 0
  ADC12_A_memoryConfigure(ADC12_A_BASE,
                          ADC12_A_MEMORY_0,
                          ADC12_A_INPUT_A15,
                          ADC12_A_VREFPOS_AVCC,
                          ADC12_A_VREFNEG_AVSS,
                          ADC12_A_NOTENDOFSEQUENCE);

  // Map input A0 (MCU_CHG1) to buffer 1
  ADC12_A_memoryConfigure(ADC12_A_BASE,
                          ADC12_A_MEMORY_1,
                          ADC12_A_INPUT_A0,
                          ADC12_A_VREFPOS_AVCC,
                          ADC12_A_VREFNEG_AVSS,
                          ADC12_A_NOTENDOFSEQUENCE);

  // Map input A1 (MCU_DCHG1) to buffer 2
  ADC12_A_memoryConfigure(ADC12_A_BASE,
                          ADC12_A_MEMORY_2,
                          ADC12_A_INPUT_A1,
                          ADC12_A_VREFPOS_AVCC,
                          ADC12_A_VREFNEG_AVSS,
                          ADC12_A_NOTENDOFSEQUENCE);

  // Map input A2 (MCU_CHG2) to buffer 3
  ADC12_A_memoryConfigure(ADC12_A_BASE,
                          ADC12_A_MEMORY_3,
                          ADC12_A_INPUT_A2,
                          ADC12_A_VREFPOS_AVCC,
                          ADC12_A_VREFNEG_AVSS,
                          ADC12_A_NOTENDOFSEQUENCE);

  // Map input A3 (MCU_DCHG2) to buffer 4
  ADC12_A_memoryConfigure(ADC12_A_BASE,
                          ADC12_A_MEMORY_4,
                          ADC12_A_INPUT_A3,
                          ADC12_A_VREFPOS_AVCC,
                          ADC12_A_VREFNEG_AVSS,
                          ADC12_A_ENDOFSEQUENCE);

  // Enable memory buffer 4 interrupt
  ADC12_A_clearInterrupt(ADC12_A_BASE, ADC12IE4);
  ADC12_A_enableInterrupt(ADC12_A_BASE, ADC12IE4);
}

uint16_t mcu_voltage_sample(VoltageIndex pin) {
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
  switch (__even_in_range(ADC12IV, 34)) {
    case  0: break;         //Vector  0:  No interrupt
    case  2: break;         //Vector  2:  ADC overflow
    case  4: break;         //Vector  4:  ADC timing overflow
    case  6: break;         //Vector  6:  ADC12IFG0
    case  8: break;         //Vector  8:  ADC12IFG1
    case 10: break;         //Vector 10:  ADC12IFG2
    case 12: break;         //Vector 12:  ADC12IFG3
    case 14:                //Vector 14:  ADC12IFG4
      // Move A15 results, IFG is cleared
      results[0] = ADC12_A_getResults(ADC12_A_BASE, ADC12_A_MEMORY_0);
      // Move A0 results, IFG is cleared
      results[1] = ADC12_A_getResults(ADC12_A_BASE, ADC12_A_MEMORY_1);
      // Move A1 results, IFG is cleared
      results[2] = ADC12_A_getResults(ADC12_A_BASE, ADC12_A_MEMORY_2);
      // Move A2 results, IFG is cleared
      results[3] = ADC12_A_getResults(ADC12_A_BASE, ADC12_A_MEMORY_3);
      // Move A3 results, IFG is cleared
      results[4] = ADC12_A_getResults(ADC12_A_BASE, ADC12_A_MEMORY_4);

      // Exit LPM4 after interrupt processes
      __bic_SR_register_on_exit(LPM4_bits);
    case 16: break;         //Vector 16:  ADC12IFG5
    case 18: break;         //Vector 18:  ADC12IFG6
    case 20: break;         //Vector 20:  ADC12IFG7
    case 22: break;         //Vector 22:  ADC12IFG8
    case 24: break;         //Vector 24:  ADC12IFG9
    case 26: break;         //Vector 26:  ADC12IFG10
    case 28: break;         //Vector 28:  ADC12IFG11
    case 30: break;         //Vector 30:  ADC12IFG12
    case 32: break;         //Vector 32:  ADC12IFG13
    case 34: break;         //Vector 34:  ADC12IFG14
    default: break;
  }
}
