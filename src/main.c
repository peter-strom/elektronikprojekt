#include "header.h"








int main(void)
{
    setup();

    uint8_t data = 5;
    //reg_read(i2c_unit,i2c_device_adress,device_register,*data_pointer, no_bytes)
    reg_read(i2c,I2C_ADDRESS,0xC0,&data,1);
    while(true)
    {
        printf("data: %d\n",data);
        blink();
    } 

    return 0;
}