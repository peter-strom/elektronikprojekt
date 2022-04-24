#ifndef PID_H_
#define PID_H_
#include "header.h"


typedef struct PID 
{
	double target; // Börvärde, det värde vi strävar efter.
	double output_min; // Utsignalens minimumvärde.
	double output_max; // Utsignalen maximumvärde.
	double Kp; // Förstärkningsfaktor för proportionerlig del (P-delen).
	double Ki; // Förstärkningsfaktor för integrerande del (I-delen).
	double Kd; // Förstärkningsfaktor för deriverande del (D-delen).
	double sensor_max; 
	double sensor_min;
	
	double actual_value; // Ärvärde, det värde vi faktiskt har. Ärvärde = 90 + vänster - höger
	double output; // Regulatorns utsignal, anpassas efter bör- och ärvärde.
	double current_error; // Aktuellt fel.
	double last_error; // Föregående fel.
	double integral; // Integralen av aktuellt fel (ökar/minskar i takt med aktuellt fel).
	double derivative; // Derivatan av felet (aktuellt fel - föregående fel).

	double left_sensor_input;
	double right_sensor_input;
	double mapped_right_sensor_input;
	double mapped_left_sensor_input;




} PID;


PID new_PID(const double target, const double Kp, const double Ki, const double Kd,
		const double output_min, const double output_max);

double PID_regulate(PID* self, const double new_left_sensor_input, const double new_right_sensor_input);




#endif /* PID_H_ */