#include "PID.h"

double sanitize_input(PID *self, double sensor_input);
void map_input(PID *self);
void sanitize_output(PID *self);
void sanitize_integral(PID *self);
void regulate(PID *self);
void print(PID *self);

/**
 * @brief Initialize an instance of a PID-structure.
 *
 * @param target Target or setpoint in degrees (90).
 * @param Kp Proportional gain constant.
 * @param Ki Integral gain constant.
 * @param Kd Derivative gain constant.
 * @param Dt Derative time constant.
 * @param output_min min steering angle in degrees
 * @param output_max max steering angle in degrees
 * @param integral_limit limit the integral value
 * @return PID instance of a PID-structure
 */
PID new_PID(const double target, const double Kp, const double Ki, const double Kd,
            const double output_min, const double output_max, const double integral_limit)
{
    PID self;
    self.target = target;
    self.output_min = output_min;
    self.output_max = output_max;
    self.Kp = Kp;
    self.Ki = Ki;
    self.Kd = Kd;
    self.Dt = 0.0;
    self.sensor_max = 1023.0;
    self.sensor_min = 0.0;
    self.integral_max = integral_limit;
    self.integral_min = integral_limit * -1;

    self.actual_value = 0;
    self.output = 0;
    self.current_error = 0;
    self.previous_error = 0;
    self.integral = 0;
    self.derivative = 0;

    self.left_sensor_input = 0x00;
    self.right_sensor_input = 0x00;
    self.mapped_right_sensor_input = 0x00;
    self.mapped_left_sensor_input = 0x00;
    return self;
}

/**
 * @brief Prevents output data from subceed/exceed the min/max allowed value.
 *
 * @param self Pointer address to the PID instance.
 * @param sensor_input Non sanitized sensor value.
 * @return double - Sanitized sensor value.
 */
double sanitize_input(PID *self, double sensor_input)
{
    if (sensor_input > self->sensor_max)
    {
        return self->sensor_max;
    }
    else if (sensor_input < self->sensor_min)
    {
        return self->sensor_min;
    }
    return sensor_input;
}

/**
 * @brief converts the sensor distance values to servo motor degrees.
 *
 * @param self Pointer address to the PID instance
 */
void map_input(PID *self)
{
    self->mapped_left_sensor_input = self->left_sensor_input / self->sensor_max * self->target;
    self->mapped_right_sensor_input = self->right_sensor_input / self->sensor_max * self->target;
}

/**
 * @brief prevents output data from subceed/exceed the min/max allowed value.
 *
 * @param self Pointer address to the PID instance
 */
void sanitize_output(PID *self)
{
    if (self->output > self->output_max)
    {
        self->output = self->output_max;
    }
    else if (self->output < self->output_min)
    {
        self->output = self->output_min;
    }
}

/**
 * @brief prevents output data from subceed/exceed the min/max allowed value.
 *
 * @param self Pointer address to the PID instance
 */
void sanitize_integral(PID *self)
{
    if (self->integral > self->integral_max)
    {
        self->integral = self->integral_max;
    }
    else if (self->integral < self->integral_min)
    {
        self->integral = self->integral_min;
    }
}

/**
 * @brief Where the PID control regulation happens
 *
 * @param self Pointer address to the PID instance
 */
void regulate(PID *self)
{
    self->current_error = self->target - self->actual_value;
    self->integral += self->Ki * self->current_error * self->Dt;
    sanitize_integral(self); // get stuck fix

    self->derivative = self->Kd * (self->current_error - self->previous_error) / self->Dt;
    self->output = self->target + self->Kp * self->current_error + self->integral + self->derivative;

    sanitize_output(self);

    self->previous_error = self->current_error;
}

/**
 * @brief Calculate a servo steering value from two sensors.
 *
 * @param self address to the PID instance
 * @param new_left_sensor_input value from the left sensor
 * @param new_right_sensor_input value from the right sensor
 * @param dt Delta time
 * @return double
 */
double PID_get_servo_value_from_sensors(PID *self, const double new_left_sensor_input, const double new_right_sensor_input, const float dt)
{
    self->Dt = dt;
    self->left_sensor_input = sanitize_input(self, new_left_sensor_input);
    self->right_sensor_input = sanitize_input(self, new_right_sensor_input);
    map_input(self);
    self->actual_value = self->target + self->mapped_left_sensor_input - self->mapped_right_sensor_input;
    regulate(self);
    return self->output;
}

/**
 * @brief Prints the values of the structure members from chosen PID instance.
 *
 * @param self Pointer address to the PID instance you want to examine.
 */
void print(PID *self)
{
    printf("integral: %f - \t", self->integral);
    printf("derivative: %f - \t", self->derivative);
}
