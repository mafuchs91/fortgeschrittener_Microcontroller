/***************************************************************************//**
 * @file    mma.c
 * @author  Max Fuchs
 *          Matr.: 4340529
 *          Email: maxfuchs@gmx.de
 * @date    22.6
 *
 * @brief   library to implement functionality for the MMA acceleration sensor

 *
 * Here goes a detailed description if required.
 ******************************************************************************/

#include "./mma.h"
#include "LCD.h"

/******************************************************************************
 * VARIABLES
 *****************************************************************************/
// register to set register adress and register value
static unsigned char write_register_byte [2] = {0,0};
// buffer to store the acceleration values from the mma
static unsigned char value_buffer [6] = {0};
// buffer to store single value to read from register of mma
static unsigned char register_value [1] = {0};
// variable to store the register adress
static unsigned char register_address [1] = {0};
// array to store the 8 bit values read from the mma
static signed char xyz_values_8_bit [3] = {0,0,0};
// array to store the 14 bit values read from the mma
static int xyz_values_14_bit [3] = {0,0,0};
// used for calculation of the acc difference in selftest
static int xyz_values_14_bit_temp [3] = {0,0,0};
// keep track for the values of the register of the MMA
static unsigned char CTRL_REG1_value = 0;
// keep track for the values of the register of the mma
static unsigned char XYZ_DATA_CFG_value = 0;

/******************************************************************************
 * LOCAL FUNCTION PROTOTYPES
 *****************************************************************************/

/**
 * checks if bit is set, with k starting from 0....
 * @param k     the bit Number where the input should be checked in P1IN register
 * @param reg   the register in which to check
 *
 * @return 0    if the bit is low
 * @return 1    if the bit is high
 *
 */
int isBitSet(unsigned char reg,unsigned char k);

/**
 * read values from the register address and store in register_value
 */
unsigned char mma_register_read(unsigned char * register_address, unsigned char * register_value, unsigned char length);

/**
 * change the mode of the mma between:
 *
// 0: standby mode
// 1: active mode
 */
unsigned char standby_mode(unsigned char state);

/******************************************************************************
 * LOCAL FUNCTION IMPLEMENTATION
 *****************************************************************************/
int isBitSet(unsigned char reg,unsigned char k) {
    if (reg & (1 << (k))) {    // use bitshift and and operator to check if bit at position k is high
        return 1;
    } else {
        return 0;
    }
}

unsigned char mma_register_read(unsigned char * register_address, unsigned char * register_value, unsigned char length) {
    unsigned char errFlag = 0;
    errFlag |= i2c_write(1, register_address, 0);
    i2c_read(length, register_value);
    return errFlag;
}



/******************************************************************************
 * FUNCTION IMPLEMENTATION
 *****************************************************************************/

unsigned char mma_init(void) {
    unsigned char errFlag = 0;
    i2c_init(29);                               // init i2c for MMA device
    // reset device registers initially
    write_register_byte[0] = CTRL_REG2;
    write_register_byte[1] = 64;
    errFlag |= i2c_write(2, write_register_byte,1);
    // wait until reset is finished
    register_value[0] = 64;
    while(register_value[0] == 64) {
        register_address[0] = CTRL_REG2;
        errFlag |= mma_register_read(register_address, register_value, 1);
    }
    mma_setRange(0);
    mma_setResolution(0);
    return errFlag;
}

