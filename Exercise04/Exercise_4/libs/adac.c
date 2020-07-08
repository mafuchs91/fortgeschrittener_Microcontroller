/***************************************************************************//**
 * @file    adac.c
 * @author  Max Fuchs
 *          Matr.: 4340529
 *          Email: maxfuchs@gmx.de
 * @date    8.7.2020
 *
 * @SheetNr 4
 *
 * @brief   implements basic functions for the ad and da conversion  for the PCF8591
 * via i2C.
 *
 ******************************************************************************/

#include "./adac.h"

/******************************************************************************
 * VARIABLES
 *****************************************************************************/
// sets the control bit of PCF8591 to auto increment channel read
unsigned char controlBitAutoIncrement [1] = {68};
unsigned char controlBitsAnalogOutput [2] = {64, 255};

unsigned char acknoledgeFlag = 0;

/******************************************************************************
 * LOCAL FUNCTION PROTOTYPES
 *****************************************************************************/



/******************************************************************************
 * LOCAL FUNCTION IMPLEMENTATION
 *****************************************************************************/



/******************************************************************************
 * FUNCTION IMPLEMENTATION
 *****************************************************************************/
unsigned char adac_init(void) {
    // initialize i2c connection to the device adress of PCF8591
    i2c_init(72);
    // the initialisation is in this type of implementation always valid
    return 0;
}


unsigned char adac_read(unsigned char * values) {
    acknoledgeFlag = i2c_write(1, controlBitAutoIncrement, 1);
    // read the values for the channesl 0 - 3 and store in values
    i2c_read(4, values);
    return acknoledgeFlag;
}

unsigned char adac_write(unsigned char value) {
    controlBitsAnalogOutput[1] = value;
    acknoledgeFlag = i2c_write(2, controlBitsAnalogOutput, 1);
    return acknoledgeFlag;
}

