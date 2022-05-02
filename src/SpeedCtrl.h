
#ifndef SPEEDCTRL_H_
#define SPEEDCTRL_H_
#include "definitions.h"
typedef struct SpeedCtrl
{
    uint16_t max_input;
    uint8_t break_distance;
    uint8_t output_min;
    uint8_t output_mid;
    uint8_t output_mid_percent;
    uint8_t output_max;
    uint8_t output_span;

} SpeedCtrl;

SpeedCtrl new_SpeedCtrl(uint16_t max_input, uint8_t break_distance, uint8_t output_min,
                        uint8_t output_mid, uint8_t output_mid_percent, uint8_t output_max);
uint8_t SpeedCtrl_calc_speed(SpeedCtrl *self, int16_t sensor_input);

#endif /* SPEEDCTRL_H_ */
