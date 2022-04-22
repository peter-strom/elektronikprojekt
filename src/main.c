#include "header.h"

int main() {
  
    setup();
    
    VL53L0X sensor_left = new_VL53L0X(TOF_SENSOR_L);
    VL53L0X sensor_right = new_VL53L0X(TOF_SENSOR_R);
    PWM servo = new_PWM(SERVO_PIN);


   assign_new_address(&sensor_left, 0x30);
   assign_new_address(&sensor_right, 0x31);
    blink();
    uint16_t distance_left, distance_right;

    while (1) {
  distance_left = read_range(&sensor_left);
  distance_right = read_range(&sensor_right);
    printf("distance_left: %u - ", distance_left);
    printf("distance_right: %u\n", distance_right);
          
     set_speed(&servo, 0);
        sleep_ms(500);
      
    }


    return 0;
}