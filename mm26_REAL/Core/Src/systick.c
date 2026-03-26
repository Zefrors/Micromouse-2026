/*
 * systick.c
 *
 *  Created on: Mar 12, 2026
 *      Author: james
 */

#include "motors.h"
#include "systick.h"
#include "main.h"
#define BASE_SPEED 2200
#define kP 332 // 215
#define kD 0
#define kI .12 // 1
#define MAX_SPEED 3199
#define CLAMP(x) ((x > MAX_SPEED) ? MAX_SPEED : x)
#define ABS(x) ( (x) < 0 ? (-(x)) : (x))
#define ERROR_SIZE 1000
#define DIST_ERROR_SIZE 50
#define distkP 25
#define distkI 0.01

#define ERROR_THRESH 1500

int16_t prevError = 0;
int16_t errorSum;
float errors[ERROR_SIZE];
float distErrors[DIST_ERROR_SIZE];
int16_t errorPtr;
int16_t distErrorPtr;
int16_t distErrorTot;
int16_t distError;
int16_t startDist;
float dest;
int16_t targetDist;
float Aerror;

int piDone;

void encoder_reset(){
	// If Encoder is close to limit (30k) Reset and keep track of differnece 
	// Take difference between right encoder and left
	targetDist -= (getMotorEnc(right)/2) + (getMotorEnc(left)/2);
	int16_t diff = getMotorEnc(right) - getMotorEnc(left);
	// Set right encoder to diff and left to 0 as to keep the same difference before reset
	setMotorEnc(right, diff);
	setMotorEnc(left, 0);
}

void encoder_reset_straight(){
	// keep distance but nothing else
	int16_t avg = (getMotorEnc(right) + getMotorEnc(left)) / 2;
	setMotorEnc(right, avg);
	setMotorEnc(left, avg);
}


void moveOne(float angle){
	setPIDGoals(angle, (getMotorEnc(right) + getMotorEnc(left))/2 + 485);
	Aerror = 100;
	while ( !piDone) {}
	//encoder_reset();
}

void setPIDGoals(float angle, int16_t dist){
	dest = angle;
	targetDist = dist;
	piDone = 0;
	distErrorTot = 0;
	errorSum = 0;

	for (int i = 0; i < ERROR_SIZE; i++){
		errors[i] = 0;
	}
	for (int i = 0; i < DIST_ERROR_SIZE; i++){
		distErrors[i] = 0;
	}
}

void center(float a){
	setPIDGoals(a, (getMotorEnc(right) + getMotorEnc(left))/2 + 100);
	while ( !piDone) {}
	//encoder_reset();
}

void turnRight(float a){
	setPIDGoals(a, (getMotorEnc(right) + getMotorEnc(left))/2 + 45);
	while ( !piDone) {}
	HAL_Delay(1000);


	setPIDGoals(a + 90, (getMotorEnc(right) + getMotorEnc(left))/2);
	while ( !piDone) {}
	HAL_Delay(1000);

	setPIDGoals(a + 90, (getMotorEnc(right) + getMotorEnc(left))/2 - 45);
	while ( !piDone) {}


	//encoder_reset();
}

void turnLeft(float a){
	setPIDGoals(a, (getMotorEnc(right) + getMotorEnc(left))/2 + 45);
	while ( !piDone) {}
	HAL_Delay(1000);


	setPIDGoals(a - 90, (getMotorEnc(right) + getMotorEnc(left))/2);
	while ( !piDone) {}
	HAL_Delay(1000);

	setPIDGoals(a - 90, (getMotorEnc(right) + getMotorEnc(left))/2 - 45);
	while ( !piDone) {}
	//encoder_reset();
}


void pid(float angle){
	if (getMotorEnc(right) > 20000 || getMotorEnc(left) > 20000)
		encoder_reset();
	if (getMotorEnc(right) < -20000 || getMotorEnc(left) < -20000)
		encoder_reset();

	Aerror = dest - angle;
	if (Aerror > 180)
		Aerror -= 360;
	else if (Aerror < -180)
		Aerror += 360;
	/*
	if (angle < 180)
		error = angle - dest;
	if (angle >= 180)
		error = angle - 360;
	 */
	distError = targetDist - ((getMotorEnc(right) + getMotorEnc(left)) / 2);


	int16_t distMotor = (distError * distkP) + distErrorTot * distkI;
	if (distMotor > 2500){
		distMotor = 2500;
	}
	else if (distMotor < -2500){
		distMotor = -2500;
	}
	distErrorTot -= distErrors[distErrorPtr];
	distErrors[distErrorPtr] =  10 * distError;
	distErrorTot += 10 * distError;
	distErrorPtr++;
	distErrorPtr = (distErrorPtr % DIST_ERROR_SIZE);
	// FOR TESTING ONLY
	//if (targetDist == 0)
	//	distMotor = BASE_SPEED;

	int16_t motorOff = (int16_t) (Aerror * kP) + (Aerror-prevError) * kD + errorSum * kI;

	if ((ABS(distMotor-motorOff) < ERROR_THRESH) && (ABS(distMotor+motorOff) < ERROR_THRESH))
		piDone = 1;
	else
		piDone = 0;

	if (!piDone){
		if (distMotor-motorOff > 0){
			setMotorSpeed(right, CLAMP(distMotor-motorOff), forward);
		}
		else{
			setMotorSpeed(right, CLAMP(-(distMotor-motorOff)), back);
		}
		if (distMotor+motorOff > 0){
			setMotorSpeed(left, CLAMP(distMotor+motorOff), forward);
		}
		else{
			setMotorSpeed(left, CLAMP(-(distMotor+motorOff)), back);
		}
	}
	else {
		setMotorSpeed(right, 0, forward);
		setMotorSpeed(left, 0, forward);
	}
	prevError = Aerror;
	updateError(distMotor);
	errorSum -= errors[errorPtr];
	errors[errorPtr] =  10 * Aerror;
	errorSum += 10 * Aerror;
	errorPtr++;
	errorPtr = (errorPtr % ERROR_SIZE);
}

void sysTick(){
	// this gets called every time the systick interrupt happens!
	//gyro stuff
	//updateGyro();

	/*
	if (getMotorEnc(right) > 20000 || getMotorEnc(left) > 20000)
		encoder_reset();
	if (getMotorEnc(right) < -20000 || getMotorEnc(left) < -20000)
		encoder_reset();


	int16_t error = (int16_t) getMotorEnc(right) - (int16_t) getMotorEnc(left);
	int16_t motorOff = (int16_t) (error * kP) + (error-prevError) * kD + errorSum * kI;
	if (BASE_SPEED-motorOff > 0){
		setMotorSpeed(right, CLAMP(BASE_SPEED-motorOff), forward);
	}
	else
		setMotorSpeed(right, CLAMP(-(BASE_SPEED-motorOff)), back);
	if (BASE_SPEED+motorOff > 0)
		setMotorSpeed(left, CLAMP(BASE_SPEED+motorOff), forward);
	else
		setMotorSpeed(left, CLAMP(-(BASE_SPEED+motorOff)), back);
	prevError = error;
	updateError(errorSum);
	errorSum += error;
	*/
}
