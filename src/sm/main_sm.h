/*
  main_sm.h - Titus Chow

  This is the main state machine.

 */

#ifndef MAIN_SM_H_
#define MAIN_SM_H_

typedef enum {
	KILLSWITCH_ON
} SystemEvent;

void init_main_sm();

struct StateMachine *get_main_sm();

#endif
