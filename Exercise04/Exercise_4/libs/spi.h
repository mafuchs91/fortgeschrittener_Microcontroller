/***************************************************************************//**
 * @file    spi.h
 * @author  Max Fuchs
 *          Matr.: 4340529
 *          Email: maxfuchs@gmx.de
 * @date    8.7.2020
 * @SheetNr 4
 * @brief   basic function prototypes for communication via SPI
 *
 *
 ******************************************************************************/

#ifndef LIBS_SPI_H_
#define LIBS_SPI_H_

/******************************************************************************
 * INCLUDES
 *****************************************************************************/


/******************************************************************************
 * CONSTANTS
 *****************************************************************************/



/******************************************************************************
 * VARIABLES
 *****************************************************************************/



/******************************************************************************
 * FUNCTION PROTOTYPES
 *****************************************************************************/
// function implementing the reiceive isr functions, this will be called by isr.c in case of UART interrupt for SPI
// always returns 0, is necessary as the function from the i2c communication shares the same function pointer and needs further return value handling
unsigned char receive_spi_isr(void);

// Set the USCI-machine to SPI and switch the 74HCT4066 (1 pt.)
void spi_init(void);

// This function needs to be called before the write function !!
// Read <length> bytes into <rxData> (1 pt.)
void spi_read(unsigned char length, unsigned char * rxData);

// Write <length> bytes from <txData> (1 pt.)
void spi_write(unsigned char length, unsigned char * txData);

// Interrupt service routines in your spi.c (1 pt.)

// Returns 1 if the SPI is still busy or 0 if not.
// Note: this is optional. You will most likely need this, but you don't have
// to implement or use this.
unsigned char spi_busy(void);

#endif /* LIBS_SPI_H_ */
