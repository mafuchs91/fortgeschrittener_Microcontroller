/***************************************************************************//**
 * @file    main.c
 * @author  <your name>
 * @date    <date of creation>
 *
 * @brief   Exercise 1 - Display Interface
 *
 * Description and pin connections go here.
 *
 * @note    The project was exported using CCS 8.0.0.
 ******************************************************************************/

#include "libs/templateEMP.h"
#include "libs/LCD.h"

int main(void) {
    initMSP();

    lcd_init();
    lcd_enable(1);
    lcd_cursorShow(1);
    _delay_cycles(1000000);
    lcd_cursorSet(0, 0);
    _delay_cycles(1000000);
    lcd_putText("Hallodfgsdfgdfgdfgfgh");
    lcd_cursorSet(1, 1);
    lcd_putNumber(-7200);
    // TODO: Add your initialization here.
    while (1) {
        // TODO: Show a funky demo of what you did.
    }
}
