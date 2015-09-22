/*
 * spi1.c
 * Titus Chow
 *
 * Functions for interfacing with SPI1.
 * Note that SPI is a synchronous protocol.
 *
 * P3.4 -> Data Out    (UCA0SIMO)
 * P3.5 <- Data In    (UCA0SOMI)
 * P3.0 -> Clock Out  (UCA0CLK)
 * PX.Y -> Slave Reset
 */
#include "spi1.h"
#include "driverlib.h"

#define SPI_CLK 500000; // Desired SPI frequency

volatile uint8_t data;

bool spi1_init(struct IOMap reset_pin) {
  // Set PX.Y for slave reset
  set_io_dir(&reset_pin, PIN_OUT);
  set_io_high(&reset_pin);

  // Set secondary fucntion P3.0, 4. 5
  GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P3,
                        GPIO_PIN0 + GPIO_PIN4 + GPIO_PIN5);

  // Prepare to initialize SPI1 with SMCLK, MSB first, 3-pin Master
  USCI_A_SPI_initMasterParam param = { 0 };
  param.selectClockSource = USCI_A_SPI_CLOCKSOURCE_SMCLK;
  param.clockSourceFrequency = UCS_getSMCLK();
  param.desiredSpiClock = SPI_CLK;
  param.msbFirst = USCI_A_SPI_MSB_FIRST;
  param.clockPhase = USCI_A_SPI_PHASE_DATA_CHANGED_ONFIRST_CAPTURED_ON_NEXT;
  param.clockPolarity = USCI_A_SPI_CLOCKPOLARITY_INACTIVITY_HIGH;

  // Attempt to initalize SPI
  if(!USCI_A_SPI_initMaster(USCI_A0_BASE, &param)) {
    // SPI failed to initialize
    return false;
  }

  // Enable the SPI module
  USCI_A_SPI_enable(USCI_A0_BASE);

  // Clear and then enable receive interrupt - don't want it to fire immediately
  // (When we get an RX interrupt, the transmit register is ready for another byte.)
  USCI_A_SPI_clearInterruptFlag(USCI_A0_BASE,
                  USCI_A_SPI_RECEIVE_INTERRUPT);
  USCI_A_SPI_enableInterrupt(USCI_A0_BASE,
                USCI_A_SPI_RECEIVE_INTERRUPT);

  // Toggle Slave - Active Low
  set_io_low(&reset_pin);

  __delay_cycles(50); // Wait for slave to initialize

  return true;
}

void spi1_write(uint8_t byte_out) {
  USCI_A_SPI_transmitData(USCI_A0_BASE, byte_out);

  // Enter LMP4 and wait for byte to be recieved -> TX cleared
  // TODO: Should it be a different LPM?
  __bis_SR_register(LPM4_bits);
}

uint8_t spi1_read(void) {
  return spi1_exchange(0xFF); // Send a dummy byte
}

uint8_t spi1_exchange(uint8_t byte_out) {
  spi1_write(byte_out);
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
