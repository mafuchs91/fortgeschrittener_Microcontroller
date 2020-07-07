/***************************************************************************//**
 * @file    i2c.c
 * @author  Max Fuchs
 *          Matr.: 4340529
 *          Email: maxfuchs@gmx.de
 * @date    22.6.2020
 *
 * @SheetNr 2
 * @brief   implements basic functionality for i2c communication. With low power mode
 *
 * Where stated variable namings and structural elements are partly taken from the
 * TI example library, msp430g2xx3_usci_i2c_standard_master.c   .
 ******************************************************************************/

#include "./i2c.h"

#include "isr.h"


/******************************************************************************
 * VARIABLES
 *****************************************************************************/


// 0: no stop byte is not set after an execution of the write function
// 1: stop byte is not set after an execution of the write function
unsigned char stopFlag = 0;

// 0 if written byte was acknoledged
// 1 if written byte was not acknoledged
unsigned char acknoledgedFlag = 0;


// Namings taken from TI Example (see description)
/* ReceiveBuffer: Buffer used to receive data in the ISR
 * RXByteCtr: Number of bytes left to receive
 * ReceiveIndex: The index of the next byte to be received in ReceiveBuffer
 * TransmitBuffer: Buffer used to transmit data in the ISR
 * TXByteCtr: Number of bytes left to transfer
 * TransmitIndex: The index of the next byte to be transmitted in TransmitBuffer
 * */

unsigned char ReceiveBuffer[MAX_BUFFER_SIZE] = {0};
unsigned char RXByteCtr = 0;
unsigned char ReceiveIndex = 0;
unsigned char TransmitBuffer[MAX_BUFFER_SIZE] = {0};
unsigned char TXByteCtr = 0;
unsigned char TransmitIndex = 0;



/******************************************************************************
 * LOCAL FUNCTION PROTOTYPES
 *****************************************************************************/
// Copys the source array to the destenation array, for the given number of bytes.
void CopyArray(unsigned char *source, unsigned char *dest, char count);



/******************************************************************************
 * LOCAL FUNCTION IMPLEMENTATION
 *****************************************************************************/
void CopyArray(unsigned char *source, unsigned char *dest, char count) {
    char copyIndex = 0;
    for (copyIndex = 0; copyIndex < count; copyIndex++)
    {
        dest[copyIndex] = source[copyIndex];
    }
}


/******************************************************************************
 * FUNCTION IMPLEMENTATION
 *****************************************************************************/

void i2c_init (unsigned char addr) {

    P1DIR |=  BIT1 + BIT2 + BIT3;

    P1OUT &= ~(BIT1 + BIT2 + BIT3);

    // make sure 74HCT4066 connects the i2c lines
    P1OUT |= BIT3;
    P1SEL |= BIT6 + BIT7;                     // Assign I2C pins to USCI_B0
    P1SEL2|= BIT6 + BIT7;                     // Assign I2C pins to USCI_B0

    // Namings and settings taken from TI Example (see description)
    UCB0CTL1 |= UCSWRST;                      // Enable SW reset
    UCB0CTL0 = UCMST + UCMODE_3 + UCSYNC;     // I2C Master, synchronous mode
    UCB0CTL1 = UCSSEL_2 + UCSWRST;            // Use SMCLK, keep SW reset
    UCB0BR0 = 160;                            // fSCL = SMCLK/160 = ~100kHz
    UCB0BR1 = 0;
    UCB0I2CSA = addr;                   // Slave Address
    UCB0CTL1 &= ~UCSWRST;                     // Clear SW reset, resume operation
    UCB0I2CIE |= UCNACKIE;
}

