#include "header.h"



#define REG_IDENTIFICATION_MODEL_ID (0xC0)
#define REG_VHV_CONFIG_PAD_SCL_SDA_EXTSUP_HV (0x89)
#define REG_MSRC_CONFIG_CONTROL (0x60)
#define REG_FINAL_RANGE_CONFIG_MIN_COUNT_RATE_RTN_LIMIT (0x44)
#define REG_SYSTEM_SEQUENCE_CONFIG (0x01)
#define REG_DYNAMIC_SPAD_REF_EN_START_OFFSET (0x4F)
#define REG_DYNAMIC_SPAD_NUM_REQUESTED_REF_SPAD (0x4E)
#define REG_GLOBAL_CONFIG_REF_EN_START_SELECT (0xB6)
#define REG_SYSTEM_INTERRUPT_CONFIG_GPIO (0x0A)
#define REG_GPIO_HV_MUX_ACTIVE_HIGH (0x84)
#define REG_SYSTEM_INTERRUPT_CLEAR (0x0B)
#define REG_RESULT_INTERRUPT_STATUS (0x13)
#define REG_SYSRANGE_START (0x00)
#define REG_GLOBAL_CONFIG_SPAD_ENABLES_REF_0 (0xB0)
#define REG_RESULT_RANGE_STATUS (0x14)
#define REG_SLAVE_DEVICE_ADDRESS (0x8A)

#define RANGE_SEQUENCE_STEP_TCC (0x10) /* Target CentreCheck */
#define RANGE_SEQUENCE_STEP_MSRC (0x04) /* Minimum Signal Rate Check */
#define RANGE_SEQUENCE_STEP_DSS (0x28) /* Dynamic SPAD selection */
#define RANGE_SEQUENCE_STEP_PRE_RANGE (0x40)
#define RANGE_SEQUENCE_STEP_FINAL_RANGE (0x80)

#define VL53L0X_EXPECTED_DEVICE_ID (0xEE)
#define VL53L0X_DEFAULT_ADDRESS (0x29)
// 0x91 0xFF whot?
 


static int addr = 0x29; //0x29 , 0x68

#ifdef i2c_default
static void mpu6050_reset() {
    // Two byte reset. First byte register, second byte data
    // There are a load more options to set up the device in different ways that could be added here
    uint8_t buf[] = {0x6B, 0x00};
    i2c_write_blocking(i2c_default, addr, buf, 2, false);
}



static uint16_t VL53L0X_read(void)
{
    /**************
     adress: 0x80   0xFF    0x00    0x91    0x00    0xFF    0x80    
      value: 0x01   0x01    0x00    stopvar 0x01    0x00    0x01  
     * ************/

  
    uint8_t stop_variable = i2c_read_uint8_from_reg(VL53L0X_ADDRESS,0x91);
      
    i2c_write_uint8_to_reg(VL53L0X_ADDRESS,0x80,0x00);
    i2c_write_uint8_to_reg(VL53L0X_ADDRESS,0xFF,0x01);
    i2c_write_uint8_to_reg(VL53L0X_ADDRESS,0x00,0x00);
    i2c_write_uint8_to_reg(VL53L0X_ADDRESS,0x91,stop_variable);
    i2c_write_uint8_to_reg(VL53L0X_ADDRESS,0x00,0x01);
    i2c_write_uint8_to_reg(VL53L0X_ADDRESS,0xFF,0x00);
    i2c_write_uint8_to_reg(VL53L0X_ADDRESS,0x80,0x01);
  

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
  


   
}

static void mpu6050_read_raw(int16_t accel[3], int16_t gyro[3], int16_t *temp) {
    // For this particular device, we send the device the register we want to read
    // first, then subsequently read from the device. The register is auto incrementing
    // so we don't need to keep sending the register we want, just the first.

    uint8_t buffer[6];

    // Start reading acceleration registers from register 0x3B for 6 bytes
    uint8_t val = 0x3B;
    i2c_write_blocking(i2c_default, addr, &val, 1, true); // true to keep master control of bus
    i2c_read_blocking(i2c_default, addr, buffer, 6, false);

    for (int i = 0; i < 3; i++) {
        accel[i] = (buffer[i * 2] << 8 | buffer[(i * 2) + 1]);
    }

    // Now gyro data from reg 0x43 for 6 bytes
    // The register is auto incrementing on each read
    val = 0x43;
    i2c_write_blocking(i2c_default, addr, &val, 1, true);
    i2c_read_blocking(i2c_default, addr, buffer, 6, false);  // False - finished with bus

    for (int i = 0; i < 3; i++) {
        gyro[i] = (buffer[i * 2] << 8 | buffer[(i * 2) + 1]);;
    }

    // Now temperature from reg 0x41 for 2 bytes
    // The register is auto incrementing on each read
    val = 0x41;
    i2c_write_blocking(i2c_default, addr, &val, 1, true);
    i2c_read_blocking(i2c_default, addr, buffer, 2, false);  // False - finished with bus

    *temp = buffer[0] << 8 | buffer[1];
}
#endif

int main() {
    stdio_init_all();
    init_i2c();
#if !defined(i2c_default) || !defined(PICO_DEFAULT_I2C_SDA_PIN) || !defined(PICO_DEFAULT_I2C_SCL_PIN)
    #warning i2c/mpu6050_i2c example requires a board with I2C pins
    puts("Default I2C pins were not defined");
#else
    printf("Hello, MPU6050! Reading raw data from registers...\n");

    // This example will use I2C0 on the default SDA and SCL pins (4, 5 on a Pico)
   
   //mpu6050_reset();

    int16_t acceleration[3], gyro[3], temp, distance;

    while (1) {
     distance = VL53L0X_read();
      printf("distance: %d\n", distance);


       //mpu6050_read_raw(acceleration, gyro, &temp);

        // These are the raw numbers from the chip, so will need tweaking to be really useful.
        // See the datasheet for more information
     //printf("Acc. X = %d, Y = %d, Z = %d\n", acceleration[0], acceleration[1], acceleration[2]);
      //  printf("Gyro. X = %d, Y = %d, Z = %d\n", gyro[0], gyro[1], gyro[2]);
        // Temperature is simple so use the datasheet calculation to get deg C.
        // Note this is chip temperature.
      //  printf("Temp. = %f\n", (temp / 340.0) + 36.53);

        sleep_ms(1000);
    }

#endif
    return 0;
}