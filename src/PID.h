#ifndef PID_H_
#define PID_H_
#include "header.h"

typedef struct PID 
{
	double target; 
	double output_min; 
	double output_max; 
	double Kp; 
	double Ki; 
	double Kd; 
	double sensor_max; 
	double sensor_min;
	
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
		const double output_min, const double output_max);

double PID_get_servo_value_from_sensors(PID* self, const double new_left_sensor_input, const double new_right_sensor_input);

#endif /* PID_H_ */