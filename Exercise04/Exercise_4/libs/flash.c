/***************************************************************************//**
 * @file    flash.h
 * @author  Max Fuchs
 *          Matr.: 4340529
 *          Email: maxfuchs@gmx.de
 * @date    8.7.2020
 * @SheetNr 4
 * @brief   Function prototypes for the communication interface for flash memory M25P16
 *
 *
 ******************************************************************************/
#include "flash.h"
#include "./spi.h"
#include "LCD.h"
#include <msp430g2553.h>

// array for instruction + adress(if necessary)
unsigned char instructionAdress[4] ={0,0,0,0};
// buffer for receiving one byte
unsigned char recBuffer[1] = {0};


void flash_init(void) {
    // init the CS
    P3DIR |= BIT4;
    P3OUT |= BIT4;           // chip not selected
    spi_init();
    // init hold and write protect
    P3DIR |= (BIT3 + BIT5);    // define as output
    P3SEL &= ~(BIT3 + BIT5);
    P3OUT |= (BIT3 + BIT5);    // deactivate hold and WP
}

// the adress is 24 bit -> 3 byte
void flash_read(long int address, unsigned char length, unsigned char * rxData){
    while(spi_busy());
    // build instruction + store the 24 bits as 3 bytes
    instructionAdress[0] = 0x03;
    instructionAdress[1] = address & 0x00ff0000 ;
    instructionAdress[2] = address & 0x0000ff00;
    instructionAdress[3] = address & 0x000000ff;

    P3OUT &= ~BIT4;                 // chip selected
    spi_read(length, rxData);       // define read destination and length
    spi_write(4, instructionAdress);
    while(spi_busy());
    P3OUT |= BIT4;                  // chip deselect
}

void flash_write(long int address, unsigned char length, unsigned char * txData){
    // enable write
    instructionAdress[0] = 6;
    P3OUT &= ~BIT4;
    spi_write(1, instructionAdress);
    while(spi_busy());
    P3OUT |= BIT4;

    // sector erase
    instructionAdress[0] = 0xd8;
    instructionAdress[1] = address & 0x00ff0000;
    instructionAdress[2] = address & 0x0000ff00;
    instructionAdress[3] = address & 0x000000ff;
    P3OUT &= ~BIT4;
    spi_write(4, instructionAdress);
    while(spi_busy());
    P3OUT |= BIT4;
    while(flash_busy());

    //enable write
    instructionAdress[0] = 6;
    P3OUT &= ~BIT4;
    spi_write(1, instructionAdress);
    while(spi_busy());
    P3OUT |= BIT4;


    // page program
    instructionAdress[0] = 0x02;
    instructionAdress[1] = address & 0x00ff0000;
    instructionAdress[2] = address & 0x0000ff00;
    instructionAdress[3] = address & 0x000000ff;
    P3OUT &= ~BIT4;
    spi_write(4, instructionAdress);
    spi_write(length, txData);
    while(spi_busy());
    P3OUT |= BIT4;
    while(flash_busy());

    // disable write
    instructionAdress[0] = 4;
    P3OUT &= ~BIT4;
    spi_write(1, instructionAdress);
    while(spi_busy());
    P3OUT |= BIT4;

    while(flash_busy());

}

unsigned char flash_busy(void) {
    // read status register
    instructionAdress[0] = 5;
    P3OUT &= ~BIT4;
    spi_read(1, recBuffer);
    spi_write(1, instructionAdress);
    while(spi_busy());
    P3OUT |= BIT4;
    // check the busy flag of the status register
    if(recBuffer[0] & BIT0){
        return 1;               //busy
    } else {
        return 0;               // not busy
    }
}

