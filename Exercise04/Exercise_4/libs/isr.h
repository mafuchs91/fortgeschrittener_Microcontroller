/*
 * isr.h
 *
 *  Created on: 05.07.2020
 *      Author: max
 */


void set_receive_isr(unsigned char(* function_pointer)());
void set_transmit_isr(unsigned char(* function_pointer)());



