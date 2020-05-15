/***************************************************************************//**
 * @file    LCD.c
 * @author  <your name>
 * @date    <date of creation>
 *
 * @brief   <brief description>
 *
 * Here goes a detailed description if required.
 ******************************************************************************/

#include "./LCD.h"
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


/******************************************************************************
 * VARIABLES
 *****************************************************************************/



/******************************************************************************
 * LOCAL FUNCTION PROTOTYPES
 *****************************************************************************/
void lcd_init (void);
void lcd_enable (unsigned char on);
void lcd_cursorSet (unsigned char x, unsigned char y);
void lcd_cursorShow (unsigned char on);
void lcd_cursorBlink (unsigned char on);
void lcd_putChar (char character);
void lcd_putText (char * text);
void lcd_putNumber (int number);



/******************************************************************************
 * LOCAL FUNCTION IMPLEMENTATION
 *****************************************************************************/


/******************************************************************************
 * FUNCTION IMPLEMENTATION
 *****************************************************************************/

// TODO: Implement the functions.
void lcd_init (void) {
    _LCDContr = 0x08;
    _LCDFunc = 0x20;

    // P3
    // BIT0 =  RS
    // BIT1 = R/W
    // BIT2 = E
    // set the registers for the control line to output
    P3DIR |= (BIT0 | BIT1 | BIT2);
    // P2
    // BIT0 = D4
    // BIT1 = D5
    // BIT2 = D6
    // BIT3 = D7
    // set the registers for the data lines  to output
    P2DIR |= (BIT0 | BIT1 | BIT2 | BIT3);
    // Wait minimum 40ms , according to Manual
    _delay_cycles(50000);
//    // Step 1
    P3OUT &= ~BIT0;
    P3OUT &= ~BIT1;
    P3OUT &= ~BIT2;
    P2OUT |= (BIT0 |BIT1);
    P2OUT &= ~BIT2;
    P2OUT &= ~BIT3;
    send_Enable();
// STEP 2
    send_Enable();
// Step 3
    send_Enable();
//
    // STEP 4
    P2OUT &= ~BIT0;
    send_Enable();
// 5 set display function
    _LCDFunc &= ~N;
    set_function();


// 6 Set display control
    _LCDContr &= ~D;
    set_display();

// 7
    lcd_clear();
// 8
    set_entry_mode();
}
void lcd_enable (unsigned char on) {
    if(on ==1) {
        serialPrintln("enabling LCD");
        _LCDContr |= D;
        set_display();
    }
    else {
        serialPrintln("disabled");
        _LCDContr &= ~D;
        set_display();
    }

}
void lcd_cursorSet (unsigned char x, unsigned char y) {
    // set the RS and R/W to zero
    P3OUT = 0;
    // set DDRAM adress for cursor positon, with 128 activiation bit and x and 64*y defining position
    // rightshift to send in 4 bit mode, this sends DB7 - DB4
    P2OUT = (x + 128 + 64*y) >> 4;
    send_Enable();

    // this sends DB3 - DB0 in 4 bit mode
    P2OUT = (x+128 + 64*y);
    send_Enable();

}
void lcd_cursorBlink (unsigned char on) {
    if (on == 1) {
        _LCDContr |= B;
        set_display();
    } else {
        _LCDContr &= ~ B;
        set_display();
    }
}


void lcd_cursorShow (unsigned char on) {
    if(on ==1) {
        serialPrintln("enabled Cursor");
        _LCDContr |= C;
        set_display();
    }
    else {
        serialPrintln("disabled");
        _LCDContr &= ~C;
        set_display();
    }
}

void lcd_putChar (char character) {
    serialPrintInt(character);
    P3OUT = 1;
    P2OUT = character >> 4;
    send_Enable();

    P2OUT = character;
    send_Enable();
}

void lcd_putText(char * text) {
    char len;
    // make sure the text is not written in the next line
    if(length(text) <= 16) {
        len = length(text);
    } else {
        len = 16;
    }
    int i;
    // send each character to the LCD screen, with cursor automatically moving right
    for(i=0; i < len;i++) {
       P3OUT = 1;
       P2OUT = text[i] >> 4;
       send_Enable();

       P2OUT = text[i];
       send_Enable();
   }
}

void lcd_putNumber (int number){
    // allocate char array, anything above 16 should be sufficient of the display size
    char snum[17];
    // int to char * conversion
    sprintf(snum, "%d", number);

    if (number >= 0){
        lcd_putText(snum);
    } else {
        strncat("-", &snum, 1);
        lcd_putText(snum);
    }
}

char length(char * text) {
    int  i = 0;
   // Count the number of bytes we shall display.
   while (text[i] != 0x00)
   {
       i++;
   }
   return i;
}

void set_display() {
    P3OUT = to_zero;
    P2OUT = _LCDContr >> 4;
    send_Enable();

    P2OUT = _LCDContr;
    send_Enable();
}
void set_function() {
    P3OUT = to_zero;
    P2OUT = _LCDFunc << 4;
    send_Enable();

    P2OUT = _LCDFunc;
    send_Enable();
}
void lcd_clear() {
    P3OUT = to_zero;
    P2OUT = to_zero;
    send_Enable();

    P2OUT = 0x01;
    send_Enable();
}
void set_entry_mode() {
    P3OUT = to_zero;
    P2OUT = to_zero;
    send_Enable();

    P2OUT = 0x06;
    send_Enable();
}

void send_Enable() {
    P3OUT &= ~BIT2;
    _delay_cycles(1);
    P3OUT |= BIT2;
    _delay_cycles(1);
    P3OUT &= ~BIT2;
    _delay_cycles(1);
    _delay_cycles(10000);
}

