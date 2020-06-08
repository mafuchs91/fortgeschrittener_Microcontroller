/***************************************************************************//**
 * @file    i2c.c
 * @author  <your name>
 * @date    <date of creation>
 *
 * @brief   <brief description>
 *
 * Here goes a detailed description if required.
 ******************************************************************************/

#include "./i2c.h"
#include <msp430.h>
#include <stdint.h>
#include <stdbool.h>


#define MAX_BUFFER_SIZE     20
/******************************************************************************
 * VARIABLES
 *****************************************************************************/

// A variable to be set by your interrupt service routine:
// 1 if all bytes have been sent, 0 if transmission is still ongoing.
unsigned char transferFinished = 0;

// Flag to signal if only one byte is to be read
// if 0, more than one Byte is read, if 1, only one byte is read
unsigned char readStopFlag = 0;

unsigned char stopFlag = 0;

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


/* The Register Address/Command to use*/
char TransmitRegAddr = 0;

typedef enum I2C_ModeEnum{
    IDLE_MODE,
    NACK_MODE,
    TX_REG_ADDRESS_MODE,
    RX_REG_ADDRESS_MODE,
    TX_DATA_MODE,
    RX_DATA_MODE,
    SWITCH_TO_RX_MODE,
    SWITHC_TO_TX_MODE,
    TIMEOUT_MODE
} I2C_Mode;


/* Used to track the state of the software state machine*/
I2C_Mode MasterMode = IDLE_MODE;


/******************************************************************************
 * LOCAL FUNCTION PROTOTYPES
 *****************************************************************************/



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

// TODO: Implement these functions:

void i2c_init (unsigned char addr) {

    P1DIR |= BIT0 + BIT1 + BIT2 + BIT3 + BIT4;
    P1OUT &= ~(BIT0 + BIT1 + BIT2 + BIT3  + BIT4);

    P1OUT |= BIT3;
    P1SEL |= BIT6 + BIT7;                     // Assign I2C pins to USCI_B0
    P1SEL2|= BIT6 + BIT7;                     // Assign I2C pins to USCI_B0

    UCB0CTL1 |= UCSWRST;                      // Enable SW reset
    UCB0CTL0 = UCMST + UCMODE_3 + UCSYNC;     // I2C Master, synchronous mode
    UCB0CTL1 = UCSSEL_2 + UCSWRST;            // Use SMCLK, keep SW reset
    UCB0BR0 = 160;                            // fSCL = SMCLK/160 = ~100kHz
    UCB0BR1 = 0;
    UCB0I2CSA = 72;                   // Slave Address
    UCB0CTL1 &= ~UCSWRST;                     // Clear SW reset, resume operation
    UCB0I2CIE |= UCNACKIE;
}

unsigned char i2c_write(unsigned char length, unsigned char * txData, unsigned char stop) {
	// Before writing, you should always check if the last STOP-condition has already been sent.
	while (UCB0CTL1 & UCTXSTP);
	transferFinished = 0;
	// set stop flag
	stopFlag = stop;

    /* Initialize state machine */
    MasterMode = TX_DATA_MODE;
//	TransmitRegAddr = reg_addr;
	// Copy txData to TransmitBuffer
	CopyArray(txData, TransmitBuffer, length);




    TXByteCtr = length;
    RXByteCtr = 0;
    ReceiveIndex = 0;
    TransmitIndex = 0;

    /* Initialize slave address and interrupts */
//    UCB0I2CSA = dev_addr;
    IFG2 &= ~(UCB0TXIFG + UCB0RXIFG);       // Clear any pending interrupts
    IE2 &= ~UCB0RXIE;                       // Disable RX interrupt
    IE2 |= UCB0TXIE;                        // Enable TX interrupt

    UCB0CTL1 |= UCTR + UCTXSTT;             // I2C TX, start condition


//    __bis_SR_register(CPUOFF + GIE);              // Enter LPM0 w/ interrupts
	// Wait for transfer to be finished.
	// Info: In TI's sample code, low-power mode statements are inserted,
	// also waiting for the transfer to be finished.

	while(!transferFinished);

	return 1;
}

