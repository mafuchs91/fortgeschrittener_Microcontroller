/***************************************************************************//**
 * @file    main.c
 * @author  Max Fuchs
 *          Matr.: 4340529
 *          Email: maxfuchs@gmx.de
 * @date    23.5.2020
 *
 * @brief   Exercise 1 - Display Interface
 *
 *Description:
 *  This main routine shows a small demo application to proof that all functions defined in LCD.h are working.
 *
 *Jumpers JP1, JP2, JP3 are set to default config
 *
 *Pin connection:
 *
 *CON5:D4 -  CON3:P2.0
 *CON5:D5 -  CON3:P2.1
 *CON5:D6 -  CON3:P2.2
 *CON5:D7 -  CON3:P2.3
 *
 *CON5:RS -  CON4:P3.0
 *CON5:R/W - CON4:P3.1
 *CON5:E  -  CON4:P3.2
 *
 *
 * @note    The project was exported using CCS  9.2.0.00013
 ******************************************************************************/

#include "libs/templateEMP.h"
#include "libs/LCD.h"

int main(void) {
    initMSP();

    lcd_init();
    lcd_enable(1);
    lcd_cursorShow(1);
    lcd_cursorBlink(1);
    lcd_putText("WELCOME");
    lcd_cursorSet(0, 1);
    lcd_putText("Lets start ...");
    _delay_cycles(2000000);
    while (1) {
        lcd_clear();
        lcd_cursorSet(0, 0);
        lcd_putText("Demo app");
        lcd_cursorSet(0, 1);
        lcd_putText("Curr temp:");
        lcd_putNumber(10);
        // create degree sign in CGRAM
        char pattern[8] = {6,9,6,0,0,0,0,0};
        set_custom_pattern(pattern);
        lcd_cursorSet(12,1 );
        lcd_putChar(0);
        lcd_putChar('C');
        lcd_cursorShow(0);
        lcd_cursorBlink(0);
        _delay_cycles(5000000);
        lcd_clear();
        lcd_cursorSet(0, 0);
        lcd_putText("Demo app");
        lcd_cursorSet(0, 1);
        lcd_putText("Curr Voltage:");
        lcd_putNumber(-1);
        // create degree sign in CGRAM
        lcd_putChar('V');
        _delay_cycles(5000000);
    }
}
