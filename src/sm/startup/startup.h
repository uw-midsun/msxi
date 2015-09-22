/*
  startup.h - Titus Chow

  This is the Startup state machine. It runs the car's startup procedure.

 */

#ifndef STARTUP_H_
#define STARTUP_H_

#define STARTUP_EVENT_OFFSET 1000

typedef enum {
  INIT_COMPLETE = STARTUP_EVENT_OFFSET,
  SOLAR_ENABLED,
  STARTUP_COMPLETE,
  STARTUP_FAIL,
  SOLAR_FAIL
} StartupEvent;

void init_startup_sm();

struct StateMachine *get_startup_sm();

#endif
