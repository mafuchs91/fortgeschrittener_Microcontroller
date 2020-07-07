/***************************************************************************//**
 * @file    spi.h
 * @author  Max Fuchs
 * @date    <date of creation>
 *
 * @brief   <brief description>
 *
 * Here goes a detailed description if required.
 ******************************************************************************/

#include "spi.h"

#include "isr.h"
#include <msp430g2553.h>




unsigned char receiveBuffer;
unsigned char readLength;
unsigned char writeLength;
unsigned char rxIndex;
unsigned char transmissionFinishedRead = 0;
unsigned char transmissionFinishedWrite = 0;
unsigned char readbufferDummy;
unsigned char *readData;
unsigned char *writeData;


unsigned char spi_write_busy(void);


void spi_init(void) {

    set_receive_isr(receive_spi_isr);
    // P1.3 as output low to route the HCT4066 signals from the spi
    P1DIR |= BIT3;
    P1SEL &= ~(BIT3);
    P1OUT &= ~BIT3;

    UCB0CTL1 |= UCSWRST;                                                            // set accroding to MSp User guide p.438
    P1SEL = BIT5 + BIT6 + BIT7;                                                    //
    P1SEL2 = BIT5 + BIT6 + BIT7;                                                   //
    UCB0CTL0 = UCCKPL+UCMSB+UCMST+UCSYNC;                                           // 3-pin, 8-bit, SPI master
    UCB0CTL1 = UCSSEL_2 + UCSWRST ;                                                          // SMCLK
    UCB0BR0 = 160;                                                                 // smclk/160 = ~ 160kHz
    UCB0BR1 = 0;                                                                   // smclk/160 = 100kHz.-
    UCB0CTL1 &= ~UCSWRST;                                                          // clear accroding to MSp User guide p.438

    IE2 |= UCB0RXIE;                                                               // enable receive interrupt
}

void spi_read(unsigned char length, unsigned char * rxData) {
    while(spi_busy());
    transmissionFinishedRead = 1;
    readLength = length;
    readData = rxData;

}

void spi_write(unsigned char length, unsigned char * txData) {

    while(spi_write_busy());                // makes sure two consecutive spi_write calls are possible
    transmissionFinishedWrite = 1;
    writeLength = length;
    writeData = txData;

    UCB0TXBUF = *txData;           // Send first value

}


unsigned char spi_busy(void) {
    if(transmissionFinishedWrite == 1 || transmissionFinishedRead == 1 ) {
        return 1;
    } else {
        return 0;
    }
}
unsigned char spi_write_busy(void) {
    if(transmissionFinishedWrite == 1 ) {
        return 1;
    } else {
        return 0;
    }
}

//
//#pragma vector=USCIAB0RX_VECTOR
//__interrupt void SPI_USCIA0RX_ISR(void)


unsigned char receive_spi_isr(void){


//
//
//
//    *writeData = UCB0RXBUF;       //read incoming byte
//    writeLength--;
//    writeData++;
//    if (writeLength > 0){
//        UCB0TXBUF = *writeData;                     // Transmit next byte
//    }
//    else{
//            transmissionFinishedWrite = 0;       //reset flag if transmission finished
//    }
//
//
//
//IFG2 &= ~( UCB0RXIFG) ;
//
//__delay_cycles(50);                     // Add time between transmissions to
//                                        // make sure slave can keep up
//return 0;                               // needed to leave sleep mode when using i2c library

  while(UCB0STAT & UCBUSY);                     //while (!(IFG2 & UCA0RXIFG)); // USCI_A0 TX buffer fertig?
  if(transmissionFinishedWrite == 1){            // reading mode
      *writeData++;
      writeLength --;
      if(writeLength == 0) {
          transmissionFinishedWrite = 0;
          if(transmissionFinishedRead == 1) {
              readbufferDummy = UCB0RXBUF;                    // read buffer
              UCB0TXBUF = 0;                    // write 0 value to tx buffer to receive next byte
          }
      } else {
          readbufferDummy = UCB0RXBUF;                    // read buffer

            UCB0TXBUF = *writeData;                    // write buffer

      }
  }else if(transmissionFinishedRead == 1){                                         // writing mode
      if(readLength == 0) {
          transmissionFinishedRead = 0;
      } else {
          *readData = UCB0RXBUF;                    // read buffer
          UCB0TXBUF = 0;                    // write 0 value to tx buffer to receive next byte
      }
      *readData++;
      readLength --;
  }

  // TODO: maby not necessry as automatically reset
  IFG2 &= ~( UCB0RXIFG) ;                       // reset interrupt flag
  __delay_cycles(50);
  return 0;                                     // necessray for i2c library

}
