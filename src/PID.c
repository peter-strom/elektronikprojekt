#include "PID.h"

void check_output(PID* self); // Kontrollerar utsignalens värde, jämför med min- och max.

PID new_PID(const double target, const double Kp, const double Ki,
	const double Kd, const double output_min, const double output_max)
{
    PID self;
    self.target = target;
	self.Kp = Kp;
	self.Ki = Ki;
	self.Kd = Kd;
	self.output_min = output_min;
	self.output_max = output_max;

	self.actual_value = 0; // Ärvärde, det värde vi faktiskt har.
	self.output = 0; // Regulatorns utsignal, anpassas efter bör- och ärvärde.
	self.current_error = 0; // Aktuellt fel.
	self.last_error = 0; // Föregående fel.
    self.integral = 0; // Integralen av aktuellt fel (ökar/minskar i takt med aktuellt fel).
	self.derivative = 0; // Derivatan av felet (aktuellt fel - föregående fel).


    return self;
}



/***************************************************************************************************
* Metoden check_output används för att kontrollera så att aktuell utsignal inte överstiger
* satt maximumvärde eller understiger angivet minimumvärde. Om användaren inte har angivit några
* värden eller om samma värden på min- och max har angivits, så görs ingen kontroll. Annars
* justeras utsignal efter behov.
***************************************************************************************************/
void check_output(PID* self)
{
	if (self->output_min == self->output_max)
		return;
	if (self->output > self->output_max)
		self->output = self->output_max;
	else if (self->output < self->output_min)
		self->output = self->output_min;
}


void set_target(PID* self, const double new_target)
{
    self->target = new_target;
}
/***************************************************************************************************
* Metoden set_parameters används för att ställa in nya förstärkningsfaktorer på en PID-regulator.
* Ingående argument Kp, Ki samt Kd utgör förstärkningsfaktor för respektive del av regulatorn,
* vilket tilldelas till instansvariablerna med samma namn.
***************************************************************************************************/
void set_parameters(PID* self, const double Kp, const double Ki, const double Kd)
{
	self->Kp = Kp;
	self->Ki = Ki;
	self->Kd = Kd;
}
void set_actual_value(PID* self, const double new_actual_value)
{
    self->actual_value = new_actual_value; 
}

// Returnerar aktuellt utsignal.
double get_output(PID* self)
{ 
    return self->output;
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
	check_output(self);
	self->last_error = self->current_error;
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
}
