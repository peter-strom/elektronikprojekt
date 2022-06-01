#include "definitions.h"

#define MPU6050_ADDRESS 0x68

#define MPU6050_PWR_MGMT_1 0x6B
#define MPU6050_ACCEL_SENSITIVITY 0x1C
#define MPU6050_GYRO_SENSITIVITY 0x1B

#define MPU6050_OUT_TEMP_H 0x41
#define MPU6050_OUT_TEMP_L 0x42

#define MPU6050_OUTX_H_G 0x43
#define MPU6050_OUTX_L_G 0x44
#define MPU6050_OUTY_H_G 0x45
#define MPU6050_OUTY_L_G 0x46
#define MPU6050_OUTZ_H_G 0x47
#define MPU6050_OUTZ_L_G 0x48

#define MPU6050_OUTX_H_XL 0x3B
#define MPU6050_OUTX_L_XL 0x3C
#define MPU6050_OUTY_H_XL 0x3D
#define MPU6050_OUTY_L_XL 0x3E
#define MPU6050_OUTZ_H_XL 0x3F
#define MPU6050_OUTZ_L_XL 0x40
#define PI 3.1415

typedef struct MPU6050
{
    uint8_t i2c_address;
    float sample_rate;
    float acc_x;
    float acc_y;
    float acc_z;
    float gyro_x;
    float gyro_y;
    float gyro_z;
    float temperature;
    float acc_error_x;
    float acc_error_y;
    float gyro_error_x;
    float gyro_error_y;
    float gyro_error_z;
    uint8_t gyro_sensitivity_sel;
    float gyro_scale_factor;
    uint8_t acc_sensitivity_sel;
    float acc_scale_factor;
    float gyro_angle_x;
    float gyro_angle_y;
    float yaw;
    float roll;
    float pitch;
    float pitch_sum;
    uint16_t pitch_sum_count;
    uint16_t pitch_sum_max;
} MPU6050;

MPU6050 new_MPU6050();
void read_acceleration(MPU6050 *self);
void read_gyroscope(MPU6050 *self);
void read_temperature(MPU6050 *self);
void calc_orientation(MPU6050 *self, float dt);
