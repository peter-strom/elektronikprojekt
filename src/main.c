#include "header.h"

int main()
{
    setup();

    blink();
    uint16_t distance_left;
    uint16_t distance_right;
    uint16_t distance_front;
    int8_t esc_speed;
    uint8_t servo_angle;
    uint32_t curr_time;
    float delta_time;
    gpio_put(LED_BUILTIN, 1);
    while (true)
    {
        
        curr_time = get_absolute_time();
        if(prev_time==0)
        {
            delta_time = 0.004;
        }
        else
        {
        delta_time = (curr_time-prev_time)/(float)1000000;
        }
        prev_time = curr_time; 
        
       calc_orientation(&imu, delta_time);
        read_temperature(&imu);
        if (!gpio_get(START_MODULE))
        {
            distance_left = read_range(&sensor_left);
            distance_right = read_range(&sensor_right);
            distance_front = read_range(&sensor_front);
            servo_angle = ((uint8_t)PID_get_servo_value_from_sensors(&pid_servo, distance_left, distance_right));
            esc_speed = SpeedCtrl_calc_speed(&speed_ctrl, distance_front, distance_left, distance_right);
            if (esc_speed < 0)
            {
                if (servo_angle > 100 && servo_angle < 140)
                {
                    servo_angle = 200;
                }
                else if (servo_angle <= 100 && servo_angle > 60)
                {
                    servo_angle = 0;
                }
                set_steering_angle(&pwm_servo, servo_angle, false);
            }
            else
            {
                set_steering_angle(&pwm_servo, servo_angle, true);
            }
            set_speed(&pwm_esc, esc_speed);
        }
        else
        {
            set_speed(&pwm_esc, 0);
            set_steering_angle(&pwm_servo, 0, true);
        }

        if (debug_pid)
        {

            print(&pid_servo);
            printf("distance_left: %u - \t ", distance_left);
            printf("distance_right: %u - \t", distance_right);
            printf("distance_front: %d - \t", distance_front);
            printf("servo_angle: %d - \t", servo_angle);
            printf("speedctrl_output: %d \n", esc_speed);
            sleep_ms(200);
        }

        if (debug_imu)
        {
           
                
          //printf("%g\t\t%g\t%g\t%g\t\t%g\n", imu.gyro_error_x, imu.gyro_error_y, imu.gyro_error_z, imu.acc_error_x,imu.acc_error_y);
          //printf("%g\t\t%g\t%g\n", imu.acc_x, imu.acc_y, imu.acc_z);
          //printf("%g\t\t%g\t%g\t%g\n", imu.roll, imu.pitch, imu.yaw, delta_time);
          printf("%g\t%g\n", imu.pitch, imu.acc_error_y);
          printf("temperature: %g \n", imu.temperature);
           //sleep_ms(200);
           
            
         
             
            
        }

    }

    return 0;
}