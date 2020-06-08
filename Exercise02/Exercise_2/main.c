/***************************************************************************//**
 * @file    main.c
 * @author  <your name>
 * @date    <date of creation>
 *
 * @brief   Exercise 2 - I2C
 *
 * Description and pin connections go here.
 *
 * @note    The project was exported using CCS 8.0.0.
 *          UART is disabled within templateEMP.h in order to avoid
 *          interference with your I2C routine!
 *
 ******************************************************************************/

#include "libs/templateEMP.h"   // UART disabled, see @note!
#include "libs/i2c.h"

unsigned char result[4];

int main(void) {
    initMSP();
    i2c_init(72);
    // send control bit to set A/D for channel 0
    P1OUT |=BIT4;
    _delay_cycles(100);
    P1OUT &= ~BIT4;
    unsigned char write [1] = {4};
    // write the control bit
    i2c_write(1, write, 1);
//    _delay_cycles(1000);
    // init receive array
    while(1) {
    unsigned char read [4] = {0,0,0,0};
    // read the last 4 bytes from channel 0
    i2c_read(4, read);


//    unsigned char write2 [2] = {64,read[3]};
    i2c_write(2, {64,read[3]}, 1);
    }


  }

