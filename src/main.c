#include "header.h"

int main() {
  
    setup();
    /*
      VL53L0X sensor_left = new_VL53L0X(TOF_SENSOR_L);
      VL53L0X sensor_right = new_VL53L0X(TOF_SENSOR_R);
      PWM pwm_servo = new_PWM(SERVO_PIN,20000,0,300);
      PID pid_servo = new_PID(100,0.8,0.001,0.04,0,200);

      assign_new_address(&sensor_left, 0x30);
      assign_new_address(&sensor_right, 0x32);
    */
      PWM pwm_esc = new_PWM(ESC_PIN,20000,0,100);
      //PID pid_servo = new_PID(100,0.8,0.001,0.04,0,200);
    VL53L0X sensor_front = new_VL53L0X(TOF_SENSOR_F);
    assign_new_address(&sensor_front, 0x31);

    blink();
    uint16_t distance_left;
    uint16_t distance_right;
    uint16_t distance_front;
    uint8_t esc_speed;
    uint8_t servo_angle;
    while (1) {
    /*
      distance_left = read_range(&sensor_left);
      distance_right = read_range(&sensor_right);
      printf("distance_left: %u - ", distance_left);
      printf("distance_right: %u\n", distance_right);
      servo_angle = ((uint8_t)PID_get_servo_value_from_sensors(&pid_servo, distance_left, distance_right));
      set_steering_angle(&pwm_servo, servo_angle);
    */
      distance_front = read_range(&sensor_front);
      
      calc_speed()
    uint16_t max_input = 1000;
    float percent ;
    uint8_t break_distance = 50;
    uint8_t output_min = 30;
    uint8_t output_max = 50;
    uint8_t output_span = output_max - output_min;
    uint8_t output = 
      //esc_speed mellan 30 och 50
      //
      esc_speed = distance_front /10;
      if(esc_speed > 100)
      {
        escspeed= 100;
      }



      printf("distance_front: %d \n", distance_front);
      set_speed(&pwm_esc, distance_front);
      sleep_ms(100);
      
    }


    return 0;
}