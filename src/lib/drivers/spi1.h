#pragma once
#include "io_map.h"
#include <stdbool.h>

// Interrupt-driven SPI driver. Note that SPI is a synchronous protocol.

bool spi1_init(struct IOMap reset_pin);

void spi1_write(uint8_t byte_out);

uint8_t spi1_read(void);

uint8_t spi1_exchange(uint8_t byte_out);
