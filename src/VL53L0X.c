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
        sysrange_start = i2c_read_uint8_from_reg(self->i2c_address, REG_SYSRANGE_START); //0x00
    } while (sysrange_start & 0x01);
   

    uint8_t interrupt_status = 0;
    do {
        interrupt_status = i2c_read_uint8_from_reg(self->i2c_address,REG_RESULT_INTERRUPT_STATUS); //0x13
    } while (interrupt_status & 0x01);
   

    uint16_t result = i2c_read_uint16_from_reg(self->i2c_address, REG_RESULT_RANGE_STATUS + 10); //0x14 + 10 = 0x1E

    i2c_read_uint16_from_reg(self->i2c_address,0x51);
    i2c_write_uint8_to_reg(self->i2c_address, REG_SYSTEM_INTERRUPT_CLEAR,0x01); //0x0B
   
     /*
     //8190 or 8191 may be returned when obstacle is out of range. 
    if (*range == 8190 || *range == 8191) {
        *range = VL53L0X_OUT_OF_RANGE;
    }
    */
        return result;
}




static bool configure_interrupt(VL53L0X* self)
{
    /* Interrupt on new sample ready */
    i2c_write_uint8_to_reg(self->i2c_address, REG_SYSTEM_INTERRUPT_CONFIG_GPIO, 0x04);

    /* Configure active low since the pin is pulled-up on most breakout boards */
    uint8_t gpio_hv_mux_active_high = i2c_read_uint16_from_reg(self->i2c_address,REG_GPIO_HV_MUX_ACTIVE_HIGH);
    gpio_hv_mux_active_high &= ~0x10;
    i2c_write_uint8_to_reg(self->i2c_address, REG_GPIO_HV_MUX_ACTIVE_HIGH, gpio_hv_mux_active_high);

    i2c_write_uint8_to_reg(self->i2c_address, REG_SYSTEM_INTERRUPT_CLEAR, 0x01);
    return true;
}



/**
 * Basic device initialization
 */
static bool static_init(VL53L0X* self)
{
    bool success = true;

    if (!set_spads_from_nvm()) {     //gööööööööööööööööööööööööööööööör
        return false;
    }

    if (!load_default_tuning_settings()) {    //gööööööööööööööööööööööööööööööör
        return false;
    }

    configure_interrupt(self);

    success &= i2c_write_uint8_to_reg(self->i2c_address, REG_SYSTEM_SEQUENCE_CONFIG, 
                                    RANGE_SEQUENCE_STEP_DSS +
                                    RANGE_SEQUENCE_STEP_PRE_RANGE +
                                    RANGE_SEQUENCE_STEP_FINAL_RANGE);

    return success;
}




    /**
 * Load tuning settings (same as default tuning settings provided by ST api code)
 */
