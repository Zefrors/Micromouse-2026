#include "motors.h"

#define SET_MLF(x) TIM1->CCR1 = x
#define SET_MLB(x) TIM1->CCR2 = x
#define SET_MRB(x) TIM1->CCR3 = x
#define SET_MRF(x) TIM1->CCR4 = x

#define GET_MR_ENC() TIM2->CNT
#define GET_ML_ENC() TIM3->CNT

#define INT_16_MAX 100

void setMotorEnc(motor side, int16_t encVal){
	GET_MR_ENC() = encVal;
	GET_ML_ENC() = encVal;

}

int16_t getMotorEnc(motor side){
	switch(side){
		case(right): {return GET_MR_ENC();}
		case(left): {return GET_ML_ENC();}
		break;
	}
	return 0;
}

void setMotorSpeed(motor side, uint16_t speed, direction dir){
	if (side == left)
	{	if (dir == forward)
			{ 
				SET_MLB(0);
				SET_MLF(speed);
				

			}
		if (dir == back)
			{
				SET_MLF(0);
				SET_MLB(speed);
			}
	}
	if (side == right)
		{if (dir == forward)
				{ 	SET_MRB(0);
					SET_MRF(speed);
					
				}
			if (dir == back)
				{
					SET_MRF(0);
					SET_MRB(speed);
				}	
	}
}
