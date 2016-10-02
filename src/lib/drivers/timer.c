#include "timer.h"
#include "msp430.h"
#include <stddef.h>

#define ACLK_FREQ 32768
#define CYCLES_IN_MS ((ACLK_FREQ) / 1000)
#define NUM_TIMERS 4

typedef enum {
  TIMER_INACTIVE = 0,
  TIMER_PERIODIC,
  TIMER_ONE_SHOT,
} TimerType;

struct Timer {
  uint16_t target_ms;
  uint16_t elapsed_ms;
  timer_delay_cb delay_cb;
  void *context;
  TimerType type;
};

static struct Timer timers[NUM_TIMERS];

void timer_init() {
  TA1CCTL0 = CCIE;
  TA1CCR0 = CYCLES_IN_MS;
  TA1CTL = TASSEL_1 | MC_1 | TACLR; // ACLK, Up mode
}

void timer_process() {
  uint16_t i;
  for (i = 0; i < NUM_TIMERS; i++) {
    struct Timer *timer = &timers[i];
    if (timer->elapsed_ms >= timer->target_ms) {
      switch (timer->type) {
        case TIMER_ONE_SHOT:
          timer->type = TIMER_INACTIVE;
        case TIMER_PERIODIC:
          if (timer->delay_cb != NULL) {
            timer->delay_cb(timer->elapsed_ms, timer->context);
          }
          timer->elapsed_ms = 0;
          break;
        default:
          // Inactive - don't do anything
          break;
      }
    }
  }
}

static bool prv_register_timer(TimerType type, uint16_t ms,
                               timer_delay_cb delay_cb, void *context) {
  uint16_t i;
  for (i = 0; i < NUM_TIMERS; i++) {
    if (timers[i].type == TIMER_INACTIVE) {
      timers[i] = (struct Timer) {
        .target_ms = ms,
        .elapsed_ms = 0,
        .delay_cb = delay_cb,
        .context = context,
        .type = type
      };

      return true;
    }
  }

  return false;

}

bool timer_delay(uint16_t ms, timer_delay_cb delay_cb, void *context) {
  return prv_register_timer(TIMER_ONE_SHOT, ms, delay_cb, context);
}

bool timer_delay_periodic(uint16_t ms, timer_delay_cb delay_cb, void *context) {
  return prv_register_timer(TIMER_PERIODIC, ms, delay_cb, context);
}

#pragma vector=TIMER1_A0_VECTOR
__interrupt void TIMERA1_ISR(void) {
  uint16_t i;
  for (i = 0; i < NUM_TIMERS; i++) {
    // Doesn't actually matter if the timer is inactive
    timers[i].elapsed_ms++;
  }
}