unsigned char mma_setRange(unsigned char range) {
    unsigned char errFlag = 0;
    // enter standby mode to change registers
    errFlag |= standby_mode(0);
    if(range == 0) {
        // set values for 2 g
        XYZ_DATA_CFG_value = 0;
        write_register_byte[0] = XYZ_DATA_CFG;
        write_register_byte[1] = XYZ_DATA_CFG_value;
        errFlag |= i2c_write(2, write_register_byte, 1);
    } else if (range == 1){
        // set values for 4g
        XYZ_DATA_CFG_value &= ~FS1;
        XYZ_DATA_CFG_value |= FS0;
        write_register_byte[0] = XYZ_DATA_CFG;
        write_register_byte[1] = XYZ_DATA_CFG_value;
        errFlag |= i2c_write(2, write_register_byte, 1);
    }else {
        // set values for 8g
        XYZ_DATA_CFG_value &= ~FS0;
        XYZ_DATA_CFG_value |= FS1;
        write_register_byte[0] = XYZ_DATA_CFG;
        write_register_byte[1] = XYZ_DATA_CFG_value;
        errFlag |= i2c_write(2, write_register_byte, 1);
    }
    // enter active mode
    errFlag |= standby_mode(1);
    return errFlag;
}

unsigned char mma_setResolution(unsigned char res) {
    unsigned char errFlag = 0;
    errFlag |= standby_mode(0);
    if(res == 0) {
        if(isBitSet(CTRL_REG1_value,1) == 0) {
            CTRL_REG1_value += F_READ;
        }
        write_register_byte[0] = CTRL_REG1;
        write_register_byte[1] = CTRL_REG1_value;
        errFlag = i2c_write(2, write_register_byte, 1);
    } else {
        if(isBitSet(CTRL_REG1_value,1) == 1) {
            CTRL_REG1_value -= F_READ;
        }
        write_register_byte[0] = CTRL_REG1;
        write_register_byte[1] = CTRL_REG1_value;
        errFlag = i2c_write(2, write_register_byte, 1);
    }
    errFlag |= standby_mode(1);
    return errFlag;
}

unsigned char mma_selftest(void) {
    unsigned char errFlag = 0;
    errFlag |= standby_mode(0);              //  go to standby
    errFlag |= mma_setResolution(1);       // 14 bit mode

    // go to lower data rate and low noise mode to increase exactness
    CTRL_REG1_value &= ~DR0;                 // set  data rate to 50Hz
    CTRL_REG1_value &= ~DR1;
    CTRL_REG1_value |= DR2;
    CTRL_REG1_value |= LNOISE;
    write_register_byte[0] = CTRL_REG1;
    write_register_byte[1] = CTRL_REG1_value;
    errFlag |= i2c_write(2, write_register_byte,1);

    unsigned char range = 0;
    //execute the selftest for all 3 ranges
    for(range=0; range <3; range++) {
        errFlag |= mma_setRange(range);            // set range for 2,4, and 8 g
        errFlag |= standby_mode(1);                    //active mode
        __delay_cycles(100000);

        errFlag |= mma_read();
        unsigned char i = 0;
        // set temp variable to compare the difference
        for(i=0;i<3;i++) {
            xyz_values_14_bit_temp[i] = xyz_values_14_bit[i];
        }

        errFlag |= standby_mode(0);                  // standby
    //  activate selftest
        write_register_byte[0] = CTRL_REG2;
        write_register_byte[1] = ST;
        errFlag |= i2c_write(2, write_register_byte,1);

        errFlag |= standby_mode(1);                  //active

        __delay_cycles(10000000);

        errFlag |= mma_read();
        lcd_cursorSet(0, 1);
        // calculate the change in acceleration
        for(i=0;i<3;i++) {
                xyz_values_14_bit_temp[i] -= xyz_values_14_bit[i];
                // decide if the test was sucessfull , didn't know what to do , nothing written in exercise or datasheet.
                // therefore just check if the value has changed at all
                if( xyz_values_14_bit_temp[i] != 0) {
                    errFlag |= 0;
                } else {
                    // indicte self test error
                    errFlag |= 2;
                }
            }
    //  go to standby
        errFlag |= standby_mode(0);

    //  disable self test
        write_register_byte[0] = CTRL_REG2;
        write_register_byte[1] = 0;
        errFlag |= i2c_write(2, write_register_byte,1);
    }

    // reset data rate to 800 Hz
    CTRL_REG1_value &= ~DR0;                 // set  data rate to 50Hz
    CTRL_REG1_value &= ~DR1;
    CTRL_REG1_value &= ~DR2;
    CTRL_REG1_value &= ~LNOISE;
    write_register_byte[0] = CTRL_REG1;
    write_register_byte[1] = CTRL_REG1_value;
    errFlag |= i2c_write(2, write_register_byte,1);
    __delay_cycles(100000);
    // go to active
    errFlag |= standby_mode(1);
    return errFlag;
}
unsigned char mma_read(void) {
    unsigned char errFlag = 0;
    // go into active mode
    errFlag |= standby_mode(1);
    // 8 bit mode
    if(isBitSet(CTRL_REG1_value, 1) == 1) {
        register_address[0] = 1;
        errFlag |= mma_register_read(register_address, xyz_values_8_bit, 3);

    } else {
        // 14 Bit mode
        register_address[0] = 1;
        errFlag |= mma_register_read(register_address, value_buffer, 6);
        unsigned char i = 0;
        unsigned char msb = 0;
        unsigned char lsb = 1;
        // read all axes and store the values as 14 bit in an integer
        for(i = 0; i<3;i++) {
            if(isBitSet(value_buffer[msb], 7) == 0) {
                // positive numbers
                xyz_values_14_bit[i] = (value_buffer[msb]<<6)  | value_buffer[lsb];
            } else {
                // negative numbers
                xyz_values_14_bit[i] = ((value_buffer[msb]<<6)  | value_buffer[lsb]) - 16384 ;
            }
            msb +=2;
            lsb +=2;
        }
    }
    return errFlag;
}

