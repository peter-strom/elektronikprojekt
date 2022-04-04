#ifndef VL53L0X_H_
#define VL53L0X_H_
#include "header.h"
//https://github.com/artfulbytes/vl6180x_vl53l0x_msp430/blob/main/drivers/i2c.c

#define VL53L0X_OUT_OF_RANGE (8190)


/* Comment these out if not connected */
#define VL53L0X_SECOND
#define VL53L0X_THIRD

typedef enum
{
    VL53L0X_IDX_FIRST,
#ifdef VL53L0X_SECOND
    VL53L0X_IDX_SECOND,
#endif
#ifdef VL53L0X_THIRD
    VL53L0X_IDX_THIRD,
#endif
} vl53l0x_idx_t;


/**
 * Initializes the sensors in the vl53l0x_idx_t enum.
 * @note Each sensor must have its XSHUT pin connected.
 */
bool vl53l0x_init(void);

/**
 * Does a single range measurement
 * @param idx selects specific sensor
 * @param range contains the measured range or VL53L0X_OUT_OF_RANGE
 *        if out of range.
 * @return True if success, False if error
 * @note   Polling-based
 */
bool vl53l0x_read_range_single(vl53l0x_idx_t idx, uint16_t *range);

bool device_is_booted();

#endif /* VL53L0X_H_ */