#include "header.h"
#include "pico/binary_info.h"







int main(void)
{
    setup();


    i2c_inst_t* i2c = i2c0; 
    //stdio_init_all();
    i2c_init(i2c_default, 400 * 1000); //400khz

    gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
  
   // gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
   // gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);

    bi_decl(bi_2pins_with_func(PICO_DEFAULT_I2C_SDA_PIN, PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C));
blink();
   
 mpu6050_reset();
blink();
    int16_t acceleration[3], gyro[3], temp;
    uint8_t data = 5;

    while(true)
    {
     
       mpu6050_read_raw(acceleration, gyro, &temp);
        
        printf("Acc. X = %d, Y = %d, Z = %d\n", acceleration[0], acceleration[1], acceleration[2]);
        printf("Gyro. X = %d, Y = %d, Z = %d\n", gyro[0], gyro[1], gyro[2]);
        printf("Temp. = %f\n", (temp / 340.0) + 36.53);
        sleep_ms(1000);
    } 

    return 0;
}