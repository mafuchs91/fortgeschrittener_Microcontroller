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
    write_register_byte[0] = CTRL_REG2;
    write_register_byte[1] = 64;
    i2c_write(2, write_register_byte,1);
    mma_setRange(0);
    mma_setResolution(0);
}

unsigned char mma_setRange(unsigned char range) {
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
}

unsigned char mma_setResolution(unsigned char res) {
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
//        xyz_values_14_bit_temp[i] = xyz_values_14_bit[i];
        lcd_putNumber(xyz_values_14_bit[i]);
        lcd_putText(" ");
    }

    CTRL_REG1_value -= ACTIVE;
    write_register_byte[0] = CTRL_REG1;
    write_register_byte[1] = CTRL_REG1_value;
    i2c_write(2, write_register_byte,1);
//    __delay_cycles(100);
//  set selftest by giving impulse
    write_register_byte[0] = CTRL_REG2;
    write_register_byte[1] = ST;
    i2c_write(2, write_register_byte,1);

    CTRL_REG1_value += ACTIVE;
    write_register_byte[0] = CTRL_REG1;
    write_register_byte[1] = CTRL_REG1_value;
    i2c_write(2, write_register_byte,1);

//    __delay_cycles(80);

    mma_read();
    lcd_cursorSet(0, 1);
//    lcd_putNumber(xyz_values_14_bit[0] );
//    lcd_putText(" ");
//    lcd_putNumber(xyz_values_14_bit[1]);
//    lcd_putText(" ");
//    lcd_putNumber(xyz_values_14_bit[2]);
    for(i=0;i<3;i++) {
            xyz_values_14_bit_temp[i] -= xyz_values_14_bit[i];
            lcd_putNumber(xyz_values_14_bit_temp[i]);
            lcd_putText(" ");
        }
//    lcd_putNumber(xyz_values_14_bit[0]-xyz_values_14_bit_temp[0] );
//    lcd_putText(" ");
//    lcd_putNumber(xyz_values_14_bit[1]-xyz_values_14_bit_temp[1]);
//    lcd_putText(" ");
//    lcd_putNumber(xyz_values_14_bit[2]-xyz_values_14_bit_temp[2]);

}
unsigned char mma_read(void) {
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
}

signed char mma_get8X(void) {
    if (isBitSet(CTRL_REG1_value, 1)==1) {
        return xyz_values_8_bit[0];
    }
    else {
        return xyz_values_14_bit[0];
    }
}
signed char mma_get8Y(void){
    if (isBitSet(CTRL_REG1_value, 1)==1) {
        return xyz_values_8_bit[1];
    } else {

    }
}

signed char mma_get8Z(void) {
    if (isBitSet(CTRL_REG1_value, 1)==1) {
        return xyz_values_8_bit[2];
    } else {

    }
}

