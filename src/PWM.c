#include "PWM.h"

static uint16_t calc_duty_cycle(PWM* self, int8_t throttle);

/***********************************************
 *  offset - is used to calibrate the servo
 *  limit - is to limit angle or max speed of the motor
 *          ESC ~ 100
 *          servo ~ 500
 *  1500 + offset +- limit
 *
 * *********************************************/
PWM new_PWM(const uint8_t GPIO)
{
   
    PWM self;
    gpio_set_function(GPIO, GPIO_FUNC_PWM);
    self.slice = pwm_gpio_to_slice_num(GPIO);
    self.channel = pwm_gpio_to_channel(GPIO);
    
    self.divider = 125;
    self.wrap = 20000;
    self.offset = 0;
    self.throttle = 0x00;
    self.limit = 100;
   
    // Setting the clock devider (prescaler)
    // 125 Mhz / 125 = 1Mhz = 1us
    pwm_set_clkdiv(self.slice, (float)self.divider);
    
     // Set the highest value the counter will reach before returning to 0
     // 1us * 20000 = 20ms
    pwm_set_wrap(self.slice, self.wrap);
      
    pwm_set_enabled(self.slice, true);
    
    return self;
}
/***********************************************
 * Fuction to change the speed of the car and 
 * prevents damageing the car when switching from 
 * reverse to forward.
 * 
 * int8_t throttle - a number between -100 to 100
 *                  where negative numbers are reverse 
 *                        zero or close to zero is off
 *                        positive is forward  
 * *********************************************/
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


/***********************************************
 * Fuction to calculate the dutycycle.
 * 
 * int8_t throttle - a number between -100 to 100
 *
 * *********************************************/
static uint16_t calc_duty_cycle(PWM* self, int8_t throttle)
{
    return (1500 + self->offset + (self->limit * ((float)throttle/100))); 
}

void setup() 
{
   // stdio_init_all();
    gpio_init(LED_BUILTIN);
    gpio_set_dir(LED_BUILTIN, GPIO_OUT);

}

void blink()
 {
    gpio_put(LED_BUILTIN, 1);
    sleep_ms(750);
    gpio_put(LED_BUILTIN, 0);
    sleep_ms(1050);
}