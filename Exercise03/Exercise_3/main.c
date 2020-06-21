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

// 0: no sleep timout
// 1:sleep timout
unsigned char sleepTimeout = 0;
int x_valueTemp;

int main(void) {
    initMSP();
    // Set ACLK divider to /8
    BCSCTL1 += DIVA0 + DIVA1;
    // route VLOCLK to ACLK
    BCSCTL3 += LFXT1S1;

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


    // go to LMP4
    lcd_putText("Sleeping");
    __bis_SR_register(LPM4_bits + GIE);              // Enter LPM4 w/ interrupts
    lcd_clear();
    lcd_putText("bubble level x");

    TA0CTL = TASSEL_1 + MC_1 ;    // Tassel_1 -> ACLK ; MC_1 -> up. mode
    TA0CCR0 =  7500;             // results in a an interrupt after 2 s
    unsigned char i = 0;


    mma_setRange(0);                    // enter 2g range
    mma_setResolution(1);               // enter 14bit mode
    mma_read();
    __delay_cycles(10000);
    x_valueTemp = mma_get14X();
    while (1) {
        mma_read();
        __delay_cycles(10000);
        if(((x_valueTemp +250) < mma_get14X()) || ((x_valueTemp -250) > mma_get14X()) ) {
            TA0CCTL0 = CCIE;              // enable interrupt
            TAR = 0;                        // reset timer value
        }
        x_valueTemp = mma_get14X();
        lcd_cursorSet(i, 1);
        lcd_putChar(254);
        for(i=0; i<16;i++) {
            if(mma_get14X() < 512*(i-7)) {
                lcd_cursorSet(i, 1);
                lcd_putChar(255);
                break;
            }
        }
        if(sleepTimeout == 1) {
            sleepTimeout = 0;
            lcd_clear();
            lcd_putText("Sleeping Timout");
            __bis_SR_register(LPM4_bits + GIE);              // Enter LPM4 w/ interrupts
            lcd_clear();
            lcd_putText("bubble level x");
        }

    }
}
#pragma vector=PORT1_VECTOR
__interrupt void PORT_1(void) {
    __delay_cycles(1000000);                // to ignore initial peak
    if(isBitSet(P1IN, 4) == 1) {
        __bic_SR_register_on_exit(LPM4_bits);      // Exit LPM4

        P1IFG &= ~(BIT4);                   // reset interrupt flag
    } else {
        P1IFG &= ~(BIT4);                   // reset interrupt flag
    }
}

// Timer A0 interrupt service routine
# pragma vector = TIMER0_A0_VECTOR
__interrupt void Timer_A0 ( void ) {
    sleepTimeout = 1;

}


