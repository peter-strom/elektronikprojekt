/**
 * pinouts: https://docs.google.com/spreadsheets/d/14GNT5NQsH8dopmf7zK-Uy6bziWoMnjxaaNdR6yWJy5o/edit#gid=0
 * 
 */

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

//GPIO Macros:
#define SERVO_PIN 13 //GPIO 13 PWM Channel 6A
#define ESC_PIN 28 //GPIO 28 PWM Channel 6B
#define LED_BUILTIN 6
#define TOF_SENSOR_L 20
#define TOF_SENSOR_F 18
#define TOF_SENSOR_R 16



//global variables


// Global functions:
void setup(void);
void blink(void);


#endif /* HEADER_H_ */