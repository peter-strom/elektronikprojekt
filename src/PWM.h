#ifndef PWM_H_
#define PWM_H_
#include "definitions.h"

/************************************************************************
GPIO SLICE CHANNEL
28    6     A
13    6     B
************************************************************************/
typedef struct PWM
{
    // uint8_t GPIO;
    uint8_t slice;
    uint8_t channel;
    uint8_t divider;
    uint16_t wrap;
    int8_t offset;
    int8_t throttle;
    uint16_t limit;

} PWM;

PWM new_PWM(const uint8_t GPIO, uint16_t wrap, int8_t offset, uint16_t limit);
void set_speed(PWM *self, int8_t throttle);
void set_steering_angle(PWM *self, int8_t angle, bool invert);

#endif /* PWM_H_ */