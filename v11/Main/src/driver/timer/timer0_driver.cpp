#include "timer0_driver.h"
#include <stdlib.h>

extern TimerBase timer_def[TIMER_NUM];

Timer0Driver::Timer0Driver(void)
{
    timer_base_ = timer_def[TIMER_0];
} 

void Timer0Driver::SetTimeupFunction(void (*function)(void))
{
    Timer0Driver::TimeupFunction[TIMER_0] = function;
}

void Timer0Driver::ClearTimeupFunction(void)
{
    Timer0Driver::TimeupFunction[TIMER_0] = NULL;
}
