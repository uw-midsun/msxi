#pragma once

// This is the startup state machine. It handles relay order and motor controller precharge.

#define STARTUP_EVENT_OFFSET 1000

typedef enum {
  INIT_COMPLETE = STARTUP_EVENT_OFFSET,
  SOLAR_ENABLED,
  STARTUP_COMPLETE,
  STARTUP_FAIL,
  SOLAR_FAIL
} StartupEvent;

void startup_sm_init();

struct StateMachine *startup_sm_get_info();
