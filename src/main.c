#include "header.h"







int main() {
    //stdio_init_all();
    //init_i2c();
    setup();
    
     VL53L0X sensor = new_VL53L0X(7);
   
    assign_new_address(&sensor, 0x29);
    blink();
    int16_t distance;

    while (1) {
   distance = read_range(&sensor);
      printf("distance: %d\n", distance);
      

       //mpu6050_read_raw(acceleration, gyro, &temp);

        // These are the raw numbers from the chip, so will need tweaking to be really useful.
        // See the datasheet for more information
     //printf("Acc. X = %d, Y = %d, Z = %d\n", acceleration[0], acceleration[1], acceleration[2]);
      //  printf("Gyro. X = %d, Y = %d, Z = %d\n", gyro[0], gyro[1], gyro[2]);
        // Temperature is simple so use the datasheet calculation to get deg C.
        // Note this is chip temperature.
      //  printf("Temp. = %f\n", (temp / 340.0) + 36.53);

        sleep_ms(100);
      
    }


    return 0;
}