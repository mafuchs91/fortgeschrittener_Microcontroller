/***************************************************************************//**
 * @file    main.c
 * @author  Max Fuchs
 *          Matr.: 4340529
 *          Email: maxfuchs@gmx.de
 * @date    9.6.2020
 *
 * @brief   Exercise 2 - I2C
 *
 * main routine which reads the input from the joystick via i2c and sets the display brightness
 * according to the x deflection and the sound of the buzzer according to the y deflection.
 * pressing the button stops the sound.
 * PIN Connections:
 *
 *      Connection of Controllerboard1 und 2
 *      CON6:I2C_SPI to CON2:P1.3
 *      CON6:XSCL to CON2:P1.6
 *      CON6:XSDA to CON2:P1.7
 *      CON6:UDAC to CON5:BCKL
 *      JP2:BKL_ON to the right position
 *      CON3:P3.6 to X1:Buzzer
 *
 *
 * @note    The project was exported using CCS 8.0.0.
 *          UART is disabled within templateEMP.h in order to avoid
 *          interference with your I2C routine!
 *
 ******************************************************************************/

#include "libs/templateEMP.h"   // UART disabled, see @note!
#include "libs/adac.h"

// init read array to be filled with the adac_read function.
unsigned char read [4] = {0,0,0,0};
//0: no error(all acks received)
//1: no acknolgedge was return for a send byte
unsigned char ackn = 0;

int main(void) {
    initMSP();
    adac_init();

    // Setup the PWM of buzzer output 3.6
    P3DIR |= BIT6;
    P3SEL |= BIT6 ;             // P3 .6 TA0 .2 option -> found in device specific datasheet
    TA0CCTL2 = OUTMOD_3 ;       // CCR2 set / reset
    TA0CCR2 = 7000;                // CCR2 PWM duty cyxle
    TA0CTL = TASSEL_2 + MC_1 ;  // SMCLK ; MC_1 -> up mode ;

    while(1) {
        // read all AD channels, keep track of errors
        ackn = adac_read(read);
        // write the value from AD_x_joystick to the DA output
        ackn = adac_write(read[1]);
        // check if the joystick button was pressed
        if(read[3] > 100) {
            TA0CCR0 = 0;                // Mute
        } else {
            TA0CCR0 = 10000 + read[2]*20;                // set PWM period according to joystick position
        }
    }
  }

