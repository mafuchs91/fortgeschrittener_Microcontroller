
/***************************************************************************//**
 * @file    LCD.c
 * @author  Max Fuchs
 *          Matr.: 4340529
 *          Email: maxfuchs@gmx.de
 * @date    8.7.2020
 * @SheetNr 4
 *
 *
 *
 * This file implents the prototypes which are defined in LCD.h.
 * These functions are giving an Interface to the HD44780-based LCD display.
 *
 * @note delays are adapted for use with 16Mhz cpu!!
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
// returns the length of an String
char length(char * text);

// writes the byte from _LCDContr to the instruction register
void set_display(void);

// sets the LCD Display to entry mode
void set_entry_mode(void);

// executes one clock with the enable signal to submit the data to the display
void send_Enable(void);

// writes 8 bit in 4 bit mode
void write_data(char byte);

// writes the byte for _LCDFunc to the instruction register
void set_function(void);

/******************************************************************************
 * LOCAL FUNCTION IMPLEMENTATION
 *****************************************************************************/
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
    P3OUT &= ~(BIT0 + BIT1 + BIT2) ;
    write_data(_LCDContr);
}

void set_function() {
    P3OUT &= ~(BIT0 + BIT1 + BIT2) ;
    write_data(_LCDFunc);
    // wait for at least 37탎
    __delay_cycles(5000);
}

void set_entry_mode() {
    P3OUT &= ~(BIT0 + BIT1 + BIT2) ;
    write_data(6);
    // wait for at least 37탎
    __delay_cycles(5000);
}

void send_Enable() {
    // makes sure the enable signal has a pulse width of at least 450ns and 1000 ns for a full enable cycle
    P3OUT &= ~BIT2;
    _delay_cycles(1000);
    P3OUT |= BIT2;
    _delay_cycles(2000);
    P3OUT &= ~BIT2;
}

void write_data(char byte) {
    // rightshift to send in 4 bit mode, this sends DB7 - DB4
    P2OUT = byte >> 4;
    send_Enable();
    // this sends  DB3 - DB0
    P2OUT = byte;
    send_Enable();
}


/******************************************************************************
 * FUNCTION IMPLEMENTATION
 *****************************************************************************/

void lcd_init (void) {
    // Init LCDContr and LCDFunc variables
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


    //############################## START INITIALIZATION ROUTINE FROM PAGE 46 Figure 26
    // Wait minimum 40ms , according to Manual
    _delay_cycles(5000000);

    // Step 1 function set
    P3OUT &= ~(BIT0 + BIT1 + BIT2) ;
    P2OUT |= (BIT0 |BIT1);
    P2OUT &= ~BIT2;
    P2OUT &= ~BIT3;
    // only one send enable necessary due to 8 bit mode
    send_Enable();
    // wait for at least 4.1 ms
    __delay_cycles(500000);

    // STEP 2 function set
    send_Enable();
    // wait for at least 100 탎
    __delay_cycles(15000);

    // Step 3
    // wait for at least 37탎
    __delay_cycles(5000);
    send_Enable();

    // STEP 4 set Interface to be 4 bits long
    P2OUT &= ~BIT0;
    // wait for at least 37탎
    __delay_cycles(5000);
    send_Enable();

    // STEP 5 set display function
    _LCDFunc |= Number_display_lines;
    set_function();

    // 6 Set display control
    _LCDContr &= ~D;
    set_display();

    // 7 display clear
    lcd_clear();

    // 8 entry mode
    set_entry_mode();
}
void lcd_enable (unsigned char on) {
    if(on ==1) {
        // set D bit to high
        _LCDContr |= D;
        set_display();
    }
    else {
        // set D bit to low
        _LCDContr &= ~D;
        set_display();
    }

}
void lcd_cursorSet (unsigned char x, unsigned char y) {
    // set the RS and R/W to zero
    P3OUT &= ~(BIT0 + BIT1 + BIT2) ;
    // set DDRAM adress for cursor positon, with 128 activiation bit and x and 64*y defining position
    write_data((x + 128 + 64*y));
}

void lcd_cursorBlink (unsigned char on) {
    if (on == 1) {
        // set B bit to high
        _LCDContr |= B;
        set_display();
    } else {
        // set B bit to low
        _LCDContr &= ~ B;
        set_display();
    }
}

void lcd_cursorShow (unsigned char on) {
    if(on ==1) {
        _LCDContr |= Cursor;
        set_display();
    }
    else {
        _LCDContr &= ~Cursor;
        set_display();
    }
}

void lcd_putChar (unsigned char character) {
    // Set RS to high, R/W to low and E to low
    P3OUT |= BIT0;
    P3OUT &= ~(BIT1 + BIT2);
    write_data(character);
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
       P3OUT |= BIT0;
       P3OUT &= ~(BIT1 + BIT2);
       write_data(text[i]);
   }
}

void lcd_putNumber (int number){
    // allocate char array, anything above 16 should be sufficient of the display size
    char snum[17];
    // int to char * conversion
    sprintf(snum, "%d", number);

    // make sure negative numbers are written with a minus sign
    if (number >= 0){
        lcd_putText(snum);
    } else {
        strncat("-", &snum, 1);
        lcd_putText(snum);
    }
}

void lcd_clear() {
    P3OUT &= ~(BIT0 + BIT1 + BIT2) ;
    // writes only DB0 as high
    write_data(1);
    // wait for clear to be finished
    __delay_cycles(2000000);
}


void set_custom_pattern(char pattern[8]) {
    // move to CGRAM adress 0(SET CGRAM ADRESS)
    // this moves up automatically due to auto increment
    P3OUT &= ~(BIT0 + BIT1 + BIT2) ;
    write_data(64);
    // send CGRAM data pattern
    P3OUT |= BIT0;
    int i = 0;
    for(i = 0; i < 8; i++) {
        write_data(pattern[i]);
    }
}



