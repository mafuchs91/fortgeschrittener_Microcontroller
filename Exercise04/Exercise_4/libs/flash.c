/***************************************************************************//**
 * @file    flash.h
 * @author  <your name>
 * @date    <date of creation>
 *
 * @brief   <brief description>
 *
 * Here goes a detailed description if required.
 ******************************************************************************/
#include "flash.h"
#include "./spi.h"
#include "LCD.h"
#include <msp430g2553.h>


unsigned char commandBuffer[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
unsigned char instructionAdress[4] ={0,0,0,0};
unsigned char command[4] = {0x9f,0,0,0};
unsigned char recBuffer[3] = {0,0,0};
//unsigned char txData[16] = {1,2,3,4,5,6,7,1,2,3,4,5,6,7};


void flash_init(void) {
    P3DIR |= BIT4;
    P3OUT |= BIT4;           // chip not selected
    spi_init();

    P3DIR |= (BIT3 + BIT5);    // define as output
    P3SEL &= ~(BIT3 + BIT5);
    P3OUT |= (BIT3 + BIT5);    // deactivate hold and WP

    P3OUT &= ~BIT4;
    spi_read(3, recBuffer);
    spi_write(1, command);

    // tranceive
//    spi_write(4, command);
    while(spi_busy());
    P3OUT |= BIT4;



}

// long 32 bit
// the adress is 24 bit -> 3 byte
void flash_read(long int address, unsigned char length, unsigned char * rxData){
    while(spi_busy());
    instructionAdress[0] = 0x03;
    instructionAdress[1] = address & 0x00ff0000 ;
    instructionAdress[2] = address & 0x0000ff00;
    instructionAdress[3] = address & 0x000000ff;

    P3OUT &= ~BIT4;                 // chip selected
    // tranceive
    //    spi_write(2, instructionAdress);
//    spi_write(length, rxData);
    spi_read(length, rxData);
    spi_write(4, instructionAdress);
//    __delay_cycles(500);
    while(spi_busy());
    P3OUT |= BIT4;                  // chip deselect


}


void flash_write(long int address, unsigned char length, unsigned char * txData){
 // enable write
    command[0] = 6;
    P3OUT &= ~BIT4;
    spi_write(1, command);
    while(spi_busy());
    P3OUT |= BIT4;
//    __delay_cycles(100000);

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
    command[0] = 6;
    P3OUT &= ~BIT4;
    spi_write(1, command);
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
      command[0] = 4;
      P3OUT &= ~BIT4;
      spi_write(1, command);
      while(spi_busy());
      P3OUT |= BIT4;

      while(flash_busy());

}


unsigned char flash_busy(void) {
    command[0] = 5;
    P3OUT &= ~BIT4;
    spi_read(1, recBuffer);
    spi_write(1, command);
    while(spi_busy());
    P3OUT |= BIT4;
    if(recBuffer[0] & BIT0){
        return 1;               //busy
    } else {
        return 0;               // not busy
    }
}