void i2c_read(unsigned char length, unsigned char * rxData) {
	// Before writing, you should always check if the last STOP-condition has already been sent.
	while (UCB0CTL1 & UCTXSTP);
	transferFinished = 0;
	// TODO: Check if all necessary
	RXByteCtr = length;
    TXByteCtr = 0;
    ReceiveIndex = 0;
    TransmitIndex = 0;

    IFG2 &= ~(UCB0TXIFG + UCB0RXIFG);       // Clear any pending interrupts

    IE2 |= UCB0RXIE;              // Enable RX interrupt
    IE2 &= ~UCB0TXIE;             // Disable TX interrupt
    UCB0CTL1 &= ~UCTR;            // Switch to receiver
    UCB0CTL1 |= UCTXSTT;          // Send repeated start


	if (length == 1) {
	    readStopFlag = 1;
		// Todo: If you only want to receive one byte, you instantly have to write a STOP-condition
		// after the START-condition got sent.
	}

	// Wait for transfer to be finished.
	// Info: In TI's sample code, low-power mode statements are inserted,
	// also waiting for the transfer to be finished.
	while(!transferFinished);
	CopyArray(ReceiveBuffer, rxData, length);
}

#pragma vector = USCIAB0TX_VECTOR
__interrupt void USCIAB0TX_ISR(void)
{

	// TODO: Read RX-Buffer or write TX-Buffer, depending on what you'd like to do.
    if (IFG2 & UCB0RXIFG)                 // Receive Data Interrupt
     {

         //Must read from UCB0RXBUF
         unsigned char rx_val = UCB0RXBUF;

         if (RXByteCtr)
         {
             ReceiveBuffer[ReceiveIndex++] = rx_val;
             RXByteCtr--;
         }

         if (RXByteCtr == 1)
         {
             UCB0CTL1 |= UCTXSTP;
         }
         else if (RXByteCtr == 0)
         {
             P1OUT |=BIT4;
             _delay_cycles(100);
             P1OUT &= ~BIT4;
             if (readStopFlag == 1) {
                 UCB0CTL1 |= UCTXSTP;
             }
             IE2 &= ~UCB0RXIE;          // disable RX interrupt
             MasterMode = IDLE_MODE;
             transferFinished = 1;
         }
     }
     else if (IFG2 & UCB0TXIFG)            // Transmit Data Interrupt
     {

         switch (MasterMode)
         {
//             case TX_REG_ADDRESS_MODE:
//                 UCB0TXBUF = TransmitRegAddr;
//                 MasterMode = TX_DATA_MODE;        // Continue to transmision with the data in Transmit Buffer
//                 break;

             case TX_DATA_MODE:

                 if (TXByteCtr)
                 {
                     UCB0TXBUF = TransmitBuffer[TransmitIndex++];
                     TXByteCtr--;
                 }
                 else
                 {
                     // send Stop condition only if previosly selected via stop flag
                     if (stopFlag != 0) {
                         //Done with transmission
                         UCB0CTL1 |= UCTXSTP;     // Send stop condition
                         MasterMode = IDLE_MODE;
                         IE2 &= ~UCB0TXIE;                       // disable TX interrupt
                     }
                     // transfer is finished eitherway
                     transferFinished = 1;
                 }
                 break;

             default:
                 __no_operation();
                 break;
         }
     }
    // Exit waiting mode after this interrupt, i.e. set the transferFinished variable.
	// TODO: Call this only when necessary

}

#pragma vector = USCIAB0RX_VECTOR
__interrupt void USCIAB0RX_ISR(void)
{
	// TODO: Check for NACKs
    if (UCB0STAT & UCNACKIFG)
    {
        UCB0STAT &= ~UCNACKIFG;             // Clear NACK Flags
    }
    if (UCB0STAT & UCSTPIFG)                        //Stop or NACK Interrupt
    {
        UCB0STAT &=
            ~(UCSTTIFG + UCSTPIFG + UCNACKIFG);     //Clear START/STOP/NACK Flags
    }
    if (UCB0STAT & UCSTTIFG)
    {
        UCB0STAT &= ~(UCSTTIFG);                    //Clear START Flags
    }
}
