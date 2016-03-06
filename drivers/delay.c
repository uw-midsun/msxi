#include "delay.h"
#include "timer_a.h"

// ACLK: 32768Hz, divided by 64: 512 -> ~1 second
#define CYCLES_SECOND 512

void delay_seconds(uint8_t seconds) {
  // Set up & Start Timer_A0 for Up Mode sourced by ACLK/64
  Timer_A_initUpModeParam initUpParam = {
    .clockSource = TIMER_A_CLOCKSOURCE_ACLK,
    .clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_64,
    .timerInterruptEnable_TAIE = TIMER_A_TAIE_INTERRUPT_DISABLE,
    .timerClear = TIMER_A_DO_CLEAR,
    .startTimer = false
  };
  Timer_A_initUpMode(TIMER_A0_BASE, &initUpParam);

  // Set up & Start compare mode - CCR0 (timer counts up to CCR0)
  Timer_A_clearCaptureCompareInterruptFlag(TIMER_A0_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_0);
  Timer_A_initCompareModeParam initCompParam = {
    .compareRegister = TIMER_A_CAPTURECOMPARE_REGISTER_0,
    .compareInterruptEnable = TIMER_A_CAPTURECOMPARE_INTERRUPT_ENABLE,
    .compareOutputMode = TIMER_A_OUTPUTMODE_OUTBITVALUE,
    .compareValue = (CYCLES_SECOND * (uint16_t)seconds)
  };
  Timer_A_initCompareMode(TIMER_A0_BASE, &initCompParam);

  Timer_A_startCounter(TIMER_A0_BASE, TIMER_A_UP_MODE);

  // Sleep until the interrupt fires
  __bis_SR_register(LPM3_bits);
}

#pragma vector = TIMER0_A0_VECTOR
__interrupt void TIMER0_A0_ISR(void) {
  // Flags automatically cleared
  Timer_A_stop(TIMER_A0_BASE);

  // Exit LPM3
  __bic_SR_register_on_exit(LPM3_bits);
}
