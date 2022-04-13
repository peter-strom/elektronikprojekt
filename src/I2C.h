#ifndef I2C_H_
#define I2C_H_
#include "header.h"

//macro
#define GYRO_ADDRESS 0x68 //mpu6050


// Function Declarations
bool i2c_write_uint8_to_reg(const uint8_t i2c_addr, const uint8_t reg, const uint8_t val);
uint8_t i2c_read_uint8_from_reg(const uint8_t i2c_addr, const uint8_t reg);
uint16_t i2c_read_uint16_from_reg(const uint8_t i2c_addr, const uint8_t reg);


void init_i2c(void);

#endif /* I2C_H_ */