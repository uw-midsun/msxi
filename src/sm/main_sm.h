#pragma once

typedef enum {
  KILLSWITCH_ON
} SystemEvent;

void main_sm_init();

struct StateMachine *main_sm_get_info();
