#include "MPU6050.h"

static void init(MPU6050 *self);
static void calc_IMU_error(MPU6050 *self);
static void pitch_reset(MPU6050 * self);


/**
 * @brief 
 * @details
 * https://www.diva-portal.org/smash/get/diva2:1127455/FULLTEXT02.pdf s21
 * https://invensense.tdk.com/wp-content/uploads/2015/02/MPU-6000-Datasheet1.pdf
 * https://invensense.tdk.com/wp-content/uploads/2015/02/MPU-6000-Register-Map1.pdf
 * Sensitivity Selection and Scale Factor
 * GYRO SENSITIVITY     250     500     1000    2000   (deg/s)
 * gyro_sensitivity_sel  0x00    0x08    0x10    0x18
 * gyro_scale_factor     131     65.5    32.8    16.4
 * 
 * ACCEL SENSITIVITY    2       4       8       16     (+-g)
 * acc_sensitivity_sel  0x00    0x08    0x10    0x18
 * gyro_scale_factor     16384   8192    4096    2048
 * 
 * @param self 
 */

MPU6050 new_MPU6050()
{
    MPU6050 self;
    self.i2c_address = MPU6050_ADDRESS;
    self.acc_x = 0.0;
    self.acc_y = 0.0;
    self.acc_z = 0.0;
    self.gyro_x = 0.0;
    self.gyro_y = 0.0;
    self.gyro_z = 0.0;
    self.temperature = 0;
    self.acc_error_x = 0.0;
    self.acc_error_y = 0.0; 
    self.gyro_error_x = 0.0; 
    self.gyro_error_y = 0.44195;
    self.gyro_error_z = 0.0;
    self.gyro_sensitivity_sel = 0x00;
    self.gyro_scale_factor = 131.0;
    self.acc_sensitivity_sel = 0x10;
    self.acc_scale_factor = 4096.0;
    self.gyro_angle_x = 0.0;
    self.gyro_angle_y = 0.0;
    self.yaw = 0.0;
    self.roll = 0.0;
    self.pitch = 0.0;
    self.pitch_sum = 0.0;
    self.pitch_sum_count = 0;
    self.pitch_sum_max = 400 ;
    init(&self);
    return self;
}

static void init(MPU6050 *self)
{
     // RESET
    i2c_write_uint8_to_reg(self->i2c_address, MPU6050_PWR_MGMT_1, 0x00);
    // +- 8g (default +- 2g)
    //i2c_write_uint8_to_reg(self->i2c_address, MPU6050_ACCEL_SENSITIVITY, self->acc_sensitivity_sel);
    // +- 1000 deg/s (default +- 250 deg/s)
    //i2c_write_uint8_to_reg(self->i2c_address, MPU6050_GYRO_SENSITIVITY, self->gyro_sensitivity_sel);
    //calc_IMU_error(self);
}

static void calc_IMU_error(MPU6050 *self)
{
    float acc_error_x;
    float acc_error_y;
    float gyro_error_x;
    float gyro_error_y;
    float gyro_error_z;
    uint16_t iterations = 1000;
    for(uint16_t i = 0; i < iterations; i++)
    {
        read_acceleration(self);
        read_gyroscope(self);
        acc_error_x += ((atanf((self->acc_y) / sqrtf(powf((self->acc_x), 2) + powf((self->acc_z), 2))) * 180 / PI));
        acc_error_y += ((atanf(-1 * (self->acc_x) / sqrtf(powf((self->acc_y), 2) + powf((self->acc_z), 2))) * 180 / PI));
        gyro_error_x += self->gyro_x;
        gyro_error_y += self->gyro_y;
        gyro_error_z += self->gyro_z;
    }
    self->acc_error_x = (acc_error_x / iterations)*-1;
    self->acc_error_y = (acc_error_y / iterations)*-1; 
    self->gyro_error_x = (gyro_error_x / iterations)*-1; 
    self->gyro_error_y = (gyro_error_y / iterations)*-1;
    self->gyro_error_z = (gyro_error_z / iterations)*-1; 
}

void read_acceleration(MPU6050 *self)
{
    uint8_t data[6];
    i2c_read_bytes_from_reg(self->i2c_address, MPU6050_OUTX_H_XL, 6, data);
    self->acc_x = ((int16_t)((data[0] << 8) | (data[1]))) / self->acc_scale_factor;
    self->acc_y = ((int16_t)((data[2] << 8) | (data[3]))) / self->acc_scale_factor;
    self->acc_z = ((int16_t)((data[4] << 8) | (data[5]))) / self->acc_scale_factor;
}

void read_gyroscope(MPU6050 *self)
{
    uint8_t data[6];
    i2c_read_bytes_from_reg(self->i2c_address, MPU6050_OUTX_H_G, 6, data);
    self->gyro_x = ((int16_t)((data[0] << 8) | (data[1]))) / self->gyro_scale_factor;
    self->gyro_y = ((int16_t)((data[2] << 8) | (data[3]))) / self->gyro_scale_factor;
    self->gyro_z = ((int16_t)((data[4] << 8) | (data[5]))) / self->gyro_scale_factor;
}

void read_temperature(MPU6050 *self)
{
    uint8_t data[2];
    i2c_read_bytes_from_reg(self->i2c_address, MPU6050_OUT_TEMP_H, 2, data);
    static float const TEMPERATURE_LSB_per_DEG = 340;
    static float const TEMPERATURE_OFFSET_DEG = 36.54;
    self->temperature = ((int16_t)((data[0] << 8) | (data[1]))/ TEMPERATURE_LSB_per_DEG) + TEMPERATURE_OFFSET_DEG;
}

/**
 * @brief calculates yaw, roll and pitch 
 * 
 * @param self 
 * @param dt Delta time
 */
void calc_orientation(MPU6050 *self, float dt)
{
    pitch_reset(self);

    read_acceleration(self);
    read_gyroscope(self);
    float acc_ang_x = ((atanf((self->acc_y) / sqrtf(powf((self->acc_x), 2) + powf((self->acc_z), 2))) * 180 / PI)+self->acc_error_x);
    float acc_ang_y = ((atanf(-1 * (self->acc_x) / sqrtf(powf((self->acc_y), 2) + powf((self->acc_z), 2))) * 180 / PI)+self->acc_error_y);
    //self->gyro_angle_x += (self->gyro_x + self->gyro_error_x) * dt;
    //self->gyro_angle_y += (self->gyro_y + self->gyro_error_y) * dt;
    self->roll += (self->gyro_x + self->gyro_error_x) * dt;
    self->pitch += (self->gyro_y + self->gyro_error_y) * dt;
    self->yaw += (self->gyro_z + self->gyro_error_z) * dt;
    //self->roll = 0.96 * self->gyro_angle_x +0.04 * acc_ang_x;
    //self->pitch = 0.96 * self->gyro_angle_y +0.04 * acc_ang_y;
    
}


static void pitch_reset(MPU6050 * self)
{
    if(self->pitch_sum_count++ < self->pitch_sum_max)
    {
        self->pitch_sum += self->pitch;
    }
    else
    {
        
        if((self->pitch_sum / self->pitch_sum_max ) >= (self->pitch - 1) || (self->pitch_sum / self->pitch_sum_max ) <= (self->pitch + 1))
        {
            self->pitch = 0.0;
        }
        self->pitch_sum_count = 0;
        self->pitch_sum = 0.0;
    }
    
}
