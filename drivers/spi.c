#include "spi.h"

struct SPIResult {
  uint8_t data;
  uint8_t flags;
};

struct SPIModule {
  volatile uint8_t *ctl0;
  volatile uint8_t *ctl1;
  volatile uint8_t *br0;
  volatile uint8_t *br1;
  struct {
    volatile uint8_t *addr;
    uint8_t value;
  } ifg;
  struct {
    volatile uint8_t *addr;
    uint8_t value;
  } ie;
  volatile uint8_t *txbuf;
};

// Since the F247 doesn't support driverlib, this is a static array of all the SPI registers
// we use across our boards. This emulates what driverlib does.
#if defined(__MSP430F247__)
static const struct SPIModule SPI_MODULE[SPI_NUM_PORTS] = {
  { &UCA0CTL0, &UCA0CTL1, &UCA0BR0, &UCA0BR1, {&IFG2, UCA0RXIFG}, {&IE2, UCA0RXIE}, &UCA0TXBUF },
  { &UCA1CTL0, &UCA1CTL1, &UCA1BR0, &UCA1BR1, {&UC1IFG, UCA1RXIFG}, {&UC1IE, UCA1RXIE}, &UCA1TXBUF },
  { &UCB0CTL0, &UCB0CTL1, &UCB0BR0, &UCB0BR1, {&IFG2, UCB0RXIFG}, {&IE2, UCB0RXIE}, &UCB0TXBUF },
  { &UCB1CTL0, &UCB1CTL1, &UCB1BR0, &UCB1BR1, {&UC1IFG, UCB1RXIFG}, {&UC1IE, UCB1RXIE}, &UCB1TXBUF }
};
#elif defined(__MSP430F5529__) || defined(__MSP430F5438A__)
static const struct SPIModule SPI_MODULE[SPI_NUM_PORTS] = {
  { &UCA0CTL0, &UCA0CTL1, &UCA0BR0, &UCA0BR1, {&UCA0IFG, UCRXIFG}, {&UCA0IE, UCRXIE}, &UCA0TXBUF },
  { &UCA1CTL0, &UCA1CTL1, &UCA1BR0, &UCA1BR1, {&UCA1IFG, UCRXIFG}, {&UCA1IE, UCRXIE}, &UCA1TXBUF },
  { &UCB0CTL0, &UCB0CTL1, &UCB0BR0, &UCB0BR1, {&UCB0IFG, UCRXIFG}, {&UCB0IE, UCRXIE}, &UCB0TXBUF },
  { &UCB1CTL0, &UCB1CTL1, &UCB1BR0, &UCB1BR1, {&UCB1IFG, UCRXIFG}, {&UCB1IE, UCRXIE}, &UCB1TXBUF },
#if defined(__MSP430_HAS_USCI_B3__)
  { &UCB3CTL0, &UCB3CTL1, &UCB3BR0, &UCB3BR1, {&UCB3IFG, UCRXIFG}, {&UCB3IE, UCRXIE}, &UCB3TXBUF }
#endif
};
#else
#error Device not supported!
#endif

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
  *SPI_MODULE[spi->port].ifg.addr &= ~SPI_MODULE[spi->port].ifg.value;
  *SPI_MODULE[spi->port].ie.addr |= SPI_MODULE[spi->port].ie.value;

  __delay_cycles(50); // Wait for slave to initialize

  return true;
}

void spi_transmit(const struct SPIConfig *spi, uint8_t data) {
  // Populate with error condition. The flag will be cleared on a successful recieve.
  results[spi->port].flags |= SPI_EXCHANGE_ERROR;
  results[spi->port].data = 0;

  *SPI_MODULE[spi->port].txbuf = data;

  // Enter LPM3 and wait for byte to be recieved -> TX cleared
  __bis_SR_register(LPM3_bits);
}

void spi_transmit_array(const struct SPIConfig *spi, const uint8_t *data, uint8_t length) {
  uint16_t i;
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
}

static prv_update_data(const uint8_t port, const uint8_t data) {
  results[port].flags &= ~SPI_EXCHANGE_ERROR;
  results[port].data = data;
}

// Add a new interrupt vector for each SPI port you want to handle.
#define STRINGIFY(str) #str
#if defined(__MSP430F247__)
#define SPI_INTERRUPT(x) \
_Pragma(STRINGIFY(vector=USCIAB##x##RX_VECTOR)) \
__interrupt void USCIAB##x##RX_ISR(void) { \
  prv_update_data(SPI_A##x, UCA##x##RXBUF); \
  prv_update_data(SPI_B##x, UCB##x##RXBUF); \
  __bic_SR_register_on_exit(LPM3_bits); \
}

SPI_INTERRUPT(0)
SPI_INTERRUPT(1)

#elif defined(__MSP430F5529__) || defined(__MSP430F5438A__)
#define SPI_INTERRUPT(port) \
_Pragma(STRINGIFY(vector=USCI_##port##_VECTOR)) \
__interrupt void USCI_##port##_ISR(void) { \
  prv_update_data(SPI_##port, UC##port##RXBUF); \
  _bic_SR_register_on_exit(LPM3_bits); \
}

// macro magic whoo - should probably replace this eventually
SPI_INTERRUPT(A0)
SPI_INTERRUPT(A1)
SPI_INTERRUPT(B0)
SPI_INTERRUPT(B1)
#if defined(__MSP430_HAS_USCI_B3__)
SPI_INTERRUPT(B3)
#endif
#endif
