#include "spi.h"
#include "msp430.h"

struct SPIResult {
  uint8_t data;
  uint8_t flags;
};

struct SPIModule {
  uint8_t *ctl0;
  uint8_t *ctl1;
  uint8_t *br0;
  uint8_t *br1;
  uint8_t ifg;
  uint8_t ie;
};

// Since the F247 doesn't support driverlib, this is a static array of all the SPI registers
// we use across our boards. This emulates what driverlib does.
static volatile struct SPIModule SPI_MODULE[SPI_NUM_PORTS] = {
  { &UCA0CTL0, &UCA0CTL1, &UCA0BR0, &UCA0BR1, UCA0RXIFG, UCA0RXIE },
  { &UCA1CTL0, &UCA1CTL1, &UCA1BR0, &UCA1BR1, UCA1RXIFG, UCA1RXIE },
  { &UCB0CTL0, &UCB0CTL1, &UCB0BR0, &UCB0BR1, UCB0RXIFG, UCB0RXIE },
#ifdef __MSP430_HAS_USCI_B3__
  { &UCB3CTL0, &UCB3CTL1, &UCB3BR0, &UCB3BR1, UCB3RXIFG, UCB3RXIE }
#endif
};

volatile struct SPIResult results[SPI_NUM_PORTS] = {};

bool spi_init(const struct SPIConfig *spi) {
  // Set CS pin high
  io_set_state(&spi->cs, IO_HIGH);
  io_set_dir(&spi->cs, PIN_OUT);

  // Set secondary function for SIMO, SOMI, and SCLK pins - direction doesn't matter
  io_set_peripheral_dir(&spi->data_out, PIN_IN);
  io_set_peripheral_dir(&spi->data_in, PIN_IN);
  io_set_peripheral_dir(&spi->clock_out, PIN_IN);

  // Stop the module for configuration
  // Active-low, MSB-first, Master, Synchronous
  *SPI_MODULE[spi->port].ctl1 |= UCSWRST;
  *SPI_MODULE[spi->port].ctl0 = UCCKPL | UCMSB | UCMST | UCSYNC;
  *SPI_MODULE[spi->port].ctl1 |= UCSSEL_2; // SMCLK

  // Baud Rate Prescaler = /(br0 + br1 * 256)
  const uint16_t prescaler = (SMCLK_FREQ / spi->clock_freq);
  *SPI_MODULE[spi->port].br0 = prescaler % 256;
  *SPI_MODULE[spi->port].br1 = (prescaler / 256);

  // Reenable the module!
  *SPI_MODULE[spi->port].ctl1 &= ~UCSWRST;

  // Clear and enable the recieve interrupt
  IFG2 &= ~SPI_MODULE[spi->port].ifg;
  IE2 |= SPI_MODULE[spi->port].ie;

  __delay_cycles(50); // Wait for slave to initialize

  return true;
}

void spi_transmit(const struct SPIConfig *spi, uint8_t data) {
  // Populate with error condition. The flag will be cleared on a successful recieve.
  results[spi->port].flags |= SPI_EXCHANGE_ERROR;
  results[spi->port].data = 0;

  UCB0TXBUF = data;

  // Enter LPM3 and wait for byte to be recieved -> TX cleared
  __bis_SR_register(LPM3_bits);
}

void spi_transmit_array(const struct SPIConfig *spi, const uint8_t *data, uint8_t length) {
  int i;
  for (i = 0; i < length; i++) {
    spi_transmit(spi, data[i]);
  }
}

uint8_t spi_receive(const struct SPIConfig *spi) {
  return spi_exchange(spi, 0xFF); // Send a dummy byte
}

uint8_t spi_exchange(const struct SPIConfig *spi, uint8_t data) {
  spi_transmit(spi, data);
  return results[spi->port].data;
}

uint8_t spi_status(const struct SPIConfig *spi) {
  return results[spi->port].flags;
}

void spi_select(const struct SPIConfig *spi) {
  io_set_state(&spi->cs, IO_LOW);
}

void spi_deselect(const struct SPIConfig *spi) {
  io_set_state(&spi->cs, IO_HIGH);

  // Give some time between SPI commands?
  // _delay_cycles(50);
}

static prv_update_data(const uint8_t port, const uint8_t data) {
  results[port].flags &= ~SPI_EXCHANGE_ERROR;
  results[port].data = data;
}

// Add a new interrupt vector for each SPI port you want to handle.
#pragma vector = USCIAB0RX_VECTOR
__interrupt void USCIAB0RX_ISR(void) {
  prv_update_data(SPI_B0, UCB0RXBUF);

  // Exit LPM3 after interrupt processes
  __bic_SR_register_on_exit(LPM3_bits);
}
