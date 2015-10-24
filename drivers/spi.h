#pragma once
#include "io_map.h"
#include <stdbool.h>

// Interrupt-driven SPI driver for USCI_A/Bx. Note that SPI is a synchronous protocol.
// SPI functions should not be used in ISRs.

#define SPI_EXCHANGE_ERROR 0x01;
#define SMCLK_FREQ 1048576

typedef enum {
  SPI_A0 = 0,
  SPI_A1,
  SPI_B0,
#ifdef __MSP430_HAS_USCI_B3__
  SPI_B3
#endif
  SPI_NUM_PORTS
} SPIPort;

struct SPIConfig {
  struct IOMap data_out;  // SIMO
  struct IOMap data_in;   // SOMI
  struct IOMap clock_out; // SCLK
  struct IOMap cs;
  uint32_t clock_freq;
  SPIPort port;
};

// Set up with SMCLK, MSB first, 3-pin master
bool spi_init(const struct SPIConfig *spi);

void spi_transmit(const struct SPIConfig *spi, uint8_t data);

void spi_transmit_array(const struct SPIConfig *spi, const uint8_t *data, uint8_t length);

uint8_t spi_receive(const struct SPIConfig *spi);

uint8_t spi_exchange(const struct SPIConfig *spi, uint8_t data);

uint8_t spi_status(const struct SPIConfig *spi);

void spi_select(const struct SPIConfig *spi);

void spi_deselect(const struct SPIConfig *spi);
