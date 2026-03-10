#include "motors.h"

#define SET_MLF(x) TIM1->CCR1 = x
#define SET_MLB(x) TIM1->CCR2 = x
#define SET_MRB(x) TIM1->CCR3 = x
#define SET_MRF(x) TIM1->CCR4 = x




void setMotorSpeed(motor side, uint16_t speed, direction dir){
	/* CCR2 => FORWARD */
	SET_MLB(0);
	SET_MLF(30000);
	SET_MRB(0);
	SET_MRF(30000);
}
