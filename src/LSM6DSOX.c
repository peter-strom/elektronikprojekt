/***************************************************************************
 * port based on the Arduino_LSM6DSOX library
 * https://github.com/arduino-libraries/Arduino_LSM6DSOX/tree/master/src
 *
 * Did never use the embedded IMU (LSM6DSOX) in this project due to issues
 * with simultaneous connection to external i2c devices.
 * Probably easy to fix. But the MPU6050 was within reach.
 ***************************************************************************/

#include "LSM6DSOX.h"

static bool init(LSM6DSOX *self);

/**
 * @brief Initialize an instance of a LSM6DSOX-structure.
 *
 * @return LSM6DSOX
 */
LSM6DSOX new_LSM6DSOX()
{
    LSM6DSOX self;
    self.i2c_address = LSM6DSOX_ADDRESS;
    self.sample_rate = 104.0F;
    self.acc_x = 0.0;
    self.acc_y = 0.0;
    self.acc_z = 0.0;
    self.gyro_x = 0.0;
    self.gyro_y = 0.0;
    self.gyro_z = 0.0;
    self.temperature = 0;
    init(&self);
    return self;
}

/**
 * @brief Initiates LSM6DSOX
 *
 * @param self
 */
static bool init(LSM6DSOX *self)
{
    // set the gyroscope control register to work at 104 Hz, 2000 dps and in bypass mode
    i2c_write_uint8_to_reg(self->i2c_address, LSM6DSOX_CTRL2_G, 0x4C);

    // Set the Accelerometer control register to work at 104 Hz, 4 g,and in bypass mode and enable ODR/4
    // low pass filter (check figure9 of LSM6DSOX's datasheet)
    i2c_write_uint8_to_reg(self->i2c_address, LSM6DSOX_CTRL1_XL, 0x4A);

    // set gyroscope power mode to high performance and bandwidth to 16 MHz
    i2c_write_uint8_to_reg(self->i2c_address, LSM6DSOX_CTRL7_G, 0x00);

    // Set the ODR config register to ODR/4
    i2c_write_uint8_to_reg(self->i2c_address, LSM6DSOX_CTRL8_XL, 0x09);

    return true;
}

/**
 * @brief function to read acceleration
 * little-endian
 * @param self
 */
void LSM_read_acceleration(LSM6DSOX *self)
{
    int8_t data[6];
    i2c_read_bytes_from_reg(self->i2c_address, LSM6DSOX_OUTX_L_XL, 6, (uint8_t *)data);
    self->acc_x = ((data[1] << 8) | (data[0])) * 4.0 / 32768.0;
    self->acc_y = ((data[3] << 8) | (data[2])) * 4.0 / 32768.0;
    self->acc_z = ((data[5] << 8) | (data[4])) * 4.0 / 32768.0;
}

/**
 * @brief function to read gyro
 * little-endian
 * @param self
 */
void LSM_read_gyroscope(LSM6DSOX *self)
{
    int8_t data[6];
    i2c_read_bytes_from_reg(self->i2c_address, LSM6DSOX_OUTX_L_G, 6, (uint8_t *)data);
    self->gyro_x = ((data[1] << 8) | (data[0])) * 2000.0 / 32768.0;
    self->gyro_y = ((data[3] << 8) | (data[2])) * 2000.0 / 32768.0;
    self->gyro_z = ((data[5] << 8) | (data[4])) * 2000.0 / 32768.0;
}

/**
 * @brief function to read temperature
 * little-endian
 * @param self
 */
void LSM_read_temperature(LSM6DSOX *self)
{
    int8_t data[2];
    i2c_read_bytes_from_reg(self->i2c_address, LSM6DSOX_OUT_TEMP_L, 2, (uint8_t *)data);
    static int const TEMPERATURE_LSB_per_DEG = 256;
    static int const TEMPERATURE_OFFSET_DEG = 25;
    self->temperature = (((data[1] << 8) | (data[0])) / TEMPERATURE_LSB_per_DEG) + TEMPERATURE_OFFSET_DEG;
}