signed char mma_get8X(void) {
    // check if the data was read in 8 bit
    if (isBitSet(CTRL_REG1_value, 1)==1) {
        return xyz_values_8_bit[0];
    }
    else {
        // data was read in 14 bit -> return 14 bit value castet to signed char
        return (xyz_values_14_bit[0]);
    }
}
signed char mma_get8Y(void){
    if (isBitSet(CTRL_REG1_value, 1)==1) {
        return xyz_values_8_bit[1];
    } else {
        return (xyz_values_14_bit[1]);
    }
}

signed char mma_get8Z(void) {
    if (isBitSet(CTRL_REG1_value, 1)==1) {
        return xyz_values_8_bit[2];
    } else {
        return (xyz_values_14_bit[2]);
    }
}

int mma_get14X(void){
    if (isBitSet(CTRL_REG1_value, 1)==1) {
        // if read in 8 bit -> left shift to get 14 bit value
        return xyz_values_8_bit[0]<<6;
    }
    else {
        return (xyz_values_14_bit[0]);
    }
}
int mma_get14Y(void){
    if (isBitSet(CTRL_REG1_value, 1)==1) {
        return xyz_values_8_bit[1]<<6;
    } else {
        return (xyz_values_14_bit[1]);
    }
}
int mma_get14Z(void){
    if (isBitSet(CTRL_REG1_value, 1)==1) {
        return xyz_values_8_bit[2]<<6;
    } else {
        return (xyz_values_14_bit[2]);
    }
}

