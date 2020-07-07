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
#include "libs/adac.h"

unsigned char cursX;
unsigned char cursY;
unsigned char rxData[16];
unsigned char rxData1[16];
unsigned char txData[16] = {65,200,200,200,200,200,200,200,200,200,200,200,200,200};
unsigned char letters[16] = {32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32};
unsigned char letterAddr = 0;
unsigned char read [4] = {0,0,0,0};
unsigned char buttonFlag = 0;

int main(void) {

    initMSP();
    lcd_init();
    lcd_enable(1);

    flash_init();
    flash_read(0, 16, rxData);
    int i = 0;
    for(i = 0; i< 16;i++) {
        lcd_putChar(rxData[i]);
    }
    __delay_cycles(50000000);
    lcd_clear();
    lcd_cursorSet(0,0);
    lcd_putText("ABCDEFGHIJKLM  >");
    lcd_cursorSet(0, 1);
    lcd_putText("NOPQRSTUVWXYZ");
    lcd_cursorSet(0, 0);
    cursX = 0;
    cursY = 0;
    lcd_cursorShow(1);
    lcd_cursorBlink(1);
    while (1) {
        __delay_cycles(2000000);
       adac_init();
       adac_read(read);
       if(read[1] < 10 && cursX < 15) {           // move right
           cursX += 1;
           lcd_cursorSet(cursX, cursY);
       } else if(read[1] > 245 && cursX > 0) {       // move left
           cursX -= 1;
          lcd_cursorSet(cursX, cursY);
       }
       if(read[2] > 245 && cursY == 0 ) {           // move down
//          if( cursY == 1) {
//              continue;
//          }
          cursY += 1;
          lcd_cursorSet(cursX, cursY);
      } else if(read[2] < 10 && cursY == 1) {        // move up
//         if(cursY == 0) {
//             continue;
//         }
          cursY -= 1;
         lcd_cursorSet(cursX, cursY);
      }
      if(read[3]>100) {                 // Button pressed

          if (cursY ==0 && cursX ==15) {
              flash_init();
              flash_write(0, 16, letters);
              lcd_clear();
              lcd_putText("Restart now!");
          }
          if(cursY == 0 && (buttonFlag == 0) ) {
              letters[letterAddr] = 65 + cursX;
              letterAddr += 1;
          } else if(cursY == 1 && (buttonFlag == 0)) {
              letters[letterAddr] = 65 + 13 + cursX;
              letterAddr += 1;
          }
          buttonFlag = 1;

      } else {
          buttonFlag = 0;
      }




    }
}
