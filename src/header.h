#ifndef HEADER_H_
#define HEADER_H_

// includes:
#include "pico/stdlib.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include "hardware/pwm.h"
#include "hardware/clocks.h"
#include "PWM.h"
#include "hardware/i2c.h"
#include "VL53L0X.h"
#include "I2C.h"
//#include <string.h>
#include "pico/binary_info.h"

//Macros:
#define SERVO_PIN 6 //GPIO 6 PWM Channel 3A
#define ESC_PIN 13 //GPIO 13 PWM Channel 6B
#define LED_BUILTIN 6
#define VL53L0X_ADDRESS 0x29


//global variables


// Global functions:
void setup(void);
void blink(void);

#endif /* HEADER_H_ */