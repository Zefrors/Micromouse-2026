/*
 * systick.h
 *
 *  Created on: Mar 12, 2026
 *      Author: james
 */

#ifndef INC_SYSTICK_H_
#define INC_SYSTICK_H_


void sysTick(void);
void pid(float a);
void setPIDGoals(float angle, int16_t dist);
void moveOne(float a);
void center(float a);
void turnRight(float a);
void turnLeft(float a);

#endif /* INC_SYSTICK_H_ */
