/***************************************************************************//**
 * @file    LCD.h
 * @author  Max Fuchs
 *          Matr.: 4340529
 *          Email: maxfuchs@gmx.de
 * @date    23.5.2020
 *
 * @brief   Exercise 1 - Display Interface
 *
 * Defines the prototypes and Constants for an Interface to the HD44780-based LCD display.
 ******************************************************************************/

#ifndef LIBS_LCD_H_
#define LIBS_LCD_H_

/******************************************************************************
 * INCLUDES
 *****************************************************************************/

#include <msp430g2553.h>
#include <stdint.h>
/******************************************************************************
 * CONSTANTS
 *****************************************************************************/
#define setTo4PinMode 0x02;
#define to_zero 0x00;
// LCD control
#define D 0x04;
#define Cursor 0x02;
#define B 0x01;

// LCD function
#define Number_display_lines 0x08;

// P3OUT(RS, R/W, E) p. 8 pin functions
#define data_register_write 1;
#define instruction_register_write 0;
#define data_register_read 3;
#define instruction_register_read 2;


/******************************************************************************
 * VARIABLES
 *****************************************************************************/
// variables for the current state of the Control and Function register of the display
uint8_t _LCDContr;
uint8_t _LCDFunc;


/******************************************************************************
 * FUNCTION PROTOTYPES
 *****************************************************************************/

/** Initialization */

// Initialization of the LCD; set all pin directions,
// basic setup of the LCD, etc. (1 pt.)
void lcd_init (void);

/** Control functions */

// Enable (1) or disable (0) the display (i.e. hide all text) (0.5 pts.)
void lcd_enable (unsigned char on);

// Set the cursor to a certain x/y-position (0.5 pts.)
void lcd_cursorSet (unsigned char x, unsigned char y);

// Show (1) or hide (0) the cursor (0.5 pts.)
void lcd_cursorShow (unsigned char on);

// Blink (1) or don't blink (0) the cursor (0.5 pts.)
void lcd_cursorBlink (unsigned char on);


/** Data manipulation */

// Delete everything on the LCD (1 pt.)
void lcd_clear (void);

// Put a single character on the display at the cursor's current position (1 pt.)
void lcd_putChar (char character);

// Show a given string on the display. If the text is too long to display,
// don't show the rest (i.e. don't break into the next line) (1 pt.).
void lcd_putText (char * text);

// Show a given number at the cursor's current position.
// Note that this is a signed variable! (1 pt.)
void lcd_putNumber (int number);

// Bonus exercise implementation
void set_custom_pattern(char pattern[8]);


#endif /* LIBS_LCD_H_ */
