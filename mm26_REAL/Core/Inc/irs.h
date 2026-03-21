/*
 * irs.h
 */

#ifndef INC_IRS_H_
#define INC_IRS_H_

// The number of samples to take
#define NUM_SAMPLES 128

// Using this enumeration makes the code more readable
#include "main.h"
typedef enum IR {IR_FRONT, IR_LEFT, IR_RIGHT, IR_LEFT_DIAG, IR_RIGHT_DIAG} IR;

uint16_t readIR(IR ir);
uint16_t readLeftIR(void);
uint16_t readFrontIR(void);
uint16_t readRightIR(void);
uint16_t analogRead(IR ir);

#endif /* INC_IRS_H_ */
