#include "header.h"







int main() {
    //stdio_init_all();
    //init_i2c();
    setup();
    
    VL53L0X sensor = new_VL53L0X(7);
    VL53L0X sensor2 = new_VL53L0X(21);
   
   assign_new_address(&sensor, 0x30);
   assign_new_address(&sensor2, 0x31);
    blink();
    uint16_t distance,distance2;

    while (1) {
  distance = read_range(&sensor);
  distance2 = read_range(&sensor2);
    printf("distance: %u - ", distance);
    printf("distance2: %u\n", distance2);

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