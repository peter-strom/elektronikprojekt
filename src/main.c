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
    while (true)
    {
        if (gpio_get(START_MODULE))
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
            if(gyroscopeAvailable(&imu))
            {
                readAcceleration(&imu);
                printf("acc: \t %g - \t %g - \t %g - \t ", imu.acc_x, imu.acc_y, imu.acc_z);
            }
                readGyroscope(&imu);
                printf("gyro: \t %g - \t %g - \t %g  \n ", imu.gyro_x, imu.gyro_y, imu.gyro_z); 
            
            readTemperature(&imu);
            
           // printf("temperature: %d\n",imu.temperature);
             
            sleep_ms(200);
        }

    }

    return 0;
}