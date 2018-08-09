#include "timer1_driver.h"
#include <stdlib.h>

extern TimerBase timer_def[TIMER_NUM];

extern void (*TimerTimeupFunction[TIMER_NUM])(void);

extern "C" void TIM7_DAC2_IRQHandler()
{
    if(TimerTimeupFunction[TIMER_1] != NULL){
        TimerTimeupFunction[TIMER_1]();
    }

    TIM7->SR = 0;
}

Timer1Driver::Timer1Driver(void){
   timer_base_ = timer_def[TIMER_1];
} 

void Timer1Driver::SetTimeupFunction(void (*function)(void))
{
    TimerTimeupFunction[TIMER_1] = function;
}

void Timer1Driver::ClearTimeupFunction(void)
{
    TimerTimeupFunction[TIMER_1] = NULL;
}
