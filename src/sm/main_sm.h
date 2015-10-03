/*
  main_sm.h - Titus Chow

  This is the main state machine.

 */

#pragma once

typedef enum {
  KILLSWITCH_ON
} SystemEvent;

void init_main_sm();

struct StateMachine *get_main_sm();
