#include "sys/unistd.h"
#include "delay.h"


void delay_ms(unsigned int ms)
{
    usleep(1000*ms);
}


void delay_us(unsigned int us)
{
    usleep(us);
}