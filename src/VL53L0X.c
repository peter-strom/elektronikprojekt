#include "VL53L0X.h"

static void data_init(VL53L0X *self);
static bool device_is_booted(VL53L0X *self);
static bool configure_interrupt(VL53L0X *self);
static bool static_init(VL53L0X *self);
static bool set_spads_from_nvm(VL53L0X *self);
static bool get_spad_info_from_nvm(VL53L0X *self, uint8_t *spad_count, uint8_t *spad_type, uint8_t good_spad_map[6]);
static bool read_strobe(VL53L0X *self);
static bool load_default_tuning_settings(VL53L0X *self);
static bool perform_single_ref_calibration(VL53L0X *self, calibration_type_t calib_type);
static bool perform_ref_calibration(VL53L0X *self);

VL53L0X new_VL53L0X(uint8_t xshut_gpio)
{
    VL53L0X self;
    self.i2c_address = 0x29;
    self.xshut_gpio = xshut_gpio;
    self.stop_variable = 0x00;
    // because the sensors use same i2c address (0x29)
    //  we have to turn off all connected sensor by default
    //  and activate them one by one while assigning the new address.
    gpio_init(xshut_gpio);
    gpio_set_dir(xshut_gpio, GPIO_OUT);
    gpio_put(xshut_gpio, false);

    return self;
}

/**
 * Device initialization
 * return stop_variable
 */
static void data_init(VL53L0X *self)
{
    /* Set 2v8 mode */
    uint8_t vhv_config_scl_sda = i2c_read_uint8_from_reg(self->i2c_address, REG_VHV_CONFIG_PAD_SCL_SDA_EXTSUP_HV);
    vhv_config_scl_sda |= 0x01;
    i2c_write_uint8_to_reg(self->i2c_address, REG_VHV_CONFIG_PAD_SCL_SDA_EXTSUP_HV, vhv_config_scl_sda);

    /* Set I2C standard mode */
    i2c_write_uint8_to_reg(self->i2c_address, 0x88, 0x00);
    i2c_write_uint8_to_reg(self->i2c_address, 0x80, 0x01);
    i2c_write_uint8_to_reg(self->i2c_address, 0xFF, 0x01);
    i2c_write_uint8_to_reg(self->i2c_address, 0x00, 0x00);
    self->stop_variable = i2c_read_uint8_from_reg(self->i2c_address, 0x91);
    i2c_write_uint8_to_reg(self->i2c_address, 0x00, 0x01);
    i2c_write_uint8_to_reg(self->i2c_address, 0xFF, 0x00);
    i2c_write_uint8_to_reg(self->i2c_address, 0x80, 0x00);
}

/**
 * Simple function to check if device is booted
 * REF_REG is a referense register according to datsheet
 */
static bool device_is_booted(VL53L0X *self)
{
    if (!i2c_read_uint8_from_reg(self->i2c_address, REF_REG))
    {
        return false;
    }
    return true;
}

/* The fuction assigns a new i2c-adress to the sensor.
 * all other sensors attached need to be disabled by the xshut-pin.
 * */
bool assign_new_address(VL53L0X *self, uint8_t new_i2c_address)
{
    // sleep_ms(300);
    // wake up sensor
    gpio_put(self->xshut_gpio, true);
    // wait till sensor starts
    // while (!device_is_booted(self)) {
    // }
    sleep_ms(300);

    // 7F == 0111 1111 -> 7-bit register
    if (!i2c_write_uint8_to_reg(self->i2c_address, REG_SLAVE_DEVICE_ADDRESS, new_i2c_address & 0x7F))
    {
        return false;
    }
    self->i2c_address = new_i2c_address;
    data_init(self);
    static_init(self);
    perform_ref_calibration(self);
    return true;
}

