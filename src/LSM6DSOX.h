/***************************************************************************
 * port based on the Arduino_LSM6DSOX library
 * https://github.com/arduino-libraries/Arduino_LSM6DSOX/tree/master/src
 *
 * Did never use the embedded IMU (LSM6DSOX) in this project due to issues
 * with simultaneous connection to external i2c devices.
 * Probably easy to fix. But the MPU6050 was within reach.
 ***************************************************************************/
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
void LSM_read_acceleration(LSM6DSOX *self);
void LSM_read_gyroscope(LSM6DSOX *self);
void LSM_read_temperature(LSM6DSOX *self);
bool acceleration_available(LSM6DSOX *self);
bool gyroscope_available(LSM6DSOX *self);
bool temperature_available(LSM6DSOX *self);