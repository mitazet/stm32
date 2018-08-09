#include "timer0_driver.h"
#include <stdlib.h>

extern TimerBase timer_def[TIMER_NUM];

extern void (*TimerTimeupFunction[TIMER_NUM])(void);

extern "C" void TIM6_DAC1_IRQHandler()
{
    if(TimerTimeupFunction[TIMER_0] != NULL){
        TimerTimeupFunction[TIMER_0]();
    }

    TIM6->SR = 0;
}

Timer0Driver::Timer0Driver(void){
   timer_base_ = timer_def[TIMER_0];
} 

void Timer0Driver::SetTimeupFunction(void (*function)(void))
{
    TimerTimeupFunction[TIMER_0] = function;
}

void Timer0Driver::ClearTimeupFunction(void)
{
    TimerTimeupFunction[TIMER_0] = NULL;
}
