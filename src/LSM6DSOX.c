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
*/

#include "LSM6DSOX.h"

static bool init(LSM6DSOX *self);

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

static bool init(LSM6DSOX *self)
{
    /**
    if (i2c_read_uint8_from_reg(self->i2c_address, LSM6DSOX_WHO_AM_I_REG) != 0x6C)
    {
        for(int i = 0 ; i<30;i++)
        {
            blink();
        }
        // end(self);
        return false;
    }
**/
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
void LSM6DSOXClass::end()
{
  if (_spi != NULL) {
    _spi->end();
    digitalWrite(_csPin, LOW);
    pinMode(_csPin, INPUT);
  } else {
    writeRegister(LSM6DSOX_CTRL2_G, 0x00);
    writeRegister(LSM6DSOX_CTRL1_XL, 0x00);
    _wire->end();
  }
}
**/

void LSM_read_acceleration(LSM6DSOX *self)
{
    int8_t data[6];
    i2c_read_bytes_from_reg(self->i2c_address, LSM6DSOX_OUTX_L_XL, 6, (uint8_t*)data);
    self->acc_x = ((data[1] << 8) | (data[0])) * 4.0 / 32768.0;
    self->acc_y = ((data[3] << 8) | (data[2])) * 4.0 / 32768.0;
    self->acc_z = ((data[5] << 8) | (data[4])) * 4.0 / 32768.0;
}

void LSM_read_gyroscope(LSM6DSOX *self)
{
    int8_t data[6];
    i2c_read_bytes_from_reg(self->i2c_address, LSM6DSOX_OUTX_L_G, 6, (uint8_t*)data);
    self->gyro_x = ((data[1] << 8) | (data[0])) * 2000.0 / 32768.0;
    self->gyro_y = ((data[3] << 8) | (data[2])) * 2000.0 / 32768.0;
    self->gyro_z = ((data[5] << 8) | (data[4])) * 2000.0 / 32768.0;
}

void LSM_read_temperature(LSM6DSOX *self)
{
    int8_t data[2];
    i2c_read_bytes_from_reg(self->i2c_address, LSM6DSOX_OUT_TEMP_L, 2, (uint8_t*)data);
    static int const TEMPERATURE_LSB_per_DEG = 256;
    static int const TEMPERATURE_OFFSET_DEG = 25;
    self->temperature = (((data[1] << 8) | (data[0])) / TEMPERATURE_LSB_per_DEG) + TEMPERATURE_OFFSET_DEG;
}

bool acceleration_available(LSM6DSOX *self)
{
    if (i2c_read_uint8_from_reg(self->i2c_address, LSM6DSOX_STATUS_REG) & 0x01)
    {
        return true;
    }
    return false;
}

bool gyroscope_available(LSM6DSOX *self)
{
    if (i2c_read_uint8_from_reg(self->i2c_address, LSM6DSOX_STATUS_REG) & 0x02)
    {
        return true;
    }
    return false;
}

bool temperature_available(LSM6DSOX *self)
{
    if (i2c_read_uint8_from_reg(self->i2c_address, LSM6DSOX_STATUS_REG) & 0x04)
    {
        return true;
    }
    return false;
}