static bool load_default_tuning_settings(VL53L0X* self)
{
    bool success = true;
    success &= i2c_write_uint8_to_reg(self->i2c_address, 0xFF, 0x01);
    success &= i2c_write_uint8_to_reg(self->i2c_address, 0x00, 0x00);
    success &= i2c_write_uint8_to_reg(self->i2c_address, 0xFF, 0x00);
    success &= i2c_write_uint8_to_reg(self->i2c_address, 0x09, 0x00);
    success &= i2c_write_uint8_to_reg(self->i2c_address, 0x10, 0x00);
    success &= i2c_write_uint8_to_reg(self->i2c_address, 0x11, 0x00);
    success &= i2c_write_uint8_to_reg(self->i2c_address, 0x24, 0x01);
    success &= i2c_write_uint8_to_reg(self->i2c_address, 0x25, 0xFF);
    success &= i2c_write_uint8_to_reg(self->i2c_address, 0x75, 0x00);
    success &= i2c_write_uint8_to_reg(self->i2c_address, 0xFF, 0x01);
    success &= i2c_write_uint8_to_reg(self->i2c_address, 0x4E, 0x2C);
    success &= i2c_write_uint8_to_reg(self->i2c_address, 0x48, 0x00);
    success &= i2c_write_uint8_to_reg(self->i2c_address, 0x30, 0x20);
    success &= i2c_write_uint8_to_reg(self->i2c_address, 0xFF, 0x00);
    success &= i2c_write_uint8_to_reg(self->i2c_address, 0x30, 0x09);
    success &= i2c_write_uint8_to_reg(self->i2c_address, 0x54, 0x00);
    success &= i2c_write_uint8_to_reg(self->i2c_address, 0x31, 0x04);
    success &= i2c_write_uint8_to_reg(self->i2c_address, 0x32, 0x03);
    success &= i2c_write_uint8_to_reg(self->i2c_address, 0x40, 0x83);
    success &= i2c_write_uint8_to_reg(self->i2c_address, 0x46, 0x25);
    success &= i2c_write_uint8_to_reg(self->i2c_address, 0x60, 0x00);
    success &= i2c_write_uint8_to_reg(self->i2c_address, 0x27, 0x00);
    success &= i2c_write_uint8_to_reg(self->i2c_address, 0x50, 0x06);
    success &= i2c_write_uint8_to_reg(self->i2c_address, 0x51, 0x00);
    success &= i2c_write_uint8_to_reg(self->i2c_address, 0x52, 0x96);
    success &= i2c_write_uint8_to_reg(self->i2c_address, 0x56, 0x08);
    success &= i2c_write_uint8_to_reg(self->i2c_address, 0x57, 0x30);
    success &= i2c_write_uint8_to_reg(self->i2c_address, 0x61, 0x00);
    success &= i2c_write_uint8_to_reg(self->i2c_address, 0x62, 0x00);
    success &= i2c_write_uint8_to_reg(self->i2c_address, 0x64, 0x00);
    success &= i2c_write_uint8_to_reg(self->i2c_address, 0x65, 0x00);
    success &= i2c_write_uint8_to_reg(self->i2c_address, 0x66, 0xA0);
    success &= i2c_write_uint8_to_reg(self->i2c_address, 0xFF, 0x01);
    success &= i2c_write_uint8_to_reg(self->i2c_address, 0x22, 0x32);
    success &= i2c_write_uint8_to_reg(self->i2c_address, 0x47, 0x14);
    success &= i2c_write_uint8_to_reg(self->i2c_address, 0x49, 0xFF);
    success &= i2c_write_uint8_to_reg(self->i2c_address, 0x4A, 0x00);
    success &= i2c_write_uint8_to_reg(self->i2c_address, 0xFF, 0x00);
    success &= i2c_write_uint8_to_reg(self->i2c_address, 0x7A, 0x0A);
    success &= i2c_write_uint8_to_reg(self->i2c_address, 0x7B, 0x00);
    success &= i2c_write_uint8_to_reg(self->i2c_address, 0x78, 0x21);
    success &= i2c_write_uint8_to_reg(self->i2c_address, 0xFF, 0x01);
    success &= i2c_write_uint8_to_reg(self->i2c_address, 0x23, 0x34);
    success &= i2c_write_uint8_to_reg(self->i2c_address, 0x42, 0x00);
    success &= i2c_write_uint8_to_reg(self->i2c_address, 0x44, 0xFF);
    success &= i2c_write_uint8_to_reg(self->i2c_address, 0x45, 0x26);
    success &= i2c_write_uint8_to_reg(self->i2c_address, 0x46, 0x05);
    success &= i2c_write_uint8_to_reg(self->i2c_address, 0x40, 0x40);
    success &= i2c_write_uint8_to_reg(self->i2c_address, 0x0E, 0x06);
    success &= i2c_write_uint8_to_reg(self->i2c_address, 0x20, 0x1A);
    success &= i2c_write_uint8_to_reg(self->i2c_address, 0x43, 0x40);
    success &= i2c_write_uint8_to_reg(self->i2c_address, 0xFF, 0x00);
    success &= i2c_write_uint8_to_reg(self->i2c_address, 0x34, 0x03);
    success &= i2c_write_uint8_to_reg(self->i2c_address, 0x35, 0x44);
    success &= i2c_write_uint8_to_reg(self->i2c_address, 0xFF, 0x01);
    success &= i2c_write_uint8_to_reg(self->i2c_address, 0x31, 0x04);
    success &= i2c_write_uint8_to_reg(self->i2c_address, 0x4B, 0x09);
    success &= i2c_write_uint8_to_reg(self->i2c_address, 0x4C, 0x05);
    success &= i2c_write_uint8_to_reg(self->i2c_address, 0x4D, 0x04);
    success &= i2c_write_uint8_to_reg(self->i2c_address, 0xFF, 0x00);
    success &= i2c_write_uint8_to_reg(self->i2c_address, 0x44, 0x00);
    success &= i2c_write_uint8_to_reg(self->i2c_address, 0x45, 0x20);
    success &= i2c_write_uint8_to_reg(self->i2c_address, 0x47, 0x08);
    success &= i2c_write_uint8_to_reg(self->i2c_address, 0x48, 0x28);
    success &= i2c_write_uint8_to_reg(self->i2c_address, 0x67, 0x00);
    success &= i2c_write_uint8_to_reg(self->i2c_address, 0x70, 0x04);
    success &= i2c_write_uint8_to_reg(self->i2c_address, 0x71, 0x01);
    success &= i2c_write_uint8_to_reg(self->i2c_address, 0x72, 0xFE);
    success &= i2c_write_uint8_to_reg(self->i2c_address, 0x76, 0x00);
    success &= i2c_write_uint8_to_reg(self->i2c_address, 0x77, 0x00);
    success &= i2c_write_uint8_to_reg(self->i2c_address, 0xFF, 0x01);
    success &= i2c_write_uint8_to_reg(self->i2c_address, 0x0D, 0x01);
    success &= i2c_write_uint8_to_reg(self->i2c_address, 0xFF, 0x00);
    success &= i2c_write_uint8_to_reg(self->i2c_address, 0x80, 0x01);
    success &= i2c_write_uint8_to_reg(self->i2c_address, 0x01, 0xF8);
    success &= i2c_write_uint8_to_reg(self->i2c_address, 0xFF, 0x01);
    success &= i2c_write_uint8_to_reg(self->i2c_address, 0x8E, 0x01);
    success &= i2c_write_uint8_to_reg(self->i2c_address, 0x00, 0x01);
    success &= i2c_write_uint8_to_reg(self->i2c_address, 0xFF, 0x00);
    success &= i2c_write_uint8_to_reg(self->i2c_address, 0x80, 0x00);
    return success;
}    

  
  