unsigned char i2c_write(unsigned char length, unsigned char * txData, unsigned char stop) {
	// Before writing, you should always check if the last STOP-condition has already been sent.
	while (UCB0CTL1 & UCTXSTP);
	// set stop flag
	stopFlag = stop;

	// Copy txData to TransmitBuffer
	CopyArray(txData, TransmitBuffer, length);

	// store length of data to be transmitted
    TXByteCtr = length;
    TransmitIndex = 0;

    /* Initialize slave address and interrupts */
    IFG2 &= ~(UCB0TXIFG + UCB0RXIFG);       // Clear any pending interrupts
    IE2 &= ~UCB0RXIE;                       // Disable RX interrupt
    IE2 |= UCB0TXIE;                        // Enable TX interrupt

    UCB0CTL1 |= UCTR + UCTXSTT;             // I2C TX, start condition



    __bis_SR_register(LPM4_bits + GIE);     // enter LPM4 with interrupts

	if(acknoledgedFlag == 1) {
	    return 0;
	} else {
	    return 1;
	}
}

void i2c_read(unsigned char length, unsigned char * rxData) {
	// Before writing, you should always check if the last STOP-condition has already been sent.
	while (UCB0CTL1 & UCTXSTP);
	RXByteCtr = length;
    ReceiveIndex = 0;

    IFG2 &= ~(UCB0TXIFG + UCB0RXIFG);       // Clear any pending interrupts

    IE2 |= UCB0RXIE;              // Enable RX interrupt
    IE2 &= ~UCB0TXIE;             // Disable TX interrupt
    UCB0CTL1 &= ~UCTR;            // Switch to receiver
    UCB0CTL1 |= UCTXSTT;          // Send repeated start

	if (length == 1) {
	    // wait for start to take place
        while((UCB0CTL1 & UCTXSTT));
        // imideately send stop bit
        UCB0CTL1 |= UCTXSTP;
	} else {
	    // enter power save mode only if stop bit wasnt send immideately before
         __bis_SR_register(LPM4_bits + GIE);              // Enter LPM4 w/ interrupts

	}


	CopyArray(ReceiveBuffer, rxData, length);
}

//#pragma vector = USCIAB0TX_VECTOR
//__interrupt void USCIAB0TX_ISR(void)
//{
//    // Namings taken from TI Example (see description)
//    if (IFG2 & UCB0RXIFG)                 // Receive Data Interrupt
//     {
//         // read buffer value
//         unsigned char rx_val = UCB0RXBUF;
//
//         if (RXByteCtr)                     // receive bytes if bytes are left
//         {
//             ReceiveBuffer[ReceiveIndex++] = rx_val;
//             RXByteCtr--;
//         }
//
//         if (RXByteCtr == 1)                // if only one byte is left send stop bit
//         {
//             UCB0CTL1 |= UCTXSTP;
//         }
//         else if (RXByteCtr == 0)           // no bytes left to receive
//         {
//             IE2 &= ~UCB0RXIE;              // disable RX interrupt
//             __bic_SR_register_on_exit(LPM4_bits);      // Exit LPM4
//         }
//
//     }
//     else if (IFG2 & UCB0TXIFG)            // Transmit Data Interrupt
//     {
//         if (TXByteCtr)                    // send bytes if bytes are left in TransmitBuffer
//         {
//             // write TransmitBuffer to buffer register
//             UCB0TXBUF = TransmitBuffer[TransmitIndex++];
//             TXByteCtr--;
//         }
//         else                               // no bytes are left in the TransmitBuffer
//         {
//             // send Stop condition only if previosly selected via stop flag
//             if (stopFlag != 0) {
//                 //Done with transmission
//                 UCB0CTL1 |= UCTXSTP;     // Send stop condition
//                 IE2 &= ~UCB0TXIE;                       // disable TX interrupt
//             } else {
//                 IFG2 &= ~UCB0TXIFG;
//             }
//             __bic_SR_register_on_exit(LPM4_bits);      // Exit LPM4
//         }
//    }
//}
//
//#pragma vector = USCIAB0RX_VECTOR
//__interrupt void USCIAB0RX_ISR(void)
//{
//    if (UCB0STAT & UCNACKIFG)               // handle NACK
//    {
//        UCB0STAT &= ~UCNACKIFG;             // Clear NACK Flags
//        UCB0CTL1 |= UCTXSTP;                // send stop bit
//        acknoledgedFlag = 1;                // set acknoledge
//        __bic_SR_register_on_exit(LPM4_bits);      // Exit LPM4
//
//    }
//
//}
