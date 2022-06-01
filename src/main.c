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

    /**
     * @brief activates a led when all components are initialized
     *
     */
    gpio_put(LED_BUILTIN, 1);
    while (true)
    {
        delta_time = calc_delta_time(&prev_time);
        calc_orientation(&imu, delta_time);

        /**
         * @brief activate all driving functions if START MODULE is active
         *
         */
        if (!gpio_get(START_MODULE))
        {

            distance_left = read_range(&sensor_left);
            distance_right = read_range(&sensor_right);
            distance_front = read_range(&sensor_front);
            servo_angle = ((uint8_t)PID_get_servo_value_from_sensors(&pid_servo, distance_left, distance_right, delta_time));
            esc_speed = SpeedCtrl_calc_speed(&speed_ctrl, distance_front, distance_left, distance_right, servo_angle, imu.pitch);

            /**
             * @brief if speed control is in state reverse: 
             * invert the steering and override PID control
             * in smal steering angles.
             */
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

        if (debug_pid)
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

        if (debug_imu)
        {
            // printf("%g\t\t%g\t%g\t%g\t\t%g\n", imu.gyro_error_x, imu.gyro_error_y, imu.gyro_error_z, imu.acc_error_x,imu.acc_error_y);
            // printf("%g\t\t%g\t%g\n", imu.acc_x, imu.acc_y, imu.acc_z);
            // printf("%g\t\t%g\t%g\t%g\n", imu.roll, imu.pitch, imu.yaw, delta_time);
            printf("%g\t%g\n", imu.pitch, imu.gyro_error_y);
            // printf("temperature: %g \n", imu.temperature);
            // sleep_ms(200);
        }
    }
    return 0;
}