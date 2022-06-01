#include "SpeedCtrl.h"

static uint16_t sanitize_sensor_input(SpeedCtrl *self, uint16_t sensor_input);
static void reverse_switch(SpeedCtrl *self, uint16_t front_sensor_input, uint16_t left_sensor_input, uint16_t right_sensor_input);
static float get_factor_from_servo_angle(uint8_t servo_angle);

/**
 * @brief Initialize an instance of a SpeedCtrl-structure.
 *
 * @param max_input max value of sensor input
 * @param output_min min output value
 * @param output_mid mid output value
 * @param mid_range_limit (0-100) where to apply output_mid as highest output value
 * @param output_max max output value
 * @param min_side_distance distance from side sensors where to slow down
 * @param reverse_output negative value between -100 and 0 representing reverse speed
 * @param rev_trigger_distance distance to trigger reverse direction
 * @param rev_stop_distance distance to stop the reverse direction
 * @return SpeedCtrl Instance of a SpeedCtrl-structure
 */
SpeedCtrl new_SpeedCtrl(uint16_t max_input, uint8_t output_min, uint8_t output_mid,
                        uint8_t mid_range_limit, uint8_t output_max, uint8_t min_side_distance,
                        int8_t reverse_output, uint8_t rev_trigger_distance, uint8_t rev_stop_distance)
{
    SpeedCtrl self;
    self.max_input = max_input;
    self.output_min = output_min;
    self.output_mid = output_mid;
    self.mid_range_limit = mid_range_limit;
    self.output_max = output_max;
    self.min_side_distance = min_side_distance;
    self.reverese_output = reverse_output;
    self.rev_trigger_distance = rev_trigger_distance;
    self.rev_stop_distance = rev_stop_distance;
    self.reverse = false;
    self.rev_count = 0;
    self.rev_count_max = 500;
    self.old_output = 0;
    self.pwr_count = 0;
    self.pwr_count_max = 130;
    self.pwr_output = false;
    return self;
}

/**
 * @brief Takes input value from from sensors and calculates a safe speed.
 * @details (remember gear protector in PWM.c:set_speed)
 * -obstacle solving via reverse_switch()
 * -curve control via get_factor_from_servo_angle()
 * -non linear variable speed control based on the front sensors distance to obstacle
 * -power mode if the car detects an uphill or drastic removal of obstacle in front of the car
 * NOTE: Only about one hour testing with IMU sensor, probably some bugs left to fix related to the power-mode
 * @param self Pointer address to the SpeedCtrl-instance.
 * @param front_sensor_input sensor value
 * @param left_sensor_input sensor value
 * @param right_sensor_input sensor value
 * @param servo_angle value between 0 and 200 where 100 is streight forward
 * @return int8_t
 */
int8_t SpeedCtrl_calc_speed(SpeedCtrl *self, uint16_t front_sensor_input, uint16_t left_sensor_input, uint16_t right_sensor_input, uint8_t servo_angle, float pitch_value)
{
    uint16_t side_sensor_input;
    uint8_t output_span;
    front_sensor_input = sanitize_sensor_input(self, front_sensor_input);
    left_sensor_input = sanitize_sensor_input(self, left_sensor_input);
    right_sensor_input = sanitize_sensor_input(self, right_sensor_input);
    float curve_slowdown_factor = get_factor_from_servo_angle(servo_angle);

    // disengage power mode in curves
    if ((curve_slowdown_factor < 0.8) && self->pwr_output)
    {
        self->pwr_output = false;
    }

    reverse_switch(self, front_sensor_input, left_sensor_input, right_sensor_input);
    if (self->reverse)
    {
        return (int8_t)self->reverese_output;
    }

    // decide if the calculated speed will be based of output_mid or output_max value
    float input_percent = (float)front_sensor_input / self->max_input;
    if (input_percent >= self->mid_range_limit)
    {
        output_span = self->output_mid - self->output_min;
    }
    else
    {
        output_span = self->output_max - self->output_min;
    }
    uint8_t output = (int8_t)((output_span * input_percent) + self->output_min) * curve_slowdown_factor;
    if (output < self->output_min)
    {
        output = self->output_min;
    }

    if ((front_sensor_input == self->max_input) && (self->old_output < 40))
    {
        self->pwr_output = true;
    }

    // power mode on if the car sense a uphill between 9 and 45 degrees
    if (pitch_value >= 45)
    {
        self->pwr_output = false;

        return 0;
    }
    else if (pitch_value < 45 && pitch_value >= 9)
    {
        self->pwr_output = true;
        self->pwr_count = 0;
    }
    else
    {
        self->pwr_output = false;
    }

    if (self->pwr_output)
    {

        if ((self->pwr_count++ == self->pwr_count_max))
        {
            self->pwr_output = false;
            self->pwr_count = 0;
        }
        self->old_output = 89;
        return 89;
    }
    self->old_output = output;
    return output;
}

/**
 * @brief sanitize the sensor input from larger values than max_input
 *
 * @param self Pointer address to the SpeedCtrl-instance.
 * @param sensor_input
 * @return uint16_t
 */
static uint16_t sanitize_sensor_input(SpeedCtrl *self, uint16_t sensor_input)
{
    if (sensor_input > self->max_input)
    {
        return self->max_input;
    }
    return sensor_input;
}

/**
 * @brief switch reverse mode on and off
 * @details
 * The reverse mode is active when the car has felt an obstacle for a certain
 * period of time, until the desired distance from the obstacle is reached.
 * @param self Pointer address to the SpeedCtrl-instance.
 * @param front_sensor_input sensor value
 * @param left_sensor_input sensor value
 * @param right_sensor_input sensor value
 */
static void reverse_switch(SpeedCtrl *self, uint16_t front_sensor_input, uint16_t left_sensor_input, uint16_t right_sensor_input)
{
    if (!self->reverse && ((front_sensor_input <= self->rev_trigger_distance) ||
                           (left_sensor_input <= self->rev_trigger_distance) ||
                           (right_sensor_input <= self->rev_trigger_distance)))
    {
        if (self->rev_count++ == self->rev_count_max)
        {
            self->reverse = true;
            self->rev_count = 0;
        }
    }

    else if (self->reverse)
    {
        if ((self->rev_count++ == self->rev_count_max) ||
            ((front_sensor_input >= self->rev_stop_distance) &&
             (left_sensor_input >= self->rev_stop_distance) &&
             (right_sensor_input >= self->rev_stop_distance)))
        {
            self->rev_count = 0;
            self->reverse = false;
        }
    }
    else
    {
        self->rev_count = 0;
    }
}

/**
 * @brief calculates a factor based on the steering angle
 * @details
 * the purpose of the factor is to slow down the car in curves
 * @param servo_angle
 * @return float
 */
static float get_factor_from_servo_angle(uint8_t servo_angle)
{
    if (servo_angle > 100)
    {
        servo_angle = 200 - servo_angle;
    }
    if (servo_angle < 50 && servo_angle >= 30)
    {
        return 1;
    }
    if (servo_angle < 30)
    {
        return 0.85;
    }
    return 1.0;
}