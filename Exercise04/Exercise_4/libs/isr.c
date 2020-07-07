/*
 * isr.c
 *
 *  Created on: 05.07.2020
 *      Author: max
 */

#include "./isr.h"
#include <msp430g2553.h>



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


