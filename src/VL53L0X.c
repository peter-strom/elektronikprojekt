#include "VL53L0X.h"



static void data_init(VL53L0X* self);

VL53L0X new_VL53L0X(uint8_t xshut_gpio)
{
    VL53L0X self;
    self.i2c_address = 0x29;
    self.xshut_gpio = xshut_gpio;
    self.stop_variable = 0x00;
    //because the sensors use same i2c address (0x29)
    // we have to turn off all connected sensor by default
    // and activate them one by one while assigning the new address.
    gpio_put(xshut_gpio, true); 
    data_init(&self);
    return self;
}


/**
 * Device initialization
 * return stop_variable
 */
static void data_init(VL53L0X* self)
{
    /* Set 2v8 mode */
    uint8_t vhv_config_scl_sda = i2c_read_uint8_from_reg(self->i2c_address,REG_VHV_CONFIG_PAD_SCL_SDA_EXTSUP_HV);
    vhv_config_scl_sda |= 0x01;
    i2c_write_uint8_to_reg(self->i2c_address,REG_VHV_CONFIG_PAD_SCL_SDA_EXTSUP_HV,vhv_config_scl_sda);

    /* Set I2C standard mode */
    i2c_write_uint8_to_reg(self->i2c_address,0x88, 0x00);
    i2c_write_uint8_to_reg(self->i2c_address,0x80, 0x01);
    i2c_write_uint8_to_reg(self->i2c_address,0xFF, 0x01);
    i2c_write_uint8_to_reg(self->i2c_address,0x00, 0x00);
    self->stop_variable = i2c_read_uint8_from_reg(self->i2c_address,0x91);
    i2c_write_uint8_to_reg(self->i2c_address,0x00, 0x01);
    i2c_write_uint8_to_reg(self->i2c_address,0xFF, 0x00);
    i2c_write_uint8_to_reg(self->i2c_address,0x80, 0x00);
}

/**
 * Simple function to check if device is booted
 * REF_REG is a referense register according to datsheet
 */
static bool device_is_booted(VL53L0X* self)
{
    if (!i2c_read_uint8_from_reg(self->i2c_address,REF_REG)) {
        return false;
    }
    return true;
}

/* The fuction assigns a new i2c-adress to the sensor.
 * all other sensors attached need to be disabled by the xshut-pin. 
 * */
bool assign_new_address(VL53L0X* self, uint8_t new_i2c_address)
{
    //wake up sensor
    gpio_put(self->xshut_gpio, false); 
    //wait till sensor starts
    while (!device_is_booted(self)) {
    }
    // 7F == 0111 1111 -> 7-bit register
    if (i2c_write_uint8_to_reg(self->i2c_address,REG_SLAVE_DEVICE_ADDRESS, new_i2c_address & 0x7F)) {
        return false;
    }
    return true;
}
//read range single 
uint16_t read_range(VL53L0X* self)
{
  
    i2c_write_uint8_to_reg(self->i2c_address,0x80, 0x01);
    i2c_write_uint8_to_reg(self->i2c_address,0xFF, 0x01);
    i2c_write_uint8_to_reg(self->i2c_address,0x00, 0x00);
    i2c_write_uint8_to_reg(self->i2c_address,0x91, self->stop_variable);
    i2c_write_uint8_to_reg(self->i2c_address,0x00, 0x01);
    i2c_write_uint8_to_reg(self->i2c_address,0xFF, 0x00);
    i2c_write_uint8_to_reg(self->i2c_address,0x80, 0x00);
   

    i2c_write_uint8_to_reg(self->i2c_address,REG_SYSRANGE_START, 0x01)


    uint8_t sysrange_start = 0;
    do {
        sysrange_start = i2c_read_uint8_from_reg(VL53L0X_ADDRESS, REG_SYSRANGE_START); //0x00
    } while (sysrange_start & 0x01);
   

    uint8_t interrupt_status = 0;
    do {
        interrupt_status = i2c_read_uint8_from_reg(VL53L0X_ADDRESS,REG_RESULT_INTERRUPT_STATUS); //0x13
    } while (interrupt_status & 0x01);
   

    uint16_t resul = i2c_read_uint16_from_reg(VL53L0X_ADDRESS, REG_RESULT_RANGE_STATUS + 10); //0x14 + 10 = 0x1E

      i2c_read_uint16_from_reg(VL53L0X_ADDRESS,0x51);

 

     i2c_write_uint8_to_reg(VL53L0X_ADDRESS, REG_SYSTEM_INTERRUPT_CLEAR,0x01); //0x0B
   
        return resul;

        

    uint8_t sysrange_start = 0;

    do {
        success = i2c_read_addr8_data8(REG_SYSRANGE_START, &sysrange_start);
    } while (success && (sysrange_start & 0x01));
    if (!success) {
        return false;
    }

    uint8_t interrupt_status = 0;
    do {
        success = i2c_read_addr8_data8(REG_RESULT_INTERRUPT_STATUS, &interrupt_status);
    } while (success && ((interrupt_status & 0x07) == 0));
    if (!success) {
        return false;
    }

    if (!i2c_read_addr8_data16(REG_RESULT_RANGE_STATUS + 10, range)) {
        return false;
    }

    if (!i2c_write_addr8_data8(REG_SYSTEM_INTERRUPT_CLEAR, 0x01)) {
        return false;
    }

    /* 8190 or 8191 may be returned when obstacle is out of range. */
    if (*range == 8190 || *range == 8191) {
        *range = VL53L0X_OUT_OF_RANGE;
    }

    return true;
}