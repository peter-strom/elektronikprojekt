#include "SpeedCtrl.h"
/**
 * @brief
 *
 * @param max_input
 * @param break_distance
 * @param output_min
 * @param output_max
 * @return SpeedCtrl
 */
SpeedCtrl new_SpeedCtrl(uint16_t max_input, uint8_t break_distance, uint8_t output_min,
                        uint8_t output_mid, uint8_t output_mid_percent, uint8_t output_max)
{
    SpeedCtrl self;
    self.max_input = max_input;
    self.break_distance = break_distance;
    self.output_min = output_min;
    self.output_mid = output_mid;
    self.output_mid_percent = output_mid_percent;
    self.output_max = output_max;
    return self;
}

uint8_t SpeedCtrl_calc_speed(SpeedCtrl *self, int16_t sensor_input)
{
    if (sensor_input > self->max_input)
    {
        sensor_input = self->max_input;
    }
    if (sensor_input > self->break_distance)
    {
        float input_percent = (float)sensor_input / self->max_input;
        if (input_percent <= self->output_mid_percent)
        {
            uint8_t output_span = self->output_mid - self->output_min;
        }
        else
        {
            uint8_t output_span = self->output_max - self->output_min;
        }
        uint8_t output_span = self->output_max - self->output_min;
        return (uint8_t)((output_span * input_percent) + self->output_min);
    }
    else
    {
        return 0;
    }
}
