#include "SpeedCtrl.h"
/**
 * @brief Initialize an instance of a SpeedCtrl-structure.
 * 
 * @param max_input max value of sensor
 * @param break_distance stop distance
 * @param output_min min output value
 * @param output_mid mid output value
 * @param output_mid_percent where to implement mid value over max value
 * @param output_max max output value
 * @return SpeedCtrl Instance of a SpeedCtrl-structure
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

/**
 * @brief Takes input value from from sensor and calculates a safe speed.
 * 
 * @param self Pointer address to the SpeedCtrl-instance.
 * @param sensor_input sensor value 
 * @return uint8_t 
 */
uint8_t SpeedCtrl_calc_speed(SpeedCtrl *self, int16_t sensor_input)
{
    uint8_t output_span;
    if (sensor_input > self->max_input)
    {
        sensor_input = self->max_input;
    }
    if (sensor_input > self->break_distance)
    {
        float input_percent = (float)sensor_input / self->max_input;
        if (input_percent <= self->output_mid_percent)
        {
            output_span = self->output_mid - self->output_min;
        }
        else
        {
            output_span = self->output_max - self->output_min;
        }
        return (uint8_t)((output_span * input_percent) + self->output_min);
    }
    else
    {
        return 0;
    }
}
