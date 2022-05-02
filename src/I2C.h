#ifndef I2C_H_
#define I2C_H_
#include "definitions.h"

// Function Declarations
void init_i2c(void);
bool i2c_write_uint8_to_reg(const uint8_t i2c_addr, const uint8_t reg, const uint8_t val);
uint8_t i2c_read_uint8_from_reg(const uint8_t i2c_addr, const uint8_t reg);
uint16_t i2c_read_uint16_from_reg(const uint8_t i2c_addr, const uint8_t reg);
uint32_t i2c_read_uint32_from_reg(const uint8_t i2c_addr, const uint8_t reg);
void i2c_read_bytes_from_reg(const uint8_t i2c_addr, const uint8_t reg, uint8_t no_of_bytes, uint8_t* ret_val);

#endif /* I2C_H_ */