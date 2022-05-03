
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
    uint8_t  min_side_distance;

} SpeedCtrl;

SpeedCtrl new_SpeedCtrl(uint16_t max_input, uint8_t break_distance, uint8_t output_min,
                        uint8_t output_mid, uint8_t output_mid_percent, uint8_t output_max,
                         uint8_t  min_side_distance);
uint8_t SpeedCtrl_calc_speed(SpeedCtrl *self, uint16_t sensor_input, uint16_t left_sensor_input, uint16_t right_sensor_input);

#endif /* SPEEDCTRL_H_ */
