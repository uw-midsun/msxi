/*
  heartbeat.c - Titus Chow
  
  Uses Timer_A0 to check for a change in the hearbeat pin's state every ~1s.
  It is assumed that when the pin is low, the heartbeat is bad.

*/
#include "heartbeat.h"
#include "sm/event_queue.h"
#include "timer_a.h"

// SMCLK: ~1.045 MHz, divided by 64: 16328Hz -> ~1 second
#define HEARTBEAT_PERIOD 16328

static struct IOMap pin = { 0 };
static IOState state = LOW;

void init_heartbeat(const struct IOMap *heartbeat_pin) {
	set_io_dir(heartbeat_pin, PIN_IN);
	pin = *heartbeat_pin;

	// Set up & Start Timer_A0 for Up Mode sourced by SMCLK/64
	Timer_A_initUpModeParam initUpParam = { 0 };
	initUpParam.clockSource = TIMER_A_CLOCKSOURCE_SMCLK;
	initUpParam.clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_64;
	initUpParam.timerPeriod = HEARTBEAT_PERIOD;
	initUpParam.timerInterruptEnable_TAIE = TIMER_A_TAIE_INTERRUPT_DISABLE;
	initUpParam.timerClear = TIMER_A_DO_CLEAR;
	initUpParam.startTimer = false;
	Timer_A_initUpMode(TIMER_A0_BASE, &initUpParam);

	// Set up & Start compare mode
	Timer_A_clearCaptureCompareInterruptFlag(TIMER_A0_BASE,
											TIMER_A_CAPTURECOMPARE_REGISTER_0);
	Timer_A_initCompareModeParam initCompParam = { 0 };
	initCompParam.compareRegister = TIMER_A_CAPTURECOMPARE_REGISTER_0;
	initCompParam.compareInterruptEnable = TIMER_A_CAPTURECOMPARE_INTERRUPT_ENABLE;
	initCompParam.compareOutputMode = TIMER_A_OUTPUTMODE_OUTBITVALUE;
	initCompParam.compareValue = HEARTBEAT_PERIOD;
	Timer_A_initCompareMode(TIMER_A0_BASE, &initCompParam);

	Timer_A_startCounter(TIMER_A0_BASE, TIMER_A_UP_MODE);
}

#pragma vector = TIMER0_A0_VECTOR
__interrupt void TIMER0_A0_ISR(void) {
	// Flags automatically cleared
	IOState new_state = get_io_state(&pin);
	if(new_state != state) {
		state = new_state;
		// Enum trickery - should probably change
		unsafe_raise_event(HEARTBEAT_EVENT_OFFSET + state);
	}
}
