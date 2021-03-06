/***************************************************************************//**
 * @file    mma.h
 * @author  Max Fuchs
 *          Matr.: 4340529
 *          Email: maxfuchs@gmx.de
 * @date    22.6
 *
 *
 * @brief   library to implement functionality for the MMA acceleration sensor
 *
 *
 *
 * @ note   The X/Y-direction printed on the board are wrong:
//          X is the real Y and Y is the real X. Sorry for the confusion. :-(
 ******************************************************************************/

#ifndef EXERCISE_3_LIBS_MMA_H_
#define EXERCISE_3_LIBS_MMA_H_

/******************************************************************************
 * INCLUDES
 *****************************************************************************/

#include "./i2c.h"

/******************************************************************************
 * CONSTANTS
 *****************************************************************************/
#define slaveAdress 29;

#define CTRL_REG1 42;
#define F_READ 2;       // CTRL_REG1 bit for 8 bit mode
#define ACTIVE 1;
#define DR0 8;          // control of data rate
#define DR1 16;         // control of data rate
#define DR2 32;         // control of data rate
#define LNOISE 4;         // reduce noise

#define XYZ_DATA_CFG 14;
#define FS1 2;       // XYZ_DATA_CFG bit for measurement range
#define FS0 1;       // XYZ_DATA_CFG bit for measurement range

#define CTRL_REG2 43;
#define ST 128;       // CTRL_REG1 bit for 8 bit mode

#define PULSE_CFG 33;
#define ZDPEFE 32;      // Event flag enable on double pulse event on Z-axis
#define DPA 128;

#define PULSE_THSZ 37;  // Threshold of Z axis

#define PULSE_TMLT 38;  // define the maximum time interval that can elapse between the start of the acceleration on the
                        //    selected axis exceeding the specified threshold and the end when the acceleration

#define PULSE_LTCY 39;  // time interval after f�rst pulse detection, during this interval all pulses are ignored


#define PULSE_WIND 40;  // defines the time window for the second tap

#define CTRL_REG3 44;
#define IPOL 2;         // set interrupt polarity

#define CTRL_REG4 45;
#define INT_EN_PULSE 8; // enable PUlse detection interrupt

#define CTRL_REG5 46;
#define INT_CFG_PULSE 8; // activate the interrupt on INT1 for PUlse
/******************************************************************************
 * VARIABLES
 *****************************************************************************/



/******************************************************************************
 * FUNCTION PROTOTYPES
 *****************************************************************************/

// All configuration functions return 0 if everything went fine
// and anything but 0 if not (they are the ones with a unsigned char return type).

// Initialize the MMA with 8 bit resolution and 4G measurement range (1 pt.)
unsigned char mma_init(void);

// Change the measurement range. (0: 2g, 1: 4g, >1: 8g) (0.5 pt.)
unsigned char mma_setRange(unsigned char range);
// Change the resolution (0: 8 Bit, >= 1: 14 Bit) (0.5 pt.)
unsigned char mma_setResolution(unsigned char resolution);

// Run a self-test on the MMA, verifying that all three axis and all three
// measurement ranges are working. (1 pt.)
/* HINT:
 * The idea of the self test is that you measure the current acceleration values,
 * then enable the on-chip self-test and then read the values again.
 * The values without selftest enabled and those with selftest enabled
 * should now feature a predefined difference (see the datasheet).
 */
unsigned char mma_selftest(void);


// Set up the double tap interrupt on the MMA (do not set up the interrupt on
// the MSP in this function!). This means that the MMA should change the INT1-
// pin whenever a double tap is detected. You may freely choose the axis on
// which the tap has to be received. (You should put a comment in your code,
// which axis you chose, though). (1 pt.)
/* HINT:
 * As the datasheet for the MMA is a bit stingy when it comes to the double
 * tap stuff, so here's (roughly) what you should do:
 *
 *  1) Go to standby (as you can only change the registers when in standby)
 *  2) Write MMA_PULSE_CFG  to enable the z-axis for double tap
 *  3) Write MMA_PULSE_THSZ to set the tap threshold (e.g. to 2g)
 *  4) Write MMA_PULSE_TMLT to set the pulse time limit (e.g. to 100 ms)
 *  5) Write MMA_PULSE_LTCY to set the pulse latency timer (e.g. to 200 ms)
 *  6) Write MMA_PULSE_WIND to set the time window for the second tap
 *  7) Write MMA_CTRL_REG4  to set the pulse interrupt
 *  8) Write MMA_CTRL_REG5  to activate the interrupt on INT1
 *  9) Write MMA_CTRL_REG3  to set the interrupt polarity
 * 10) Return to active mode
 */
unsigned char mma_enableTapInterrupt(void);
// Disable the double-tap-interrupt on the MMA. (0.5 pt.)
unsigned char mma_disableTapInterrupt(void);


// Read the values of all three axis from the chip and store the values
// internally. Take the requested resolution into account. (1 pt.)
unsigned char mma_read(void);


/* Get Functions (1 pt. total): */

// Return the appropriate 8 bit values
// If the resolution during mma_read was 14 bit, translate the data to 8 bit
signed char mma_get8X(void);
signed char mma_get8Y(void);
signed char mma_get8Z(void);

// Return the appropriate 14 bit values
// If the resolution during mma_read was 8 bit, translate the data to 14 bit
int mma_get14X(void);
int mma_get14Y(void);
int mma_get14Z(void);

// Return the appropriate values in m*s^-2.
double mma_getRealX(void);
double mma_getRealY(void);
double mma_getRealZ(void);

#endif /* EXERCISE_3_LIBS_MMA_H_ */
