/***************************************************************************//**
 * @file    main.c
 * @author  Max Fuchs
 *          Matr.: 4340529
 *          Email: maxfuchs@gmx.de
 * @date    8.7.2020
 * @brief   Exercise 4 - SPI
 *
 * Programm to display an welcome message stored in flash and afterwards lets the user enter letters for a new welcome message
 *  Pins:
 *      Connection of Controllerboard1 und 2
 *
 *      CON6:I2C_SPI to CON2:P1.3
 *      CON6:XSCL to CON2:P1.6
 *      CON6:XSDA to CON2:P1.7
 *      JP2:BKL_ON to the left position
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
 *      CON2:P1.5 - CON6:CC_CLK
 *      CON6:CC_SO to CON9:F_SO
 *      CON6:CC_SI to CON9:F_SI
 *      CON6:CC_CLK to CON9:F_CLK
 *      CON9:F_/CS to CON4:P3.4
 *      CON9:F_/HOLD to CON4:P3.3,
 *      CON9:F_/WP to CON4:P3.5
 *
 *
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

// current cursor position
unsigned char cursX;
unsigned char cursY;
// Buffer to receive the stored welcome message
unsigned char rxData[16];
// array with ascii whitespaces
unsigned char letters[16] = {32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32};
// current index for letters
unsigned char letterAddr = 0;
// Buffer to read the joystick input
unsigned char read [4] = {0,0,0,0};
// indication wheater joystick button is pressed
// 0: button was released in last cycle
// 1: button was pressed in last cycle
unsigned char buttonFlag = 0;

int main(void) {

    initMSP();
    lcd_init();
    lcd_enable(1);

    flash_init();
    flash_read(0, 16, rxData);      // read the flash message that was stored in flash
    int i = 0;
    for(i = 0; i< 16;i++) {
        lcd_putChar(rxData[i]);     // display the message
    }
    __delay_cycles(50000000);       // wait some time to display message
    // show the alphabet on the screen and add an sign ">" to submit the input
    lcd_clear();
    lcd_cursorSet(0,0);
    lcd_putText("ABCDEFGHIJKLM  >");
    lcd_cursorSet(0, 1);
    lcd_putText("NOPQRSTUVWXYZ");
    lcd_cursorSet(0, 0);
    // init cursor values
    cursX = 0;
    cursY = 0;
    // show cursor and blink such that a moving on the display is visible
    lcd_cursorShow(1);
    lcd_cursorBlink(1);
    adac_init();
    while (1) {
        __delay_cycles(2000000);
       adac_read(read);
       if(read[1] < 10 && cursX < 15) {                 // move right, check for display limit
           cursX += 1;
           lcd_cursorSet(cursX, cursY);
       } else if(read[1] > 245 && cursX > 0) {          // move left, check for display limit
           cursX -= 1;
           lcd_cursorSet(cursX, cursY);
       }
       if(read[2] > 245 && cursY == 0 ) {               // move down, check for display limit
          cursY += 1;
          lcd_cursorSet(cursX, cursY);
      } else if(read[2] < 10 && cursY == 1) {           // move up, check for display limit
          cursY -= 1;
          lcd_cursorSet(cursX, cursY);
      }
      if(read[3]>100) {                                 // Button pressed
          if (cursY ==0 && cursX ==15) {                // submit sign was pressed
              // write the stored letters in the letters array to the first 16 bytes of the flash memory
              flash_init();
              flash_write(0, 16, letters);
              // diplay message to restart the MCU and test the new welcome message
              lcd_clear();
              lcd_putText("Restart now!");
              while(1);                                 // loop here
          }
          if ((cursX == 13 || cursX == 14) ||(cursX == 15)) { // do nothing in any of the whitespace fields on the select screen are selected
          }else if(cursY == 0 && (buttonFlag == 0) ) {        // letter selected in upper display line
              letters[letterAddr] = 65 + cursX;         // calculate the ASCI letter selected
              letterAddr += 1;                          // increment the address index
          } else if(cursY == 1 && (buttonFlag == 0)) {  // letter selected in lower display line
              letters[letterAddr] = 65 + 13 + cursX;    // calc ASCI letter by extra 13 addition to get the letters from "N" onwards
              letterAddr += 1;
          }
          buttonFlag = 1;
      } else {
          buttonFlag = 0;
      }
    }
}