// read range single
uint16_t read_range(VL53L0X *self)
{

    i2c_write_uint8_to_reg(self->i2c_address, 0x80, 0x01);
    i2c_write_uint8_to_reg(self->i2c_address, 0xFF, 0x01);
    i2c_write_uint8_to_reg(self->i2c_address, 0x00, 0x00);
    i2c_write_uint8_to_reg(self->i2c_address, 0x91, self->stop_variable);
    i2c_write_uint8_to_reg(self->i2c_address, 0x00, 0x01);
    i2c_write_uint8_to_reg(self->i2c_address, 0xFF, 0x00);
    i2c_write_uint8_to_reg(self->i2c_address, 0x80, 0x00);
    i2c_write_uint8_to_reg(self->i2c_address, REG_SYSRANGE_START, 0x01);

    uint8_t sysrange_start = 0;
    do
    {
        sysrange_start = i2c_read_uint8_from_reg(self->i2c_address, REG_SYSRANGE_START); // 0x00
    } while (sysrange_start & 0x01);

    uint8_t interrupt_status = 0;
    do
    {
        interrupt_status = i2c_read_uint8_from_reg(self->i2c_address, REG_RESULT_INTERRUPT_STATUS); // 0x13
    } while (interrupt_status & 0x01);

    uint16_t result = i2c_read_uint16_from_reg(self->i2c_address, REG_RESULT_RANGE_STATUS + 10); // 0x14 + 10 = 0x1E

    i2c_read_uint16_from_reg(self->i2c_address, 0x51);
    i2c_write_uint8_to_reg(self->i2c_address, REG_SYSTEM_INTERRUPT_CLEAR, 0x01); // 0x0B

    /*
    //8190 or 8191 may be returned when obstacle is out of range.
   if (*range == 8190 || *range == 8191) {
       *range = VL53L0X_OUT_OF_RANGE;
   }
   */
    return result;
}

static bool configure_interrupt(VL53L0X *self)
{
    /* Interrupt on new sample ready */
    i2c_write_uint8_to_reg(self->i2c_address, REG_SYSTEM_INTERRUPT_CONFIG_GPIO, 0x04);

    /* Configure active low since the pin is pulled-up on most breakout boards */
    uint8_t gpio_hv_mux_active_high = i2c_read_uint16_from_reg(self->i2c_address, REG_GPIO_HV_MUX_ACTIVE_HIGH);
    gpio_hv_mux_active_high &= ~0x10;
    i2c_write_uint8_to_reg(self->i2c_address, REG_GPIO_HV_MUX_ACTIVE_HIGH, gpio_hv_mux_active_high);

    i2c_write_uint8_to_reg(self->i2c_address, REG_SYSTEM_INTERRUPT_CLEAR, 0x01);
    return true;
}

/**
 * Basic device initialization
 */

static bool static_init(VL53L0X *self)
{
    bool success = true;

    set_spads_from_nvm(self);

    load_default_tuning_settings(self);

    configure_interrupt(self);

    success &= i2c_write_uint8_to_reg(self->i2c_address, REG_SYSTEM_SEQUENCE_CONFIG,
                                      RANGE_SEQUENCE_STEP_DSS +
                                          RANGE_SEQUENCE_STEP_PRE_RANGE +
                                          RANGE_SEQUENCE_STEP_FINAL_RANGE);

    return success;
}

/**
 * Sets the SPADs according to the value saved to NVM by ST during production. Assuming
 * similar conditions (e.g. no cover glass), this should give reasonable readings and we
 * can avoid running ref spad management (tedious code).
 */

static bool set_spads_from_nvm(VL53L0X *self)
{
    uint8_t spad_map[SPAD_MAP_ROW_COUNT] = {0};
    uint8_t good_spad_map[SPAD_MAP_ROW_COUNT] = {0};
    uint8_t spads_enabled_count = 0;
    uint8_t spads_to_enable_count = 0;
    uint8_t spad_type = 0;
    volatile uint32_t total_val = 0;
    bool success = true;

    if (!get_spad_info_from_nvm(self, &spads_to_enable_count, &spad_type, good_spad_map))
    {
        return false;
    }

    for (int i = 0; i < 6; i++)
    {
        total_val += good_spad_map[i];
    }

    success &= i2c_write_uint8_to_reg(self->i2c_address, 0xFF, 0x01);
    success &= i2c_write_uint8_to_reg(self->i2c_address, REG_DYNAMIC_SPAD_REF_EN_START_OFFSET, 0x00);
    success &= i2c_write_uint8_to_reg(self->i2c_address, REG_DYNAMIC_SPAD_NUM_REQUESTED_REF_SPAD, 0x2C);
    success &= i2c_write_uint8_to_reg(self->i2c_address, 0xFF, 0x00);
    success &= i2c_write_uint8_to_reg(self->i2c_address, REG_GLOBAL_CONFIG_REF_EN_START_SELECT, SPAD_START_SELECT);
    if (!success)
    {
        return false;
    }

    uint8_t offset = (spad_type == SPAD_TYPE_APERTURE) ? SPAD_APERTURE_START_INDEX : 0;

    /* Create a new SPAD array by selecting a subset of the SPADs suggested by the good SPAD map.
     * The subset should only have the number of type enabled as suggested by the reading from
     * the NVM (spads_to_enable_count and spad_type). */
    for (int row = 0; row < SPAD_MAP_ROW_COUNT; row++)
    {
        for (int column = 0; column < SPAD_ROW_SIZE; column++)
        {
            int index = (row * SPAD_ROW_SIZE) + column;
            if (index >= SPAD_MAX_COUNT)
            {
                return false;
            }
            if (spads_enabled_count == spads_to_enable_count)
            {
                /* We are done */
                break;
            }
            if (index < offset)
            {
                continue;
            }
            if ((good_spad_map[row] >> column) & 0x1)
            {
                spad_map[row] |= (1 << column);
                spads_enabled_count++;
            }
        }
        if (spads_enabled_count == spads_to_enable_count)
        {
            /* To avoid looping unnecessarily when we are already done. */
            break;
        }
    }

    if (spads_enabled_count != spads_to_enable_count)
    {
        return false;
    }

    /* Write the new SPAD configuration */
    // original :  i2c_write_addr8_bytes(REG_GLOBAL_CONFIG_SPAD_ENABLES_REF_0, spad_map, SPAD_MAP_ROW_COUNT)
    for (uint8_t i = 0; i < SPAD_MAP_ROW_COUNT; i++)
    {
        i2c_write_uint8_to_reg(self->i2c_address, REG_GLOBAL_CONFIG_SPAD_ENABLES_REF_0 + i, spad_map[i]);
    }

    return true;
}

