/*
  main_sm.h - Titus Chow

  This is the Startup state machine. It runs the car's startup procedure.

 */

#ifndef MAIN_SM_H_
#define MAIN_SM_H_

typedef enum {
	INIT_COMPLETE,
	STARTUP_COMPLETE
} StartupEvent;

void init_main_sm();

struct StateMachine *get_main_sm();

#endif
