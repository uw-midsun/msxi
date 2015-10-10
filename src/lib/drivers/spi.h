#pragma once
#include "io_map.h"
#include <stdbool.h>

// Interrupt-driven SPI driver for USCI_Ax. Note that SPI is a synchronous protocol.

typedef struct SPIConfig {
  struct IOMap data_out;  // SIMO
  struct IOMap data_in;   // SOMI
  struct IOMap clock_out; // SCLK
  struct IOMap cs;
  uint32_t clock_freq;
  uint16_t base_addr;
} SPIConfig;

bool spi_init(const struct SPIConfig *spi);

void spi_write(const struct SPIConfig *spi, uint8_t byte_out);

uint8_t spi_read(const struct SPIConfig *spi);

uint8_t spi_exchange(const struct SPIConfig *spi, uint8_t byte_out);
