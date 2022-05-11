#include "I2C.h"

/**
 * @brief Initiates i2c
 * @details 400 kHz
 *          SDA -> GPIO 4
 *          SCL -> GPIO 5
 */
void init_i2c(void)
{
    i2c_init(i2c_default, 400 * 1000);
    //gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
    //gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_set_function(12, GPIO_FUNC_I2C);
    gpio_set_function(13, GPIO_FUNC_I2C);
    // Make the I2C pins available to picotool ??
    bi_decl(bi_2pins_with_func(PICO_DEFAULT_I2C_SDA_PIN, PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C));
}

/**
 * @brief Write 8-bits to a register on a specific i2c device
 *
 * @param i2c_addr The physical address of the i2c devie (8-bit)
 * @param reg The register on the i2c device to write to.
 * @param val The value to write
 * @return bool
 *
 */
bool i2c_write_uint8_to_reg(const uint8_t i2c_addr, const uint8_t reg, const uint8_t val)
{
    uint8_t buf[] = {reg, val};
    return i2c_write_blocking(i2c_default, i2c_addr, buf, 2, false);
}

/**
 * @brief Read 8-bits from a register on a specific i2c device
 *
 * @param i2c_addr The physical address of the i2c devie (8-bit)
 * @param reg The register on the i2c device to read from.
 * @return uint8_t
 */
uint8_t i2c_read_uint8_from_reg(const uint8_t i2c_addr, const uint8_t reg)
{
    uint8_t ret;
    i2c_write_blocking(i2c_default, i2c_addr, &reg, 1, true);
    i2c_read_blocking(i2c_default, i2c_addr, &ret, 1, false);
    return ret;
}

/**
 * @brief Read 16-bits from a register on a specific i2c device
 *
 * @param i2c_addr The physical address of the i2c devie (8-bit)
 * @param reg The register on the i2c device to read from.
 * @return uint8_t
 */
uint16_t i2c_read_uint16_from_reg(const uint8_t i2c_addr, const uint8_t reg)
{
    uint8_t ret[2];
    i2c_write_blocking(i2c_default, i2c_addr, &reg, 1, true);
    i2c_read_blocking(i2c_default, i2c_addr, ret, 2, false);
    return ((ret[0] << 8) | (ret[1]));
}

/**
 * @brief Read 32-bits from a register on a specific i2c device
 *
 * @param i2c_addr The physical address of the i2c devie (8-bit)
 * @param reg The register on the i2c device to read from.
 * @return uint8_t
 */
uint32_t i2c_read_uint32_from_reg(const uint8_t i2c_addr, const uint8_t reg)
{
    uint8_t ret[4];
    i2c_write_blocking(i2c_default, i2c_addr, &reg, 1, true);
    i2c_read_blocking(i2c_default, i2c_addr, ret, 4, false);
    return ((ret[0] << 24) | (ret[1] << 16) | (ret[2] << 8) | (ret[3]));
}

/**
 * @brief Read no_of_bytes from a register on a specific i2c device
 *
 * @param i2c_addr The physical address of the i2c devie (8-bit)
 * @param reg The register on the i2c device to read from.
 * @param no_of_bytes Lengt of data in bytes to receive
 * @param ret_val Pointer to buffer to receive data
 */
void i2c_read_bytes_from_reg(const uint8_t i2c_addr, const uint8_t reg, uint8_t no_of_bytes, uint8_t *ret_val)
{
    i2c_write_blocking(i2c_default, i2c_addr, &reg, 1, true);
    i2c_read_blocking(i2c_default, i2c_addr, ret_val, no_of_bytes, false);
}