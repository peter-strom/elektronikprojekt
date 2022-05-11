/*
  This file is part of the Arduino_LSM6DSOX library.
  Copyright (c) 2021 Arduino SA. All rights reserved.
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.
  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

  https://github.com/arduino-libraries/Arduino_LSM6DSOX/tree/master/src
*/
#define LSM6DSOX_ADDRESS 0x6A

#define LSM6DSOX_WHO_AM_I_REG 0X0F
#define LSM6DSOX_CTRL1_XL 0X10
#define LSM6DSOX_CTRL2_G 0X11

#define LSM6DSOX_STATUS_REG 0X1E

#define LSM6DSOX_CTRL6_C 0X15
#define LSM6DSOX_CTRL7_G 0X16
#define LSM6DSOX_CTRL8_XL 0X17

#define LSM6DSOX_OUT_TEMP_L 0X20
#define LSM6DSOX_OUT_TEMP_H 0X21

#define LSM6DSOX_OUTX_L_G 0X22
#define LSM6DSOX_OUTX_H_G 0X23
#define LSM6DSOX_OUTY_L_G 0X24
#define LSM6DSOX_OUTY_H_G 0X25
#define LSM6DSOX_OUTZ_L_G 0X26
#define LSM6DSOX_OUTZ_H_G 0X27

#define LSM6DSOX_OUTX_L_XL 0X28
#define LSM6DSOX_OUTX_H_XL 0X29
#define LSM6DSOX_OUTY_L_XL 0X2A
#define LSM6DSOX_OUTY_H_XL 0X2B
#define LSM6DSOX_OUTZ_L_XL 0X2C
#define LSM6DSOX_OUTZ_H_XL 0X2D

#include "definitions.h"

typedef struct LSM6DSOX
{
    uint8_t i2c_address;
    float sample_rate;
    float acc_x;
    float acc_y;
    float acc_z;
    float gyro_x;
    float gyro_y;
    float gyro_z;
    int temperature;
} LSM6DSOX;

LSM6DSOX new_LSM6DSOX();
void readAcceleration(LSM6DSOX *self);
void readGyroscope(LSM6DSOX *self);
void readTemperature(LSM6DSOX *self);
bool accelerationAvailable(LSM6DSOX *self);
bool gyroscopeAvailable(LSM6DSOX *self);
bool temperatureAvailable(LSM6DSOX *self);