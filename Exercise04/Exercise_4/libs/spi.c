/***************************************************************************//**
 * @file    spi.h
 * @author  Max Fuchs
 *          Matr.: 4340529
 *          Email: maxfuchs@gmx.de
 * @date    8.7.2020
 * @SheetNr 4
 * @brief   basic function implementations for communication via SPI
 *
 * read functions needs to be called before the write function!
 *
 * Here goes a detailed description if required.
 ******************************************************************************/

#include "spi.h"

#include "isr.h"
#include <msp430g2553.h>


unsigned char readLength;                   // number of databytes to be read
unsigned char writeLength;                  // number of databytes to be written
unsigned char transmissionFinishedRead = 0; // read flag
unsigned char transmissionFinishedWrite = 0;// write flag
unsigned char readbufferDummy;              // dummy buffer for buffer values not needed but need to be read
unsigned char *readData;                    // pointer for data to be read
unsigned char *writeData;                   // pointer for data to be written

// indicates weather the write command is still busy
// 0: finished
// 1: still busy
unsigned char spi_write_busy(void);


void spi_init(void) {
    // route the UART interrupt to spi
    set_receive_isr(receive_spi_isr);
    // P1.3 as output low to route the HCT4066 signals from the spi
    P1DIR |= BIT3;
    P1SEL &= ~(BIT3);
    P1OUT &= ~BIT3;

    UCB0CTL1 |= UCSWRST;                                                            // set accroding to MSp User guide p.438
    P1SEL = BIT5 + BIT6 + BIT7;
    P1SEL2 = BIT5 + BIT6 + BIT7;
    UCB0CTL0 = UCCKPL +UCMSB +UCMST +UCSYNC;                                           // 3 pin, 8-bit,SPI master
    UCB0CTL1 = UCSSEL_2 + UCSWRST ;                                                // SMCLK
    UCB0BR0 = 160;                                                                 // smclk/160 = 100kHz
    UCB0BR1 = 0;                                                                   // smclk/160 = 100kHz
    UCB0CTL1 &= ~UCSWRST;                                                          // clear accroding to MSp User guide p.438

    IE2 |= UCB0RXIE;                                                               // enable receive interrupt
}

void spi_read(unsigned char length, unsigned char * rxData) {
    while(spi_busy());              // wait if read or write is still ongoing
    transmissionFinishedRead = 1;
    readLength = length;
    readData = rxData;              // set pointer for data to be read
}

void spi_write(unsigned char length, unsigned char * txData) {
    while(spi_write_busy());                // makes sure two consecutive spi_write calls are possible
    transmissionFinishedWrite = 1;
    writeLength = length;
    writeData = txData;

    UCB0TXBUF = *txData;           // Send first value
}

unsigned char spi_busy(void) {
    if(transmissionFinishedWrite == 1 || transmissionFinishedRead == 1 ) {      // read or write still active
        return 1;
    } else {                                                                    // none active
        return 0;
    }
}
unsigned char spi_write_busy(void) {
    if(transmissionFinishedWrite == 1 ) {                                       // write still active
        return 1;
    } else {
        return 0;
    }
}


unsigned char receive_spi_isr(void){
  while(UCB0STAT & UCBUSY);                     //while (!(IFG2 & UCA0RXIFG)); // USCI_A0 TX buffer fertig?
  if(transmissionFinishedWrite == 1){            // write mode
      *writeData++;                              // next adress
      writeLength --;
      if(writeLength == 0) {                    // last byte to write
          transmissionFinishedWrite = 0;
          if(transmissionFinishedRead == 1) {   // read bytes if spi_read() was called before
              readbufferDummy = UCB0RXBUF;      // read buffer as dummy
              UCB0TXBUF = 0;                    // write 0 value to tx buffer to receive next byte
          }
      } else {                                  // bytes left to write
          readbufferDummy = UCB0RXBUF;          // read as dummy
          UCB0TXBUF = *writeData;               // write buffer
      }
  }else if(transmissionFinishedRead == 1){      // reading mode
      if(readLength == 0) {                     // no bytes left to read
          transmissionFinishedRead = 0;
      } else {                                  // bytes left to read
          *readData = UCB0RXBUF;                // read buffer
          UCB0TXBUF = 0;                        // write 0 value to tx buffer to receive next byte
      }
      *readData++;
      readLength --;
  }

  IFG2 &= ~( UCB0RXIFG) ;                       // reset interrupt flag
  __delay_cycles(100);                           // give slave some time
  return 0;                                     // necessray for i2c library not spi specific

}
