/*
 * systick.c
 *
 *  Created on: Mar 12, 2026
 *      Author: james
 */

#include "motors.h"
#include "systick.h"
#include "main.h"
#define BASE_SPEED 35000
#define kP 150
#define kD 0

int16_t prevError = 0;

void encoder_reset(){
	// If Encoder is close to limit (30k) Reset and keep track of differnece 
	// Take difference between right encoder and left
	int16_t diff = getMotorEnc(right) - getMotorEnc(left);
	// Set right encoder to diff and left to 0 as to keep the same difference before reset
	setMotorEnc(right, diff);
	setMotorEnc(left, 0);
}

void sysTick(){
	// this gets called every time the systick interrupt happens!
	//gyro stuff!
	//updateGyro();
	/*
	if (getMotorEnc(right) > 20000 || getMotorEnc(left) > 20000)
		encoder_reset();
	if (getMotorEnc(right) < -20000 || getMotorEnc(left) < 20000)
				encoder_reset();
	int16_t error = (int16_t) getMotorEnc(right) - (int16_t) getMotorEnc(left);
	int16_t motorOff = error * kP + (error-prevError) * kD;
	setMotorSpeed(right, BASE_SPEED-motorOff, forward);
	setMotorSpeed(left, BASE_SPEED+motorOff, forward);
	prevError = error;
	*/
}
