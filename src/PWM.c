#include "PWM.h"

static uint16_t calc_duty_cycle(PWM *self, int8_t throttle);

/**
 * @brief Initialize an instance of a PWM-structure. (more info in details)
 * @details
 * The PWM function is configured with a clocdevider at 125
 * that result in 1 us / Hz.
 *
 * wrap:
 * savox SH-0257MG 240 Hz => 1 us * 4166  = 4.166ms => wrap = 4166
 * standard servo 50 Hz   => 1 us * 20000 = 20 ms   => wrap = 20000
 *
 * The duty cycle is 1.5 ms for 0 degrees / 0 speed.
 *
 * @param GPIO Pin number for the connected device
 * @param wrap PWM period in us
 * @param offset To calibrate steering servo.
 * @param limit To limit max/min speed or movement angle.
 * @return PWM Instance of a PWM-structure
 */
PWM new_PWM(const uint8_t GPIO, uint16_t wrap, int8_t offset, uint16_t limit)
{
    PWM self;
    gpio_set_function(GPIO, GPIO_FUNC_PWM);
    self.slice = pwm_gpio_to_slice_num(GPIO);
    self.channel = pwm_gpio_to_channel(GPIO);

    self.divider = 125;
    pwm_set_clkdiv(self.slice, (float)self.divider);

    self.wrap = wrap;
    pwm_set_wrap(self.slice, self.wrap);

    self.offset = offset;
    self.throttle = 0x00;
    self.limit = limit; // 350

    pwm_set_enabled(self.slice, true);

    return self;
}

/**
 * @brief Change rotation speed of motor.
 * Prevents damageing the car when switching from reverse to forward.
 * 0 = still, positive = forward, negative = reverse
 *
 * @param self Pointer address to the PWM instance.
 * @param throttle integer between -100 to 100
 */
void set_speed(PWM *self, int8_t throttle)
{
    if ((self->throttle < 0 && throttle > 0) ||
        (self->throttle > 0 && throttle < 0))
    {
        pwm_set_chan_level(self->slice, self->channel, calc_duty_cycle(self, 0));
        sleep_ms(200);
    }

    self->throttle = throttle;
    pwm_set_chan_level(self->slice, self->channel, calc_duty_cycle(self, throttle));
}

/**
 * @brief Set the steering angle for a PWM instance
 * @details
 * @param self Pointer address to the PWM instance.
 * @param angle the angle value from 0 to 200, where 100 is 0 degrees
 * @param invert inverts the steering if true
 */
void set_steering_angle(PWM *self, int8_t angle, bool invert)
{
    angle = angle - 100;
    if (invert)
    {
        angle = angle * -1;
    }
    pwm_set_chan_level(self->slice, self->channel, calc_duty_cycle(self, angle));
}

/**
 * @brief Function to calculate the dutycycle.
 *
 * @param self Pointer address to the PWM instance.
 * @param throttle Int between -100 and 100
 * @return uint16_t
 */
static uint16_t calc_duty_cycle(PWM *self, int8_t throttle)
{
    return (1500 + self->offset + (self->limit * ((float)throttle / 100)));
}
