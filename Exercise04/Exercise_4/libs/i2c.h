/***************************************************************************//**
 * @file    i2c.h
 * @author  Max Fuchs
 *          Matr.: 4340529
 *          Email: maxfuchs@gmx.de
 * @date    8.7.2020
 * @SheetNr 4
 *
 * @brief   structure for i2c communication.
 *
 * Here goes a detailed description if required.
 ******************************************************************************/

#ifndef EXERCISE_LIBS_I2C_H_
#define EXERCISE_LIBS_I2C_H_

/******************************************************************************
 * INCLUDES
 *****************************************************************************/
#include "isr.h"
#include <msp430g2553.h>

/******************************************************************************
 * CONSTANTS
 *****************************************************************************/
// define arbitrary, big enough buffer space for receive and transmit
#define MAX_BUFFER_SIZE     20


/******************************************************************************
 * VARIABLES
 *****************************************************************************/



/******************************************************************************
 * FUNCTION PROTOTYPES
 *****************************************************************************/
// function implementing the reiceive isr functions, this will be called by isr.c in case of UART interrupt for I2C
// returns 0 if no nack is received, return 1 if an Nack is received and wakes up from sleep mode
unsigned char receive_i2c_isr(void);
// function implementing the transmit isr functions, this will be called by isr.c in case of UART interrupt for I2C
// returns 0 if there are more bytes to read and write, return 1 if all bytes are read and read and the controller is to wake up from sleep mode
unsigned char transmit_i2c_isr(void);

// Initialize the I2C state machine. The speed should be 100 kBit/s.
// <addr> is the 7-bit address of the slave (MSB(Most significant bit) shall always be 0, i.e. "right alignment"). (2 pts.)
void i2c_init (unsigned char addr);

// Write a sequence of <length> characters from the pointer <txData>.
// Return 0 if the sequence was acknowledged, 1 if not. Also stop transmitting further bytes upon a missing acknowledge.
// Only send a stop condition if <stop> is not 0. (2 pts.)
unsigned char i2c_write(unsigned char length, unsigned char * txData, unsigned char stop);

// Returns the next <length> characters from the I2C interface. (2 pts.)
void i2c_read(unsigned char length, unsigned char * rxData);

#endif /* EXERCISE_LIBS_I2C_H_ */
