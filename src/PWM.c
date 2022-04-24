#include "PWM.h"

static uint16_t calc_duty_cycle(PWM* self, int8_t throttle);

/***********************************************
 *  offset - is used to calibrate the servo
 *  limit - is to limit angle or max speed of the motor
 *          ESC ~ 100
 *          servo ~ 500
 *  1500 + offset +- limit
 *
 **********************************************/

/**
 * @brief Initialize an instance of a PWM-structure.
 * 
 * @param GPIO pin number for the connected device
 * @return PWM Instance of a PWM-structure
 */
PWM new_PWM(const uint8_t GPIO)
{
    PWM self;
    gpio_set_function(GPIO, GPIO_FUNC_PWM);
    self.slice = pwm_gpio_to_slice_num(GPIO);
    self.channel = pwm_gpio_to_channel(GPIO);

     // Setting the clock devider (prescaler)
    // 125 Mhz / 125 = 1Mhz = 1us
    self.divider = 125;
    pwm_set_clkdiv(self.slice, (float)self.divider);

       // Set the highest value the counter will reach before returning to 0
     // 1us * 20000 = 20ms => vanliga servon 50Hz
     // 1us * 4166 = 4,166ms => savox SH-0257MG 240Hz
    self.wrap = 20000;
    pwm_set_wrap(self.slice, self.wrap);

    self.offset = 0;
    self.throttle = 0x00;
    self.limit = 700;//350
    
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
void set_speed(PWM* self, int8_t throttle)
{
     if((self->throttle < 0 && throttle > 0) ||
         (self->throttle > 0 && throttle < 0 ))
    {
        pwm_set_chan_level(self->slice, self->channel, calc_duty_cycle(self, 0));
        sleep_ms(200);
    }

    self->throttle = throttle;
    pwm_set_chan_level(self->slice, self->channel, calc_duty_cycle(self, throttle));
}


void set_steering_angle(PWM* self, int8_t angle)
{
    angle = angle -100;
    printf("stering value: %d\n", angle);
    pwm_set_chan_level(self->slice, self->channel, calc_duty_cycle(self, angle));
}


/**
 * @brief Fuction to calculate the dutycycle.
 * 
 * @param self Pointer address to the PWM instance.
 * @param throttle Int between -100 and 100
 * @return uint16_t 
 */
static uint16_t calc_duty_cycle(PWM* self, int8_t throttle)
{
    return (1500 + self->offset + (self->limit * ((float)throttle/100))); 
}

