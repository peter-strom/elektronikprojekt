#include "header.h"








int main(void)
{
    setup();
    
    //test
    PWM ESC = new_PWM(ESC_PIN);
    set_speed(&ESC, 35);
    sleep_ms(6000);
    set_speed(&ESC, -70);
    sleep_ms(6000);
    set_speed(&ESC, 50);
    sleep_ms(6000);
    set_speed(&ESC, 0);

    while(true)
    {
        blink();
    } 

    return 0;
}