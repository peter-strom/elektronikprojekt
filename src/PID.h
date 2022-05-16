#ifndef PID_H_
#define PID_H_
#include "definitions.h"

typedef struct PID
{
    double target;
    double output_min;
    double output_max;
    double Kp;
    double Ki;
    double Kd;
    double Dt;
    double sensor_max;
    double sensor_min;
    double integral_max;
    double integral_min;

    double actual_value;
    double output;
    double current_error;
    double previous_error;
    double integral;
    double derivative;

    double left_sensor_input;
    double right_sensor_input;
    double mapped_right_sensor_input;
    double mapped_left_sensor_input;
} PID;

PID new_PID(const double target, const double Kp, const double Ki, const double Kd,
            const double output_min, const double output_max,const double integral_limit);

double PID_get_servo_value_from_sensors(PID *self, const double new_left_sensor_input, const double new_right_sensor_input, const float dt);
void print(PID *self);

#endif /* PID_H_ */