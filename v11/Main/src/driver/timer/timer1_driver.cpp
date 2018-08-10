#include "timer1_driver.h"
#include <stdlib.h>

extern TimerBase timer_def[TIMER_NUM];

Timer1Driver::Timer1Driver(void)
{
   timer_base_ = timer_def[TIMER_1];
} 

void Timer1Driver::SetTimeupFunction(void (*function)(void))
{
    Timer1Driver::TimeupFunction[TIMER_1] = function;
}

void Timer1Driver::ClearTimeupFunction(void)
{
    Timer1Driver::TimeupFunction[TIMER_1] = NULL;
}
