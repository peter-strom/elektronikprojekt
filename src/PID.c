#include "PID.h"

double sanitize_input(PID *self, double sensor_input);
void map_input(PID *self);
void sanitize_output(PID *self);
void regulate(PID *self);
void print(PID *self);

/**
 * @brief Initialize an instance of a PID-structure.
 *
 * @param target Target or setpoint in degrees (90).
 * @param Kp Proportional gain constant.
 * @param Ki Integral gain constant.
 * @param Kd Derivative gain constant.
 * @param output_min min steering angle in degrees
 * @param output_max max steering angle in degrees
 * @return PID instance of a PID-structure
 */
PID new_PID(const double target, const double Kp, const double Ki,
            const double Kd, const double output_min, const double output_max)
{
    PID self;
    self.target = target;
    self.output_min = output_min;
    self.output_max = output_max;
    self.Kp = Kp;
    self.Ki = Ki;
    self.Kd = Kd;
    self.sensor_max = 1023.0;
    self.sensor_min = 0.0;

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
 * @brief Where the PID control regulation happens
 *
 * @param self Pointer address to the PID instance
 */
void regulate(PID *self)
{
    self->current_error = self->target - self->actual_value;
    self->integral += self->current_error;
    self->derivative = self->current_error - self->previous_error;
    self->output = self->target + self->Kp * self->current_error +
                   self->Ki * self->integral + self->Kd * self->derivative;
    sanitize_output(self);
    self->previous_error = self->current_error;
}

/**
 * @brief Calculate a servo steering value from two sensors.
 *
 * @param self address to the PID instance
 * @param new_left_sensor_input value from the left sensor
 * @param new_right_sensor_input value from the right sensor
 * @return double
 */
double PID_get_servo_value_from_sensors(PID *self, const double new_left_sensor_input, const double new_right_sensor_input)
{
    self->left_sensor_input = sanitize_input(self, new_left_sensor_input);
    self->right_sensor_input = sanitize_input(self, new_right_sensor_input);
    map_input(self);
    self->actual_value = self->target + self->mapped_left_sensor_input - self->mapped_right_sensor_input;
    regulate(self);
    printf("output: %f\n", self->output);
    return self->output;
}

/**
 * @brief Prints the values of the structure members from chosen PID instance.
 *
 * @param self Pointer address to the PID instance you want to examine.
 */
void print(PID *self)
{
    printf("----------------------------------------------------------------------------\n");
    printf("Target value: %f\n", self->target);
    printf("Actual value: %f\n", self->actual_value);
    printf("Output: %f\n", self->output);
    printf("Current error: %f\n", self->current_error);
    printf("previous error: %f\n", self->previous_error);
    printf("----------------------------------------------------------------------------\n\n");
    printf("Left input: %f\n", self->left_sensor_input);
    printf("Right input: %f\n", self->right_sensor_input);
    printf("Mapped left input: %f\n", self->mapped_left_sensor_input);
    printf("Mapped right input: %f\n", self->mapped_right_sensor_input);
    printf("----------------------------------------------------------------\n");
    if (self->output > self->target)
    {
        printf("unit driving: %f degrees to the right\n\n", self->output - self->target);
    }
    else if (self->output < self->target)
    {
        printf("unit driving: %f degrees to the left\n\n", self->target - self->output);
    }
    else
    {
        printf("unit driving: streight ahead\n\n");
    }
}
