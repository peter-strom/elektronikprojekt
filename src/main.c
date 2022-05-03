#include "header.h"

int main()
{
    setup();

    blink();
    uint16_t distance_left;
    uint16_t distance_right;
    uint16_t distance_front;
    uint8_t esc_speed;
    uint8_t servo_angle;

    while (true)
    {
        distance_left = read_range(&sensor_left);
        distance_right = read_range(&sensor_right);
        distance_front = read_range(&sensor_front);
        servo_angle = ((uint8_t)PID_get_servo_value_from_sensors(&pid_servo, distance_left, distance_right));
        set_steering_angle(&pwm_servo, servo_angle, true);
        esc_speed = SpeedCtrl_calc_speed(&speed_ctrl, distance_front,distance_left, distance_right);
        set_speed(&pwm_esc, esc_speed);

        if (debug)
        {
            printf("distance_left: %u - ", distance_left);
            printf("distance_right: %u -", distance_right);
            printf("distance_front: %d -", distance_front);
            printf("servo_angle: %d ", servo_angle);
            printf("speedctrl_output: %d \n", esc_speed);
        }
        //sleep_ms(2);
    }

    return 0;
}