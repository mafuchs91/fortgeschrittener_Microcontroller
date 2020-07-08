/*
 * isr.c
 * @author  Max Fuchs
 *          Matr.: 4340529
 *          Email: maxfuchs@gmx.de
 * @date    8.7.2020
 * @SheetNr 4
 * @brief   function implementations for the handling of SPI and I2C interrupt routines
 */

#include "./isr.h"
#include <msp430g2553.h>


// function pointers to be set by the setters
unsigned char (*receive_function_pointer)(void);
unsigned char (*transmit_function_pointer)(void);

void set_receive_isr(unsigned char(* function_pointer)()) {
    receive_function_pointer = function_pointer;
}

void set_transmit_isr(unsigned char(* function_pointer)()) {
    transmit_function_pointer = function_pointer;
}


#pragma vector=USCIAB0RX_VECTOR
__interrupt void SPI_USCIA0RX_ISR(void)
{
    // this is necessary for the i2c to return to the low power mode once all bytes are received
    if(receive_function_pointer() == 1) {
        __bic_SR_register_on_exit(LPM4_bits);      // Exit LPM4
    }
}


#pragma vector=USCIAB0TX_VECTOR
__interrupt void SPI_USCIA0TX_ISR(void)
{
    // this is necessary for the i2c to return to the low power mode once all bytes are transmitted
    if(transmit_function_pointer() == 1) {
        __bic_SR_register_on_exit(LPM4_bits);      // Exit LPM4
    }

}


