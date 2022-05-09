#include "SpeedCtrl.h"

uint16_t sanitize_sensor_input(SpeedCtrl *self, uint16_t sensor_input);

/**
 * @brief Initialize an instance of a SpeedCtrl-structure.
 *
 * @param max_input max value of sensor input
 * @param break_distance stop distance
 * @param output_min min output value
 * @param output_mid mid output value
 * @param mid_range_limit (0-100) where to apply output_mid as highest output value
 * @param output_max max output value
 * @param min_side_distance distance from side sensors where to slow down
 * @param reverse_output negative value between -100 and 0 representing reverse speed
 * @return SpeedCtrl Instance of a SpeedCtrl-structure
 */
SpeedCtrl new_SpeedCtrl(uint16_t max_input, uint8_t break_distance, uint8_t output_min,
                        uint8_t output_mid, uint8_t mid_range_limit, uint8_t output_max,
                        uint8_t min_side_distance, int8_t reverse_output)
{
    SpeedCtrl self;
    self.max_input = max_input;
    self.break_distance = break_distance;
    self.output_min = output_min;
    self.output_mid = output_mid;
    self.mid_range_limit = mid_range_limit;
    self.output_max = output_max;
    self.min_side_distance = min_side_distance;
    self.reverese_output = reverse_output;
    return self;
}

/**
 * @brief Takes input value from from sensor and calculates a safe speed.
 * @details
 * if any sensor values is lower than break_distance => reverse_output (remember gear protector in PWM.c:set_speed)
 * if any side_sensors values is lower than min_side_distance => slow down
 * if front sensor value is higher than mid_range_value => apply output_max as highest available speed
 * if front sensor value is lower than mid_range_value => apply output_mid as highest available speed
 * @param self Pointer address to the SpeedCtrl-instance.
 * @param front_sensor_input sensor value
 * @param left_sensor_input sensor value
 * @param right_sensor_input sensor value
 * @return int8_t
 */
int8_t SpeedCtrl_calc_speed(SpeedCtrl *self, uint16_t front_sensor_input, uint16_t left_sensor_input, uint16_t right_sensor_input)
{
    uint16_t side_sensor_input;
    uint8_t output_span;
    front_sensor_input = sanitize_sensor_input(self, front_sensor_input);
    left_sensor_input = sanitize_sensor_input(self, left_sensor_input);
    right_sensor_input = sanitize_sensor_input(self, right_sensor_input);

    if ((front_sensor_input <= self->break_distance) ||
        (left_sensor_input <= self->break_distance) ||
        (right_sensor_input <= self->break_distance))
    {
        return (int8_t)self->reverese_output;
    }

    if (left_sensor_input >= right_sensor_input)
    {
        side_sensor_input = right_sensor_input;
    }
    else
    {
        side_sensor_input = left_sensor_input;
    }

    if (side_sensor_input < self->min_side_distance)
    {
        float input_percent = (float)side_sensor_input / self->min_side_distance;
        output_span = self->output_mid - self->output_min;
        return (int8_t)((output_span * input_percent) + self->output_min);
    }

    float input_percent = (float)front_sensor_input / self->max_input;
    if (input_percent >= self->mid_range_limit)
    {
        output_span = self->output_mid - self->output_min;
    }
    else
    {
        output_span = self->output_max - self->output_min;
    }
    return (int8_t)((output_span * input_percent) + self->output_min);
}

/**
 * @brief sanitize the sensor input from larger values than max_input
 *
 * @param self Pointer address to the SpeedCtrl-instance.
 * @param sensor_input
 * @return uint16_t
 */
uint16_t sanitize_sensor_input(SpeedCtrl *self, uint16_t sensor_input)
{
    if (sensor_input > self->max_input)
    {
        return self->max_input;
    }
    return sensor_input;
}