// the measured value is castet to double and divided by the 2*fullscale to get the acceleration
// value in g. This value is than multiplied by the earth acceleration.
double mma_getRealX(void){
    // check which range was selected
    if((isBitSet(XYZ_DATA_CFG_value, 0)== 0) && (isBitSet(XYZ_DATA_CFG_value, 1)== 0) ) {
        // 2g is selected at data read
        return (((double)mma_get14X()/4096)*9.80665);
    } else if ((isBitSet(XYZ_DATA_CFG_value, 0)== 1) && (isBitSet(XYZ_DATA_CFG_value, 1)== 0) ) {
        // 4g was selected
        return (((double)mma_get14X()/2048)*9.80665);
    } else {
        // 8g was selected
        return (((double)mma_get14X()/1028)*9.80665);
    }
}
double mma_getRealY(void){

    if((isBitSet(XYZ_DATA_CFG_value, 0)== 0) && (isBitSet(XYZ_DATA_CFG_value, 1)== 0) ) {
        // 2g is selected at data read
        return (((double)mma_get14Y()/4096)*9.80665);
    } else if ((isBitSet(XYZ_DATA_CFG_value, 0)== 1) && (isBitSet(XYZ_DATA_CFG_value, 1)== 0) ) {
        return (((double)mma_get14Y()/2048)*9.80665);
    } else {
        return (((double)mma_get14Y()/1028)*9.80665);
    }
}
double mma_getRealZ(void){

    if((isBitSet(XYZ_DATA_CFG_value, 0)== 0) && (isBitSet(XYZ_DATA_CFG_value, 1)== 0) ) {
        // 2g is selected at data read
        return (((double)mma_get14Z()/4096)*9.80665);
    } else if ((isBitSet(XYZ_DATA_CFG_value, 0)== 1) && (isBitSet(XYZ_DATA_CFG_value, 1)== 0) ) {
        return (((double)mma_get14Z()/2048)*9.80665);
//        return mma_get14Y();
    } else {
        return (((double)mma_get14Z()/1028)*9.80665);
    }
}

unsigned char mma_enableTapInterrupt(void) {
    unsigned char errFlag = 0;

    errFlag |= mma_setRange(2);            // 8g range

//  go to standby
    errFlag |= standby_mode(0);

//  set to double pulse event
    write_register_byte[0] = PULSE_CFG;
    write_register_byte[1] = ZDPEFE;
    errFlag |= i2c_write(2, write_register_byte,1);

    // set the threshold
    write_register_byte[0] = PULSE_THSZ;
    write_register_byte[1] = 10;
    errFlag |= i2c_write(2, write_register_byte,1);

    // set the time interval
    write_register_byte[0] = PULSE_TMLT;
    write_register_byte[1] = 160;                // With ODR = 800Hz  for 100ms 100/0.625 = 160
    errFlag |= i2c_write(2, write_register_byte,1);

    // set the time interval
   write_register_byte[0] = PULSE_LTCY;
   write_register_byte[1] = 160;                // With ODR = 800Hz  for 200ms 200/1,25 = 160
   errFlag |= i2c_write(2, write_register_byte,1);

   // set the time interval
    write_register_byte[0] = PULSE_WIND;
    write_register_byte[1] = 72;                // With ODR = 800Hz  for 90ms(shorter than PULSE_TMLT) 90/1,25 = 72
    errFlag |= i2c_write(2, write_register_byte,1);

    //activate interrupt
    write_register_byte[0] = CTRL_REG4;
    write_register_byte[1] = INT_EN_PULSE;
    errFlag |= i2c_write(2, write_register_byte,1);

    // pulse detection
    write_register_byte[0] = CTRL_REG5;
    write_register_byte[1] = INT_CFG_PULSE;
    errFlag |= i2c_write(2, write_register_byte,1);

    // react on high interrupt polarity
    write_register_byte[0] = CTRL_REG3;
    write_register_byte[1] = IPOL;
    errFlag |= i2c_write(2, write_register_byte,1);

    // go to  active mode
    errFlag |= standby_mode(1);

    return errFlag;
}

unsigned char mma_disableTapInterrupt(void) {
    unsigned char errFlag = 0;

    //  go to standby
    errFlag |= standby_mode(0);

//  disable double pulse event
    write_register_byte[0] = PULSE_CFG;
    write_register_byte[1] = 0;
    errFlag |= i2c_write(2, write_register_byte,1);

    // go to  active mode
    errFlag |= standby_mode(1);
    return errFlag;
}


unsigned char standby_mode(unsigned char state) {
    unsigned char errFlag = 0;
    if(state == 0) {
        CTRL_REG1_value &= ~ACTIVE;                 // standby mode
    } else {
        CTRL_REG1_value |= ACTIVE;                  // active mode
    }
    // write the byte to the according register
    write_register_byte[0] = CTRL_REG1;
    write_register_byte[1] = CTRL_REG1_value;
    errFlag |= i2c_write(2, write_register_byte,1);
    return errFlag;
}
