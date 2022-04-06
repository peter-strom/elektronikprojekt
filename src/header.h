#ifndef HEADER_H_
#define HEADER_H_

// Inkluderingsdirektiv:
#include "pico/stdlib.h"
#include <stdio.h>
//#include "pico/stdio.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"
#include "PWM.h"


//Makron
#define SERVO_PIN 6 //GPIO 6 PWM Channel 3A
#define ESC_PIN 13 //GPIO 13 PWM Channel 6B
#define LED_BUILTIN 6
//Globala variabler:
//Input ledButton ,motorButton;

// Global funktionsdeklaration:
void setup();
void blink();

#endif /* HEADER_H_ */