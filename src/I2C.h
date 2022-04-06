#ifndef I2C_H_
#define I2C_H_
#include "header.h"

//macro
#define GYRO_ADDRESS 0x68 //mpu6050


// Function Declarations
void mpu6050_reset();
void mpu6050_read_raw(int16_t accel[3], int16_t gyro[3], int16_t *temp);

int reg_write(i2c_inst_t *i2c, 
                const uint addr, 
                const uint8_t reg, 
                uint8_t *buf,
                const uint8_t nbytes);

int reg_read(i2c_inst_t *i2c,
                const uint addr,
                const uint8_t reg,
                uint8_t *buf,
                const uint8_t nbytes);

void init_i2c(void);

#endif /* I2C_H_ */