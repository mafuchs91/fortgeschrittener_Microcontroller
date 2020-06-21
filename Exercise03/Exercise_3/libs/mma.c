/***************************************************************************//**
 * @file    mma.c
 * @author  <your name>
 * @date    <date of creation>
 *
 * @brief   <brief description>
 *
 * Here goes a detailed description if required.
 ******************************************************************************/

#include "./mma.h"

/******************************************************************************
 * VARIABLES
 *****************************************************************************/

unsigned char write_register_byte [2] = {0,0};
unsigned char value_buffer [6] = {0};
unsigned char register_value [1] = {0};
unsigned char register_address [1] = {0};
signed char xyz_values_8_bit [3] = {0,0,0};
int xyz_values_14_bit [3] = {0,0,0};
int xyz_values_14_bit_temp [3] = {0,0,0};
// keep track for the values of the register of the MMA
unsigned char CTRL_REG1_value = 0;
unsigned char XYZ_DATA_CFG_value = 0;

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

void mma_register_read(unsigned char * register_address, unsigned char * register_value, unsigned char length);

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

void mma_register_read(unsigned char * register_address, unsigned char * register_value, unsigned char length) {
    i2c_write(1, register_address, 0);
    i2c_read(length, register_value);
}



//void mma_write_register_bit(unsigned char register_adress unsigned char bit_value) {
//    register_addr[0] = register_adress;
//    mma_register_read(register_addr, register_val);
//    write_register_bit[0] = register_adress;
//    write_register_bit[1] = register_val[0] + bit;
//    i2c_write(2, write_register_bit, 1);
//}

/******************************************************************************
 * FUNCTION IMPLEMENTATION
 *****************************************************************************/

unsigned char mma_init(void) {
    i2c_init(29);                               // init i2c for MMA device
    // reset device registers initially
    write_register_byte[0] = CTRL_REG2;
    write_register_byte[1] = 64;
    i2c_write(2, write_register_byte,1);
    // wait until reset is finished
    register_value[0] = 64;
    while(register_value[0] == 64) {
        register_address[0] = CTRL_REG2;
        mma_register_read(register_address, register_value, 1);
    }

    mma_setRange(0);
    mma_setResolution(0);
}

unsigned char mma_setRange(unsigned char range) {
    standby_mode(0);
    if(range == 0) {
        XYZ_DATA_CFG_value = 0;
        write_register_byte[0] = XYZ_DATA_CFG;
        write_register_byte[1] = XYZ_DATA_CFG_value;
        i2c_write(2, write_register_byte, 1);
    } else if (range == 1){
        XYZ_DATA_CFG_value &= ~FS1;
        XYZ_DATA_CFG_value |= FS0;
        write_register_byte[0] = XYZ_DATA_CFG;
        write_register_byte[1] = XYZ_DATA_CFG_value;
        i2c_write(2, write_register_byte, 1);
    }else {
        XYZ_DATA_CFG_value &= ~FS0;
        XYZ_DATA_CFG_value |= FS1;
        write_register_byte[0] = XYZ_DATA_CFG;
        write_register_byte[1] = XYZ_DATA_CFG_value;
        i2c_write(2, write_register_byte, 1);
    }
    standby_mode(1);
}

unsigned char mma_setResolution(unsigned char res) {
    standby_mode(0);
    if(res == 0) {
        if(isBitSet(CTRL_REG1_value,1) == 0) {
            CTRL_REG1_value += F_READ;
        }
        write_register_byte[0] = CTRL_REG1;
        write_register_byte[1] = CTRL_REG1_value;
        i2c_write(2, write_register_byte, 1);
    } else {
        if(isBitSet(CTRL_REG1_value,1) == 1) {
            CTRL_REG1_value -= F_READ;
        }
        write_register_byte[0] = CTRL_REG1;
        write_register_byte[1] = CTRL_REG1_value;
        i2c_write(2, write_register_byte, 1);
    }
    standby_mode(1);
}

