#ifndef PID_H_
#define PID_H_
#include "header.h"


typedef struct PID 
{
	double target; // Börvärde, det värde vi strävar efter.
	double actual_value; // Ärvärde, det värde vi faktiskt har.
	double output; // Regulatorns utsignal, anpassas efter bör- och ärvärde.
	double current_error; // Aktuellt fel.
	double last_error; // Föregående fel.
	double Kp; // Förstärkningsfaktor för proportionerlig del (P-delen).
	double Ki; // Förstärkningsfaktor för integrerande del (I-delen).
	double Kd; // Förstärkningsfaktor för deriverande del (D-delen).
	double integral; // Integralen av aktuellt fel (ökar/minskar i takt med aktuellt fel).
	double derivative; // Derivatan av felet (aktuellt fel - föregående fel).
	double output_min; // Utsignalens minimumvärde.
	double output_max; // Utsignalen maximumvärde.
	
} PID;


PID new_PID(const double target, const double Kp, const double Ki, const double Kd,
		const double output_min, const double output_max);
	
void set_target(PID* self, const double new_target);
void set_parameters(PID* self, const double Kp, const double Ki, const double Kd);
void set_actual_value(PID* self, const double new_actual_value);
double get_output(PID* self); 
void regulate(PID* self); 
void print(PID* self); 

#endif /* PID_H_ */