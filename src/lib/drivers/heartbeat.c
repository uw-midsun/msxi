#include "heartbeat.h"
#include "sm/event_queue.h"
#include "timer_a.h"

// SMCLK: ~1.045 MHz, divided by 64: 16328Hz -> ~1 second
#define HEARTBEAT_PERIOD 16328

static struct IOMap pin = { 0 };

void heartbeat_begin(const struct IOMap *heartbeat_pin) {
  io_set_dir(heartbeat_pin, PIN_IN);
  pin = *heartbeat_pin;

  // Set up & Start Timer_A0 for Up Mode sourced by SMCLK/64
  Timer_A_initUpModeParam initUpParam = {
    .clockSource = TIMER_A_CLOCKSOURCE_SMCLK,
    .clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_64,
    .timerPeriod = HEARTBEAT_PERIOD,
    .timerInterruptEnable_TAIE = TIMER_A_TAIE_INTERRUPT_DISABLE,
    .timerClear = TIMER_A_DO_CLEAR,
    .startTimer = false
  };
  Timer_A_initUpMode(TIMER_A0_BASE, &initUpParam);

  // Set up & Start compare mode
  Timer_A_clearCaptureCompareInterruptFlag(TIMER_A0_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_0);
  Timer_A_initCompareModeParam initCompParam = {
    .compareRegister = TIMER_A_CAPTURECOMPARE_REGISTER_0,
    .compareInterruptEnable = TIMER_A_CAPTURECOMPARE_INTERRUPT_ENABLE,
    .compareOutputMode = TIMER_A_OUTPUTMODE_OUTBITVALUE,
    .compareValue = HEARTBEAT_PERIOD
  };
  Timer_A_initCompareMode(TIMER_A0_BASE, &initCompParam);

  Timer_A_startCounter(TIMER_A0_BASE, TIMER_A_UP_MODE);
}

#pragma vector = TIMER0_A0_VECTOR
__interrupt void TIMER0_A0_ISR(void) {
  static IOState state = IO_LOW;
  // Flags automatically cleared
  IOState new_state = io_get_state(&pin);
  if(new_state != state) {
    state = new_state;
    // Enum trickery - should probably change
    event_raise_isr(HEARTBEAT_EVENT_OFFSET + state);
  }
}
