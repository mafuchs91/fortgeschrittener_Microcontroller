/***************************************************************************//**
 * @file    main.c
 * @author  Max Fuchs
 *          Matr.: 4340529
 *          Email: maxfuchs@gmx.de
 * @date    22.6
 *
 * @brief   Exercise 3 - Advanced Sensors
 *
 * This main routine performs a selftetst of the mma sensor. Displays the result of the selftests.
 * Afterwards the device goes into LPM4 until a double tap wakes it up again and displays a bubble level. If
 * no real changes in the level occur for about 5 s, the device goes into LPM4 again. This can be woken up with the dou
 * ble tap again. Before the first sleep the device dimms the lcd display and lights it up again when woken up.
 *
 *PINS: CON6:MMAINT1 to CON3: P1.4
 *
 *      Connection of Controllerboard1 und 2
 *
 *      CON6:I2C_SPI to CON2:P1.3
 *      CON6:XSCL to CON2:P1.6
 *      CON6:XSDA to CON2:P1.7
 *      CON6:UDAC to CON5:BCKL
 *      JP2:BKL_ON to the right position
 *
 *      CON5:D4 -  CON3:P2.0
 *      CON5:D5 -  CON3:P2.1
 *      CON5:D6 -  CON3:P2.2
 *      CON5:D7 -  CON3:P2.3
 *
 *      CON5:RS -  CON4:P3.0
 *      CON5:R/W - CON4:P3.1
 *      CON5:E  -  CON4:P3.2
 *
 *
 * @note    The project was exported using CCS 8.0.0.
 *
 ******************************************************************************/

#include "libs/templateEMP.h"   // UART disabled, see @note!
#include "libs/mma.h"
#include "libs/lcd.h"
#include "libs/adac.h"

// Flag indication the main routine to go into sleep mode , due to timeout of ne change in level.
// 0: no sleep timout
// 1 :sleep timout
unsigned char sleepTimeout = 0;
int x_valueTemp;                    // x level value stored from last read cycle

// keeps track of errors
// 0: no error
// 1: i2c connection error
// 2: mma selftest error
unsigned char errFlag = 0;


int main(void) {
    initMSP();
    BCSCTL1 += DIVA0 + DIVA1;               // Set ACLK divider to /8
    BCSCTL3 += LFXT1S1;                     // route VLOCLK to ACLK
    // init LCD display and enable it
    lcd_init();
    lcd_enable(1);

    // init mma
    mma_init();
    lcd_putText("Selftests run...");
    _delay_cycles(10000000);                // make the text visible for a while
//     perform selftest until no error occurs
    while(1){
        errFlag |= mma_selftest();               // perform selftest of mma
        if(errFlag == 0) {
            lcd_clear();
            lcd_putText("Selftest finish");
            _delay_cycles(10000000);
            break;
        } else {
            lcd_clear();
            lcd_cursorSet(0, 1);
            lcd_putText("Err:");
            if(errFlag == 1) {
                lcd_putText("i2c");
            } else {
                lcd_putText("selftest");
            }
            _delay_cycles(100000000);                // show error for a while on display
            lcd_clear();
            lcd_cursorSet(0,0);
            lcd_putText("Selftests run...");
        }
    }
    mma_enableTapInterrupt();
    // define interrupt for MSP430 which is wired to the MMAINT1 from the mma
    P1DIR &= ~BIT4;          // Setzte P1.4 als Input
    P1IES &= ~BIT4;          // react on low to high transition
    P1IE |= BIT4;            // Enable interrupt for P1.4
    P1IFG &= ~BIT4;          // Clear interrupt flag

    // signal user, device in sleep mode
    lcd_clear();
    lcd_putText("Sleeping");
    __bis_SR_register(LPM4_bits + GIE);              // Enter LPM4 w/ interrupts

    // after wake up start displaying the bubble level
    lcd_clear();
    lcd_putText("bubble level x");

    // init the interreupt counter for bubble level timeout
    TA0CTL = TASSEL_1 + MC_1 ;    // Tassel_1 -> ACLK ; MC_1 -> up. mode
    TA0CCR0 =  7500;             // results in a an interrupt after roughly 5 s
    TA0CCTL0 = CCIE;              // enable interrupt

    unsigned char i = 0;

    // set specs of mma for bubble level

    mma_setRange(0);                    // enter 2g range
    mma_setResolution(1);               // enter 14bit mode
    mma_read();
    __delay_cycles(10000);
    x_valueTemp = mma_get14X();         // store x axis value in temp variable
    while (1) {
        mma_read();
        // check if x value is not changing in an intervall of +- 1000 in compared to the last
        // measurement
        // If the bubble level is still in use reset the counter value
        if(((x_valueTemp +1000) < mma_get14X()) || ((x_valueTemp -1000) > mma_get14X()) ) {
            TA0CCTL0 = CCIE;              // enable interrupt
            TAR = 0;                        // reset timer value
        }
        x_valueTemp = mma_get14X();         // store for next iteration
        // display the level bubble
        lcd_cursorSet(i, 1);
        lcd_putChar(254);
        // check in which of the 16 diplay segments the bubble should be
        for(i=0; i<16;i++) {
            // each segment has a delta of 512 due to range and resolution
            if(mma_get14X() < 512*(i-7)) {
                lcd_cursorSet(i, 1);
                lcd_putChar(255);
                break;
            }
        }
        // enter sleeping timout when bubble level is inactive
        if(sleepTimeout == 1) {
            sleepTimeout = 0;           // reset variable
            lcd_clear();
            lcd_putText("Sleeping Timout");
            // init adac to dimm the lcd backgroundlight
            adac_init();
            adac_write(0);
            _delay_cycles(1000);
            __bis_SR_register(LPM4_bits + GIE);              // Enter LPM4 w/ interrupts
            // init adac to light up the lcd backgroundlight afterwards
            adac_init();
            adac_write(255);
            _delay_cycles(1000);
            lcd_clear();
            lcd_putText("bubble level x");
            // init mma, as i2c is still configured for analog digital converter
            mma_init();
            mma_enableTapInterrupt();
            mma_setRange(0);
            mma_setResolution(1);               // enter 14bit mode
        }
    }
}
#pragma vector=PORT1_VECTOR
__interrupt void PORT_1(void) {
    // interrupt to catch double tap of MMA
    __delay_cycles(10000);                // to ignore initial peak
    // check if this wasnt an artefact but a propper level change
    if(isBitSet(P1IN, 4) == 1) {
        __bic_SR_register_on_exit(LPM4_bits);      // Exit LPM4
        P1IFG &= ~(BIT4);                   // reset interrupt flag
    } else {
        P1IFG &= ~(BIT4);                   // reset interrupt flag
    }
}

// Timer A0 interrupt service routine for bubble level sleep
# pragma vector = TIMER0_A0_VECTOR
__interrupt void Timer_A0 ( void ) {
    sleepTimeout = 1;
}


