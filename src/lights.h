#pragma once
#include "drivers/can.h"

void lights_init(const struct CANConfig *can);

void lights_process_message(void);
