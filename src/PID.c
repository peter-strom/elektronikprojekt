#include "PID.h"


double sanitize_input(PID* self, double sensor_input); //kontrollera insignal från sensor
void map_input(PID* self); //Mappningsfunktion
void sanitize_output(PID* self); // Kontrollerar utsignalens värde, jämför med min- och max.
void regulate(PID* self); 
void print(PID* self); 

/**
 * @brief
 * 
 * @param target in degrees
 * @param Kp 
 * @param Ki 
 * @param Kd 
 * @param output_min in degrees 0 
 * @param output_max in degrees 180
 * @return PID 
 */
PID new_PID(const double target, const double Kp, const double Ki,
	const double Kd, const double output_min, const double output_max)
{
    PID self;
    self.target = target; // 90 grader
	self.output_min = output_min; // 0 grader
	self.output_max = output_max; // 180 grader
	self.Kp = Kp;
	self.Ki = Ki;
	self.Kd = Kd;
	self.sensor_max = 1023.0; 
	self.sensor_min = 0.0;

	self.actual_value = 0; // Ärvärde, det värde vi faktiskt har.
	self.output = 0; // Regulatorns utsignal, anpassas efter bör- och ärvärde.
	self.current_error = 0; // Aktuellt fel.
	self.last_error = 0; // Föregående fel.
    self.integral = 0; // Integralen av aktuellt fel (ökar/minskar i takt med aktuellt fel).
	self.derivative = 0; // Derivatan av felet (aktuellt fel - föregående fel).

	self.left_sensor_input = 0x00;
	self.right_sensor_input = 0x00;
	self.mapped_right_sensor_input = 0x00;
	self.mapped_left_sensor_input = 0x00;



    return self;
}


double sanitize_input(PID* self, double sensor_input)
{
	if(sensor_input > self->sensor_max)
	{
		return self->sensor_max;
	}
	else if (sensor_input < self->sensor_min)
	{
		return self->sensor_min;
	}
	return sensor_input;
}

	// Sparat mappat värde av vänster insignal (från 0 - 1023 ned till 0 - 180):
void map_input(PID* self)
{
	self->mapped_left_sensor_input = self->left_sensor_input / self->sensor_max * self->target;
	self->mapped_right_sensor_input = self->right_sensor_input / self->sensor_max * self->target;
}

/***************************************************************************************************
* Metoden check_output används för att kontrollera så att aktuell utsignal inte överstiger
* satt maximumvärde eller understiger angivet minimumvärde. Om användaren inte har angivit några
* värden eller om samma värden på min- och max har angivits, så görs ingen kontroll. Annars
* justeras utsignal efter behov.
***************************************************************************************************/
void sanitize_output(PID* self)
{
	if (self->output_min == self->output_max)
		return;
	if (self->output > self->output_max)
		self->output = self->output_max;
	else if (self->output < self->output_min)
		self->output = self->output_min;
}


// Genomför reglering.
/***************************************************************************************************
* Metoden regulate används för att genomföra en reglering på en PID-regulator. Aktuellt fel
* beräknas som differensen mellan aktuellt bör- och ärvärde. Integralen av felet uppdateras genom
* att addera aktuellt fel. Derivatan av felete beräknas som differensen mellan aktuellt fel och
* föregående fel. Därmed gäller att om nuvarande fel är större än föregående fel, så ökar felet.
* D-delen av regulatorn kommer då medföra ökar reglering, så att felet minskar. Utsignalen
* uppdateras genom att addera börvärdet med respektive del av regulatorn (Kp * aktuellt fel
* för P-delen, Ki * integralen för I-delen samt Kd * derivatan för D-delen). Uppdated utsignal
* kontrolleras via anrop av metoden check_output. Nuvarande fel sparas sedan som föregående fel
* inför nästa reglering (behövs för beräkning av derivatan).
***************************************************************************************************/
void regulate(PID* self)
{
	self->current_error = self->target - self->actual_value;
	self->integral += self->current_error;
	self->derivative = self->current_error - self->last_error;
	self->output = self->target + self->Kp * self->current_error +
		self->Ki * self->integral + self->Kd * self->derivative;
	sanitize_output(self);
	self->last_error = self->current_error;
}


double PID_regulate(PID* self, const double new_left_sensor_input, const double new_right_sensor_input)
{
	self->left_sensor_input = sanitize_input(self, new_left_sensor_input);
	self->right_sensor_input = sanitize_input(self, new_right_sensor_input);
	map_input(self);
	self->actual_value = self->target + self->mapped_left_sensor_input - self->mapped_right_sensor_input;
	regulate(self);
	printf("output: %f\n", self->output);
	return self->output;
}

// Skriver ut PID-regulatorns aktuella parametrar.

/***************************************************************************************************
* Metoden print används för att skriva ut aktuella parametrar gällande en PID-regulator. Här
* sker utskrift av börvärdet, ärvärdet, aktuell utsignal, aktuellt fel samt föregående fel.
***************************************************************************************************/
void print(PID* self)
{
	printf("----------------------------------------------------------------------------\n");
	printf("Target value: %f\n", self->target);
	printf("Actual value: %f\n", self->actual_value);
	printf("Output: %f\n", self->output);
	printf("Current error: %f\n", self->current_error);
	printf("Last error: %f\n", self->last_error);
	printf("----------------------------------------------------------------------------\n\n");
		printf("Left input: %f\n", self->left_sensor_input);
	printf("Right input: %f\n", self->right_sensor_input);
	printf("Mapped left input: %f\n", self->mapped_left_sensor_input);
	printf("Mapped right input: %f\n", self->mapped_right_sensor_input);
	printf("----------------------------------------------------------------\n");
	if (self->output > self->target)
	{
		printf("unit driving: %f degrees to the right\n\n", self->output-self->target);
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