/**
 * Gets the spad count, spad type och "good" spad map stored by ST in NVM at
 * their production line.
 * .
 * According to the datasheet, ST runs a calibration (without cover glass) and
 * saves a "good" SPAD map to NVM (non volatile memory). The SPAD array has two
 * types of SPADs: aperture and non-aperture. By default, all of the
 * good SPADs are enabled, but we should only enable a subset of them to get
 * an optimized signal rate. We should also only enable either only aperture
 * or only non-aperture SPADs. The number of SPADs to enable and which type
 * are also saved during the calibration step at ST factory and can be retrieved
 * from NVM.
 */

static bool get_spad_info_from_nvm(VL53L0X *self, uint8_t *spad_count, uint8_t *spad_type, uint8_t good_spad_map[6])
{
    bool success = true;
    uint8_t tmp_data8 = 0;
    uint32_t tmp_data32 = 0;

    /* Setup to read from NVM */
    success &= i2c_write_uint8_to_reg(self->i2c_address, 0x80, 0x01);
    success &= i2c_write_uint8_to_reg(self->i2c_address, 0xFF, 0x01);
    success &= i2c_write_uint8_to_reg(self->i2c_address, 0x00, 0x00);
    success &= i2c_write_uint8_to_reg(self->i2c_address, 0xFF, 0x06);
    tmp_data8 = i2c_read_uint8_from_reg(self->i2c_address, 0x83);
    success &= i2c_write_uint8_to_reg(self->i2c_address, 0x83, tmp_data8 | 0x04);
    success &= i2c_write_uint8_to_reg(self->i2c_address, 0xFF, 0x07);
    success &= i2c_write_uint8_to_reg(self->i2c_address, 0x81, 0x01);
    success &= i2c_write_uint8_to_reg(self->i2c_address, 0x80, 0x01);
    if (!success)
    {
        return false;
    }

    /* Get the SPAD count and type */
    success &= i2c_write_uint8_to_reg(self->i2c_address, 0x94, 0x6b);
    if (!success)
    {
        return false;
    }
    if (!read_strobe(self))
    {
        return false;
    }
    tmp_data32 = i2c_read_uint32_from_reg(self->i2c_address, 0x90);
    if (!success)
    {
        return false;
    }
    *spad_count = (tmp_data32 >> 8) & 0x7f;
    *spad_type = (tmp_data32 >> 15) & 0x01;

    /* Since the good SPAD map is already stored in REG_GLOBAL_CONFIG_SPAD_ENABLES_REF_0
     * we can simply read that register instead of doing the below */
#if 0
    /* Get the first part of the SPAD map */
    if (!i2c_write_uint8_to_reg(self->i2c_address, 0x94, 0x24)) {
        return false;
    }
    if (!read_strobe()) {
        return false;
    }
    if (!i2c_read_addr8_data32(0x90, &tmp_data32)) {
      return false;
    }
    good_spad_map[0] = (uint8_t)((tmp_data32 >> 24) & 0xFF);
    good_spad_map[1] = (uint8_t)((tmp_data32 >> 16) & 0xFF);
    good_spad_map[2] = (uint8_t)((tmp_data32 >> 8) & 0xFF);
    good_spad_map[3] = (uint8_t)(tmp_data32 & 0xFF);

    /* Get the second part of the SPAD map */
    if (!i2c_write_uint8_to_reg(self->i2c_address, 0x94, 0x25)) {
        return false;
    }
    if (!read_strobe()) {
        return false;
    }
    if (!i2c_read_addr8_data32(0x90, &tmp_data32)) {
        return false;
    }
    good_spad_map[4] = (uint8_t)((tmp_data32 >> 24) & 0xFF);
    good_spad_map[5] = (uint8_t)((tmp_data32 >> 16) & 0xFF);

#endif

    /* Restore after reading from NVM */
    success &= i2c_write_uint8_to_reg(self->i2c_address, 0x81, 0x00);
    success &= i2c_write_uint8_to_reg(self->i2c_address, 0xFF, 0x06);
    tmp_data8 = i2c_read_uint8_from_reg(self->i2c_address, 0x83);
    success &= i2c_write_uint8_to_reg(self->i2c_address, 0x83, tmp_data8 & 0xfb);
    success &= i2c_write_uint8_to_reg(self->i2c_address, 0xFF, 0x01);
    success &= i2c_write_uint8_to_reg(self->i2c_address, 0x00, 0x01);
    success &= i2c_write_uint8_to_reg(self->i2c_address, 0xFF, 0x00);
    success &= i2c_write_uint8_to_reg(self->i2c_address, 0x80, 0x00);

    /* When we haven't configured the SPAD map yet, the SPAD map register actually
     * contains the good SPAD map, so we can retrieve it straight from this register
     * instead of reading it from the NVM. */

    i2c_read_bytes_from_reg(self->i2c_address, REG_GLOBAL_CONFIG_SPAD_ENABLES_REF_0, SPAD_MAP_ROW_COUNT, good_spad_map);
    return success;
}

