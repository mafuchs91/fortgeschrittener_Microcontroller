/***************************************************************************//**
 * @file    main.c
 * @author  <your name>
 * @date    <date of creation>
 *
 * @brief   Exercise 4 - SPI
 *
 * Description and pin connections go here.
 *
 * @note    The project was exported using CCS 8.0.0.
 *
 ******************************************************************************/

#include "libs/templateEMP.h"   // UART disabled, see @note!
//#include "libs/lcd.h"
#include "libs/isr.h"
//#include "libs/spi.h"
#include "libs/flash.h"
#include "libs/LCD.h"


unsigned char rxData[16];
unsigned char rxData1[16];
unsigned char txData[16] = {200,200,200,200,200,200,200,200,200,200,200,200,200,200};

int main(void) {

    initMSP();
    lcd_init();
    lcd_enable(1);
    flash_init();
    flash_read(0, 16, rxData);
    int i = 0;
    for(i=0;i<3;i++) {
        lcd_putNumber(rxData[i]);
        lcd_putText(" ");
    }
    lcd_cursorSet(0, 1);
    flash_write(0, 16, txData);

    flash_read(0, 16, rxData1);

    i= 0;
    for(i=0;i<3;i++) {
        lcd_putNumber(rxData1[i]);
        lcd_putText(" ");
    }

    while (1) {
        // TODO: Add your main program here.
    }
}
