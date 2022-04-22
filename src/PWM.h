#ifndef PWM_H_
#define PWM_H_
#include "header.h"

/************************************************************************
GPIO SLICE CHANNEL
28    6     A
13    6     B 
************************************************************************/
typedef struct PWM
{
	//uint8_t GPIO;
    uint8_t slice;
    uint8_t channel;
	uint8_t divider;
    uint16_t wrap;
	int16_t offset;
    int8_t throttle;
	uint16_t limit;

} PWM;

PWM new_PWM(const uint8_t GPIO);
void set_speed(PWM* self, int8_t throttle);

#endif /* PWM_H_ */