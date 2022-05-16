#include "header.h"

int main()
{
    setup();

    uint16_t distance_left;
    uint16_t distance_right;
    uint16_t distance_front;
    int8_t esc_speed;
    uint8_t servo_angle;
    float delta_time;
    gpio_put(LED_BUILTIN, 1);
    while (true)
    {
        delta_time = calc_delta_time(&prev_time);
        if (!gpio_get(START_MODULE))
        {

            distance_left = read_range(&sensor_left);
            distance_right = read_range(&sensor_right);
            distance_front = read_range(&sensor_front);
            servo_angle = ((uint8_t)PID_get_servo_value_from_sensors(&pid_servo, distance_left, distance_right));
            esc_speed = SpeedCtrl_calc_speed(&speed_ctrl, distance_front, distance_left, distance_right);
            if (speed_ctrl.reverse)
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

        // printf("left: %u -\n ", speed_ctrl.rev_count);

        printf("left: \t%g -\t ", delta_time);
        if (debug)
        {

            print(&pid_servo);
            printf("left: \t%u -\t ", distance_left);
            printf("right: \t%u - \t", distance_right);
            printf("front: \t%u - \t", distance_front);
            printf("integral: \t%f \t", pid_servo.integral);
            printf("servo_angle: \t%d - \t", servo_angle);
            printf("output: \t%d \n", esc_speed);
            sleep_ms(200);
        }
    }

    return 0;
}