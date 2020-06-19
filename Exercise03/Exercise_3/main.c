/***************************************************************************//**
 * @file    main.c
 * @author  <your name>
 * @date    <date of creation>
 *
 * @brief   Exercise 3 - Advanced Sensors
 *
 * Description and pin connections go here.
 *PINS: CON6:MMAINT1 to CON3: P1.0
 *
 * @note    The project was exported using CCS 8.0.0.
 *
 ******************************************************************************/

#include "libs/templateEMP.h"   // UART disabled, see @note!
#include "libs/mma.h"
#include "libs/lcd.h"

int main(void) {

    initMSP();
    lcd_init();
    lcd_enable(1);
    lcd_cursorShow(1);
    lcd_cursorBlink(1);
//    lcd_putText("WELCOME");
//    int i = 1000;
//    lcd_putNumber(i);
    // TODO: Add your initialization here.
    mma_init();
    mma_selftest();

    while (1) {
        // TODO: Add your main program here.
    }
}
