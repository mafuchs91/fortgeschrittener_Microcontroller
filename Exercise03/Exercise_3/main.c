/***************************************************************************//**
 * @file    main.c
 * @author  <your name>
 * @date    <date of creation>
 *
 * @brief   Exercise 3 - Advanced Sensors
 *
 * Description and pin connections go here.
 *PINS: CON6:MMAINT1 to CON3: P1.4
 *
 * @note    The project was exported using CCS 8.0.0.
 *
 ******************************************************************************/

#include "libs/templateEMP.h"   // UART disabled, see @note!
#include "libs/mma.h"
#include "libs/lcd.h"

int main(void) {
    initMSP();


    // init LCD display
    lcd_init();
    lcd_enable(1);
//    lcd_cursorShow(1);

//    lcd_cursorBlink(1);


    // init mma
    mma_init();
    // perform selftest until no error occurs
//    mma_selftest();

//    mma_selftest();
    mma_enableTapInterrupt();
    __delay_cycles(100000);
    // define interrupt for MSP430
    P1DIR &= ~BIT4;          // Setzte P1.4 als Input
    P1IES &= ~BIT4;          // react on low to high transition
    P1IE |= BIT4;            // Enable interrupt for P1.4
    P1IFG &= ~BIT4;          // Clear interrupt flag


    // go to LMP0
    lcd_putText("Sleeping");
    __bis_SR_register(LPM4_bits + GIE);              // Enter LPM4 w/ interrupts
    unsigned char i = 0;


    mma_setRange(0);                    // enter 2g range
    mma_setResolution(1);
    while (1) {

        mma_read();
        __delay_cycles(10000);
        lcd_cursorSet(i, 1);
        lcd_putChar(254);
        for(i=0; i<16;i++) {
            if(mma_get14X() < 512*(i-7)) {
                lcd_cursorSet(i, 1);
                lcd_putChar(255);
                break;
            }
        }
        __delay_cycles(10000000);
    }
}
#pragma vector=PORT1_VECTOR
__interrupt void PORT_1(void) {
    __delay_cycles(1000000);                // to ignore initial peak
    if(isBitSet(P1IN, 4) == 1) {
        __bic_SR_register_on_exit(LPM4_bits);      // Exit LPM4
        lcd_clear();
        lcd_putText("bubble level x");
        __delay_cycles(10000000);
        P1IFG &= ~(BIT4);                   // reset interrupt flag
    } else {
        P1IFG &= ~(BIT4);                   // reset interrupt flag
    }
}