unsigned char mma_selftest(void) {

    mma_setRange(1);            // 4g range
    mma_setResolution(1);       // 14 bit mode
    // go into active mode
    CTRL_REG1_value += ACTIVE;
    write_register_byte[0] = CTRL_REG1;
    write_register_byte[1] = CTRL_REG1_value;
    i2c_write(2, write_register_byte, 1);
    // example for 8 bit read
    __delay_cycles(100000);


    mma_read();
    unsigned char i = 0;
    for(i=0;i<3;i++) {
        xyz_values_14_bit_temp[i] = xyz_values_14_bit[i];
        lcd_putNumber(xyz_values_14_bit[i]);
        lcd_putText(" ");
    }
//  go to standby
    standby_mode(0);

//  deactivateselftest
    write_register_byte[0] = CTRL_REG2;
    write_register_byte[1] = ST;
    i2c_write(2, write_register_byte,1);
// go to active
    standby_mode(1);

    __delay_cycles(10000000);

    mma_read();
    lcd_cursorSet(0, 1);
    for(i=0;i<3;i++) {
            xyz_values_14_bit_temp[i] -= xyz_values_14_bit[i];
            lcd_putNumber(xyz_values_14_bit_temp[i]);
            lcd_putText(" ");
        }
//  go to standby
    standby_mode(0);

//  set selftest by giving impulse
    write_register_byte[0] = CTRL_REG2;
    write_register_byte[1] = 0;
    i2c_write(2, write_register_byte,1);
// go to active
    standby_mode(1);
}
unsigned char mma_read(void) {
    // go into active mode
    standby_mode(1);
    // 8 bit mode
    if(isBitSet(CTRL_REG1_value, 1) == 1) {
        register_address[0] = 1;
        mma_register_read(register_address, xyz_values_8_bit, 3);

    } else {
        // 14 Bit mode
        register_address[0] = 1;
        mma_register_read(register_address, value_buffer, 6);

        unsigned char i = 0;
        unsigned char msb = 0;
        unsigned char lsb = 1;
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
    return 1;
}

signed char mma_get8X(void) {
    if (isBitSet(CTRL_REG1_value, 1)==1) {
        return xyz_values_8_bit[0];
    }
    else {
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

double mma_getRealX(void){

    if((isBitSet(XYZ_DATA_CFG_value, 0)== 0) && (isBitSet(XYZ_DATA_CFG_value, 1)== 0) ) {
        // 2g is selected at data read
        return (((double)mma_get14X()/4096)*9.80665);
    } else if ((isBitSet(XYZ_DATA_CFG_value, 0)== 1) && (isBitSet(XYZ_DATA_CFG_value, 1)== 0) ) {
        return (((double)mma_get14X()/2048)*9.80665);
    } else {
        return (((double)mma_get14X()/1028)*9.80665);
    }
}
double mma_getRealY(void){

    if((isBitSet(XYZ_DATA_CFG_value, 0)== 0) && (isBitSet(XYZ_DATA_CFG_value, 1)== 0) ) {
        // 2g is selected at data read
        return (((double)mma_get14Y()/4096)*9.80665);
    } else if ((isBitSet(XYZ_DATA_CFG_value, 0)== 1) && (isBitSet(XYZ_DATA_CFG_value, 1)== 0) ) {
        return (((double)mma_get14Y()/2048)*9.80665);
//        return mma_get14Y();
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
    mma_setRange(2);            // 8g range

//  go to standby
    standby_mode(0);

//  set to double pulse event
    write_register_byte[0] = PULSE_CFG;
    write_register_byte[1] = ZDPEFE;
    i2c_write(2, write_register_byte,1);

    // set the threshold
    write_register_byte[0] = PULSE_THSZ;
    write_register_byte[1] = 10;                //
    i2c_write(2, write_register_byte,1);

    // set the time interval
    write_register_byte[0] = PULSE_TMLT;
    write_register_byte[1] = 160;                // With ODR = 800Hz  for 100ms 100/0.625 = 160
    i2c_write(2, write_register_byte,1);

    // set the time interval
   write_register_byte[0] = PULSE_LTCY;
   write_register_byte[1] = 160;                // With ODR = 800Hz  for 200ms 200/1,25 = 160
   i2c_write(2, write_register_byte,1);

   // set the time interval
    write_register_byte[0] = PULSE_WIND;
    write_register_byte[1] = 72;                // With ODR = 800Hz  for 90ms(shorter than PULSE_TMLT) 90/1,25 = 72
    i2c_write(2, write_register_byte,1);

    //activate interrupt
    write_register_byte[0] = CTRL_REG4;
    write_register_byte[1] = INT_EN_PULSE;
    i2c_write(2, write_register_byte,1);

    // pulse detection
    write_register_byte[0] = CTRL_REG5;
    write_register_byte[1] = INT_CFG_PULSE;
    i2c_write(2, write_register_byte,1);

    // react on high interrupt polarity
    write_register_byte[0] = CTRL_REG3;
    write_register_byte[1] = IPOL;
    i2c_write(2, write_register_byte,1);

    // go to  active mode
    standby_mode(1);


}
// 0: standby mode
// 1: active mode
void standby_mode(unsigned char state) {
    if(state == 0) {
        CTRL_REG1_value &= ~ACTIVE;                 // unset standby bit
    } else {
        CTRL_REG1_value |= ACTIVE;                  // set standby bit
    }
        write_register_byte[0] = CTRL_REG1;
        write_register_byte[1] = CTRL_REG1_value;
        i2c_write(2, write_register_byte,1);
}
