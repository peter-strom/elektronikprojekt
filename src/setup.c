#include "header.h"

void setup()
{
    gpio_init(LED_BUILTIN);
    gpio_set_dir(LED_BUILTIN, GPIO_OUT);

    gpio_set_function(START_MODULE, GPIO_FUNC_SIO);
    gpio_set_dir(START_MODULE, GPIO_IN);
    gpio_is_pulled_down(START_MODULE);

    init_i2c();
    stdio_init_all();

    sensor_left = new_VL53L0X(TOF_SENSOR_L);
    sensor_right = new_VL53L0X(TOF_SENSOR_R);
    sensor_front = new_VL53L0X(TOF_SENSOR_F);
    pwm_servo = new_PWM(SERVO_PIN, 4166, 90, 450);
    pwm_esc = new_PWM(ESC_PIN, 20000, 0, 100);
    //pid_servo = new_PID(100, 1.8, 0.001, 0.04, 0, 200);
    pid_servo = new_PID(100, 1.5, 0.001, 0.08, 0.004, 0, 200,1000);
    //speed_ctrl = new_SpeedCtrl(1000, 33, 55, 90, 72, 150,-60, 70, 140);
    speed_ctrl = new_SpeedCtrl(1000, 33, 50, 90, 72, 0,-60, 60, 140);
    assign_new_address(&sensor_left, 0x30);
    assign_new_address(&sensor_right, 0x31);
    assign_new_address(&sensor_front, 0x32);

    debug = false;
}

// function used for debuging
void blink()
{
    gpio_put(LED_BUILTIN, 1);
    sleep_ms(750);
    gpio_put(LED_BUILTIN, 0);
    sleep_ms(1050);
}