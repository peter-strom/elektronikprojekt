#include "I2C.h"
//Function Definitions

void init_i2c(void)
{
    i2c_init(i2c_default, 400 * 1000); //400 kHz
    gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C); //GPIO 4
    gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C); //GPIO 5
   // gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN); //onödig?
   // gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN); //onödig?
    // Make the I2C pins available to picotool ??
    bi_decl(bi_2pins_with_func(PICO_DEFAULT_I2C_SDA_PIN, PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C));
}


/************************************************
* verifierade:
* i2c_write_uint8_to_reg
* i2c_read_uint8_from_reg
* i2c_read_uint16_from_reg
*************************************************/
void i2c_write_uint8_to_reg(const uint8_t i2c_addr, const uint8_t reg, const uint8_t val)
{
    uint8_t buf[] = {reg, val};
    i2c_write_blocking(i2c_default, i2c_addr, buf, 2, false);
}

uint8_t i2c_read_uint8_from_reg(const uint8_t i2c_addr, const uint8_t reg)
{ 
    uint8_t ret;
    i2c_write_blocking(i2c_default, i2c_addr, &reg, 1, true);
    i2c_read_blocking(i2c_default, i2c_addr, &ret, 1, false);
    return ret;
}

uint16_t i2c_read_uint16_from_reg(const uint8_t i2c_addr, const uint8_t reg)
{ 
    uint8_t ret[2];
    i2c_write_blocking(i2c_default, i2c_addr, &reg, 1, true);
    i2c_read_blocking(i2c_default, i2c_addr, ret, 2, false);
    return ((ret[0] << 8) | (ret[1]));
}
