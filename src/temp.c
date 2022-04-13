
/**
 * Wait for strobe value to be set. This is used when we read values
 * from NVM (non volatile memory).
 */
static bool read_strobe()
{
    bool success = false;
    uint8_t strobe = 0;
    if (!i2c_write_addr8_data8(0x83, 0x00)) {
        return false;
    }
    do {
        success = i2c_read_addr8_data8(0x83, &strobe);
    } while (success && (strobe == 0));
    if (!success) {
        return false;
    }
    if (!i2c_write_addr8_data8(0x83, 0x01)) {
        return false;
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
static bool get_spad_info_from_nvm(uint8_t *spad_count, uint8_t *spad_type, uint8_t good_spad_map[6])
{
    bool success = true;
    uint8_t tmp_data8 = 0;
    uint32_t tmp_data32 = 0;

    /* Setup to read from NVM */
    success &= i2c_write_addr8_data8(0x80, 0x01);
    success &= i2c_write_addr8_data8(0xFF, 0x01);
    success &= i2c_write_addr8_data8(0x00, 0x00);
    success &= i2c_write_addr8_data8(0xFF, 0x06);
    success &= i2c_read_addr8_data8(0x83, &tmp_data8);
    success &= i2c_write_addr8_data8(0x83, tmp_data8 | 0x04);
    success &= i2c_write_addr8_data8(0xFF, 0x07);
    success &= i2c_write_addr8_data8(0x81, 0x01);
    success &= i2c_write_addr8_data8(0x80, 0x01);
    if (!success) {
      return false;
    }

    /* Get the SPAD count and type */
    success &= i2c_write_addr8_data8(0x94, 0x6b);
    if (!success) {
        return false;
    }
    if (!read_strobe()) {
        return false;
    }
    success &= i2c_read_addr8_data32(0x90, &tmp_data32);
    if (!success) {
        return false;
    }
    *spad_count = (tmp_data32 >> 8) & 0x7f;
    *spad_type = (tmp_data32 >> 15) & 0x01;

    /* Since the good SPAD map is already stored in REG_GLOBAL_CONFIG_SPAD_ENABLES_REF_0
     * we can simply read that register instead of doing the below */
#if 0
    /* Get the first part of the SPAD map */
    if (!i2c_write_addr8_data8(0x94, 0x24)) {
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
    if (!i2c_write_addr8_data8(0x94, 0x25)) {
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
    success &=i2c_write_addr8_data8(0x81, 0x00);
    success &=i2c_write_addr8_data8(0xFF, 0x06);
    success &=i2c_read_addr8_data8(0x83, &tmp_data8);
    success &=i2c_write_addr8_data8(0x83, tmp_data8 & 0xfb);
    success &=i2c_write_addr8_data8(0xFF, 0x01);
    success &=i2c_write_addr8_data8(0x00, 0x01);
    success &=i2c_write_addr8_data8(0xFF, 0x00);
    success &=i2c_write_addr8_data8(0x80, 0x00);

    /* When we haven't configured the SPAD map yet, the SPAD map register actually
     * contains the good SPAD map, so we can retrieve it straight from this register
     * instead of reading it from the NVM. */
    if (!i2c_read_addr8_bytes(REG_GLOBAL_CONFIG_SPAD_ENABLES_REF_0, good_spad_map, 6)) {
        return false;
    }
    return success;
}

/**
 * Sets the SPADs according to the value saved to NVM by ST during production. Assuming
 * similar conditions (e.g. no cover glass), this should give reasonable readings and we
 * can avoid running ref spad management (tedious code).
 */
static bool set_spads_from_nvm()
{
    uint8_t spad_map[SPAD_MAP_ROW_COUNT] = { 0 };
    uint8_t good_spad_map[SPAD_MAP_ROW_COUNT] = { 0 };
    uint8_t spads_enabled_count = 0;
    uint8_t spads_to_enable_count = 0;
    uint8_t spad_type = 0;
    volatile uint32_t total_val = 0;

    if (!get_spad_info_from_nvm(&spads_to_enable_count, &spad_type, good_spad_map)) {
        return false;
    }

    for (int i = 0; i < 6; i++) {
        total_val += good_spad_map[i];
    }

    bool success = i2c_write_addr8_data8(0xFF, 0x01);
    success &= i2c_write_addr8_data8(REG_DYNAMIC_SPAD_REF_EN_START_OFFSET, 0x00);
    success &= i2c_write_addr8_data8(REG_DYNAMIC_SPAD_NUM_REQUESTED_REF_SPAD, 0x2C);
    success &= i2c_write_addr8_data8(0xFF, 0x00);
    success &= i2c_write_addr8_data8(REG_GLOBAL_CONFIG_REF_EN_START_SELECT, SPAD_START_SELECT);
    if (!success) {
        return false;
    }

    uint8_t offset = (spad_type == SPAD_TYPE_APERTURE) ? SPAD_APERTURE_START_INDEX : 0;

    /* Create a new SPAD array by selecting a subset of the SPADs suggested by the good SPAD map.
     * The subset should only have the number of type enabled as suggested by the reading from
     * the NVM (spads_to_enable_count and spad_type). */
    for (int row = 0; row < SPAD_MAP_ROW_COUNT; row++) {
        for (int column = 0; column < SPAD_ROW_SIZE; column++) {
            int index = (row * SPAD_ROW_SIZE) + column;
            if (index >= SPAD_MAX_COUNT) {
                return false;
            }
            if (spads_enabled_count == spads_to_enable_count) {
                /* We are done */
                break;
            }
            if (index < offset) {
                continue;
            }
            if ((good_spad_map[row] >> column) & 0x1) {
                spad_map[row] |= (1 << column);
                spads_enabled_count++;
            }
        }
        if (spads_enabled_count == spads_to_enable_count) {
            /* To avoid looping unnecessarily when we are already done. */
            break;
        }
    }

    if (spads_enabled_count != spads_to_enable_count) {
        return false;
    }

    /* Write the new SPAD configuration */
    if (!i2c_write_addr8_bytes(REG_GLOBAL_CONFIG_SPAD_ENABLES_REF_0, spad_map, SPAD_MAP_ROW_COUNT)) {
        return false;
    }

    return true;
}





static bool perform_single_ref_calibration(calibration_type_t calib_type)
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
    if (!i2c_write_addr8_data8(REG_SYSTEM_SEQUENCE_CONFIG, sequence_config)) {
        return false;
    }
    if (!i2c_write_addr8_data8(REG_SYSRANGE_START, sysrange_start)) {
        return false;
    }
    /* Wait for interrupt */
    uint8_t interrupt_status = 0;
    bool success = false;
    do {
        success = i2c_read_addr8_data8(REG_RESULT_INTERRUPT_STATUS, &interrupt_status);
    } while (success && ((interrupt_status & 0x07) == 0));
    if (!success) {
        return false;
    }
    if (!i2c_write_addr8_data8(REG_SYSTEM_INTERRUPT_CLEAR, 0x01)) {
        return false;
    }

    if (!i2c_write_addr8_data8(REG_SYSRANGE_START, 0x00)) {
        return false;
    }
    return true;
}

/**
 * Temperature calibration needs to be run again if the temperature changes by
 * more than 8 degrees according to the datasheet.
 */
static bool perform_ref_calibration()
{
    if (!perform_single_ref_calibration(CALIBRATION_TYPE_VHV)) {
        return false;
    }
    if (!perform_single_ref_calibration(CALIBRATION_TYPE_PHASE)) {
        return false;
    }
    /* Restore sequence steps enabled */
    if (!set_sequence_steps_enabled(RANGE_SEQUENCE_STEP_DSS +
                                    RANGE_SEQUENCE_STEP_PRE_RANGE +
                                    RANGE_SEQUENCE_STEP_FINAL_RANGE)) {
        return false;
    }
    return true;
}





