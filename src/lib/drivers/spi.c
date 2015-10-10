#include <drivers/spi.h>
#include "driverlib.h"

volatile uint8_t data;

bool spi_init(const struct SPIConfig *spi) {
  // Set CS pin high
  io_set_state(&spi->cs, IO_HIGH);
  io_set_dir(&spi->cs, PIN_OUT);

  // Set secondary function for SIMO, SOMI, and SCLK pins - direction doesn't matter
  io_set_peripheral_dir(&spi->data_out, PIN_IN);
  io_set_peripheral_dir(&spi->data_in, PIN_IN);
  io_set_peripheral_dir(&spi->clock_out, PIN_IN);

  // Prepare to initialize SPI1 with SMCLK, MSB first, 3-pin Master
  USCI_A_SPI_initMasterParam param = {
    .selectClockSource = USCI_A_SPI_CLOCKSOURCE_SMCLK,
    .clockSourceFrequency = UCS_getSMCLK(),
    .desiredSpiClock = spi->clock_freq,
    .msbFirst = USCI_A_SPI_MSB_FIRST,
    .clockPhase = USCI_A_SPI_PHASE_DATA_CHANGED_ONFIRST_CAPTURED_ON_NEXT,
    .clockPolarity = USCI_A_SPI_CLOCKPOLARITY_INACTIVITY_HIGH
  };

  // Attempt to initalize SPI
  if(!USCI_A_SPI_initMaster(spi->base_addr, &param)) {
    // SPI failed to initialize
    return false;
  }

  // Enable the SPI module
  USCI_A_SPI_enable(spi->base_addr);

  // Clear and then enable receive interrupt - don't want it to fire immediately
  // (When we get an RX interrupt, the transmit register is ready for another byte.)
  USCI_A_SPI_clearInterruptFlag(spi->base_addr, USCI_A_SPI_RECEIVE_INTERRUPT);
  USCI_A_SPI_enableInterrupt(spi->base_addr, USCI_A_SPI_RECEIVE_INTERRUPT);

  // Select slave - Active low
  io_set_state(&spi->cs, IO_LOW);

  __delay_cycles(50); // Wait for slave to initialize

  return true;
}

void spi_write(const struct SPIConfig *spi, uint8_t byte_out) {
  USCI_A_SPI_transmitData(spi->base_addr, byte_out);

  // Enter LMP4 and wait for byte to be recieved -> TX cleared
  // TODO: Should it be a different LPM?
  __bis_SR_register(LPM4_bits);
}

uint8_t spi_read(const struct SPIConfig *spi) {
  return spi_exchange(spi, 0xFF); // Send a dummy byte
}

uint8_t spi_exchange(const struct SPIConfig *spi, uint8_t byte_out) {
  spi_write(spi, byte_out);
  return data;
}

#pragma vector = USCI_A0_VECTOR
__interrupt void USCI_A0_ISR(void) {
  // Flags automatically cleared
  switch(_even_in_range(UCA0IV, 4)) {
    case 2: //RX
      // Set data to received data
      data = USCI_A_SPI_receiveData(USCI_A0_BASE);

      // Exit LPM4 after interrupt processes
      __bic_SR_register_on_exit(LPM4_bits);
      break;
    case 4: //TX
    default:
      break;
  }
}
