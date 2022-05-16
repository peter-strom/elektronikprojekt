/**
 * pinouts: https://docs.google.com/spreadsheets/d/14GNT5NQsH8dopmf7zK-Uy6bziWoMnjxaaNdR6yWJy5o/edit#gid=0
 *
 */

#ifndef HEADER_H_
#define HEADER_H_

#include "definitions.h"
#include "PWM.h"

#include "VL53L0X.h"

#include "PID.h"
#include "SpeedCtrl.h"
#include "MPU6050.h"

// GPIO Macros:
#define SERVO_PIN 13 // GPIO 13 PWM Channel 6A
#define ESC_PIN 28   // GPIO 28 PWM Channel 6B
#define LED_BUILTIN 6
#define TOF_SENSOR_L 20
#define TOF_SENSOR_F 18
#define TOF_SENSOR_R 16
#define START_MODULE 26

// global variables
VL53L0X sensor_left;
VL53L0X sensor_right;
VL53L0X sensor_front;
MPU6050 imu;
PWM pwm_servo;
PWM pwm_esc;
PID pid_servo;
SpeedCtrl speed_ctrl;

bool debug_pid;
bool debug_imu;
uint64_t prev_time;

// Global functions:
void setup(void);
void blink(void);
float calc_delta_time(uint64_t* prev_time);

#endif /* HEADER_H_ */