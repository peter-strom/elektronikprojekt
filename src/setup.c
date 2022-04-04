#include "header.h"

void setup() 
{
    gpio_init(LED_BUILTIN);
    gpio_set_dir(LED_BUILTIN, GPIO_OUT);

}

//function used for debuging
void blink()
 {
    gpio_put(LED_BUILTIN, 1);
    sleep_ms(750);
    gpio_put(LED_BUILTIN, 0);
    sleep_ms(1050);
}