/**
 * Wait for strobe value to be set. This is used when we read values
 * from NVM (non volatile memory).
 */

static bool read_strobe(VL53L0X *self)
{

    bool success = false;
    uint8_t strobe = 0;

    i2c_write_uint8_to_reg(self->i2c_address, 0x83, 0x00);

    while (!i2c_read_uint8_from_reg(self->i2c_address, 0x83))
        ;

    i2c_write_uint8_to_reg(self->i2c_address, 0x83, 0x01);

    return true;
}

/**
 * Load tuning settings (same as default tuning settings provided by ST api code)
 */

static bool load_default_tuning_settings(VL53L0X *self)
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

static bool perform_single_ref_calibration(VL53L0X *self, calibration_type_t calib_type)
{
    uint8_t sysrange_start = 0;
    uint8_t sequence_config = 0;
    switch (calib_type)
    {
    case CALIBRATION_TYPE_VHV:
        sequence_config = 0x01;
        sysrange_start = 0x01 | 0x40;
        break;
    case CALIBRATION_TYPE_PHASE:
        sequence_config = 0x02;
        sysrange_start = 0x01 | 0x00;
        break;
    }
    if (!i2c_write_uint8_to_reg(self->i2c_address, REG_SYSTEM_SEQUENCE_CONFIG, sequence_config))
    {
        return false;
    }
    if (!i2c_write_uint8_to_reg(self->i2c_address, REG_SYSRANGE_START, sysrange_start))
    {
        return false;
    }
    // Wait for interrupt
    uint8_t interrupt_status = 0;

    do
    {
        interrupt_status = i2c_read_uint8_from_reg(self->i2c_address, REG_RESULT_INTERRUPT_STATUS);
    } while ((interrupt_status & 0x07) == 0);

    if (!i2c_write_uint8_to_reg(self->i2c_address, REG_SYSTEM_INTERRUPT_CLEAR, 0x01))
    {
        return false;
    }

    if (!i2c_write_uint8_to_reg(self->i2c_address, REG_SYSRANGE_START, 0x00))
    {
        return false;
    }
    return true;
}

/**
 * Temperature calibration needs to be run again if the temperature changes by
 * more than 8 degrees according to the datasheet.
 */

static bool perform_ref_calibration(VL53L0X *self)
{
    if (!perform_single_ref_calibration(self, CALIBRATION_TYPE_VHV))
    {
        return false;
    }
    if (!perform_single_ref_calibration(self, CALIBRATION_TYPE_PHASE))
    {
        return false;
    }
    // Restore sequence steps enabled
    if (!i2c_write_uint8_to_reg(self->i2c_address, REG_SYSTEM_SEQUENCE_CONFIG,
                                RANGE_SEQUENCE_STEP_DSS +
                                    RANGE_SEQUENCE_STEP_PRE_RANGE +
                                    RANGE_SEQUENCE_STEP_FINAL_RANGE))
    {
        return false;
    }

    return true;
}
