#include "header.h"

int main() {
  
    setup();
    
    VL53L0X sensor_left = new_VL53L0X(TOF_SENSOR_L);
    VL53L0X sensor_right = new_VL53L0X(TOF_SENSOR_R);
    PWM pwm_servo = new_PWM(SERVO_PIN);
    PID pid_servo = new_PID(100,0.8,0.001,0.04,0,200);

   assign_new_address(&sensor_left, 0x30);
   assign_new_address(&sensor_right, 0x31);
    blink();
    uint16_t distance_left, distance_right;
    uint8_t servo_angle;
    while (1) {
  distance_left = read_range(&sensor_left);
  distance_right = read_range(&sensor_right);
    printf("distance_left: %u - ", distance_left);
    printf("distance_right: %u\n", distance_right);
    servo_angle = ((uint8_t)PID_get_servo_value_from_sensors(&pid_servo, distance_left, distance_right));
     set_steering_angle(&pwm_servo, servo_angle);
        sleep_ms(10);
      
    }


    return 0;
}