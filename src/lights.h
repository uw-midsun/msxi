#pragma once
#include <stdbool.h>
#include "drivers/can.h"
#define MAX_LIGHTS 10
#define MAX_SIGNALS 16

enum LightType {
  LIGHT_RUNNING,
  LIGHT_BLINK,
  LIGHT_BRAKE,
};

struct LightSignal {
	uint16_t can_msg_id;
	enum LightType type;
	struct IOMap signal_pins[MAX_LIGHTS];
};

struct LightConfig {
  struct SPIConfig spi;
  struct CANConfig can;
  uint16_t signal_states;
  struct LightSignal signals[MAX_SIGNALS];
  bool active;
};

void lights_init(struct LightConfig *light_config);

void lights_process_message(void);
