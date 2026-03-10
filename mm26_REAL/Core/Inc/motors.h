#include "main.h"

typedef enum motor {left, right} motor;
typedef enum direction {forward, back} direction;

void setMotorSpeed(motor side, uint16_t speed, direction dir);
uint16_t getMotorEnc(motor side);
