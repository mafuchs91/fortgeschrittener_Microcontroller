/*
 * isr.h
 * @author  Max Fuchs
 *          Matr.: 4340529
 *          Email: maxfuchs@gmx.de
 * @date    8.7.2020
 * @SheetNr 4
 *
 * @brief   function prototypes for the handling of SPI and I2C interrupt routines
 */

// binds a function pointer to the UART RX interrupt routine
void set_receive_isr(unsigned char(* function_pointer)());
// binds a function pointer to the UART TX interrupt routine
void set_transmit_isr(unsigned char(* function_pointer)());



