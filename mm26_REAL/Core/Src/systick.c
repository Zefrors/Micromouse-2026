/*
 * systick.c
 *
 *  Created on: Mar 12, 2026
 *      Author: james
 */


#include "systick.h"
#include "motors.h"
#define BASE_SPEED 35000
#define kP 1000

void sysTick(){
	// this gets called every time the systick interrupt happens!
	// TODO: reset counts if too big

	int16_t error = (int16_t) getMotorEnc(right) - (int16_t) getMotorEnc(left);
	int16_t motorOff = error * kP;
	setMotorSpeed(right, BASE_SPEED-motorOff, forward);
	setMotorSpeed(left, BASE_SPEED+motorOff, forward);

}
