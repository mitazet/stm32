// Timer ----------------------------------------------------------------------
#include "timer_base_driver.h"
#include <stdlib.h>
#include "io_reg.h"


TimerBase timer_def[TIMER_NUM] = {
    {RCC, TIM6, RCC_APB1ENR_TIM6EN, TIM6_DAC1_IRQn},
    {RCC, TIM7, RCC_APB1ENR_TIM7EN, TIM7_DAC2_IRQn}
};

void (*TimerBaseDriver::TimeupFunction[TIMER_NUM])(void) = {NULL, NULL};

void TimerIRQHandler(TimerId id)
{
    if(TimerBaseDriver::TimeupFunction[id] != NULL){
        TimerBaseDriver::TimeupFunction[id]();
    }

    (timer_def[id].timer)->SR = 0;
}

extern "C" void TIM6_DAC1_IRQHandler()
{
    TimerIRQHandler(TIMER_0);
}

extern "C" void TIM7_DAC2_IRQHandler()
{
    TimerIRQHandler(TIMER_1);
}

void TimerBaseDriver::SetBase(TimerBase base)
{
    timer_base_ = base;
}

void TimerBaseDriver::Init(void)
{
    StopTimer();
    NVIC_DisableIRQ(timer_base_.irq_num);
    ClearTimeupFunction();
}

// start timer counting by seconds
int TimerBaseDriver::Start_sec(int timeout_sec, void (*function)(void))
{
    if(timeout_sec > 80){
        return -1;
    }

    SetTimeupFunction(function);
    NVIC_EnableIRQ(timer_base_.irq_num);
    StartTimer_sec(timeout_sec);

    return 0;
}

// start timer counting by milliseconds
int TimerBaseDriver::Start_msec(int timeout_msec, void (*function)(void))
{
    if(timeout_msec > 8000){
        return -1;
    }

    SetTimeupFunction(function);
    NVIC_EnableIRQ(timer_base_.irq_num);
    StartTimer_msec(timeout_msec);

    return 0;
}

// cancel timer
void TimerBaseDriver::Cancel(void)
{
    StopTimer();
    NVIC_DisableIRQ(timer_base_.irq_num);
    ClearTimeupFunction();
}

void TimerBaseDriver::StopTimer(void)
{
    ClearBit(&timer_base_.rcc->APB1ENR, timer_base_.enable_bit);

    ClearReg(&timer_base_.timer->PSC);
    ClearReg(&timer_base_.timer->ARR);
    ClearReg(&timer_base_.timer->CNT);

    ClearBit(&timer_base_.timer->CR1, TIM_CR1_CEN);

    ClearBit(&timer_base_.timer->DIER, TIM_DIER_UIE);
}

void TimerBaseDriver::StartTimer_sec(int timeout_sec)
{
    SetBit(&timer_base_.rcc->APB1ENR, timer_base_.enable_bit);

    WriteReg(&timer_base_.timer->PSC, 9999);
    WriteReg(&timer_base_.timer->ARR, 800 * timeout_sec);
    WriteReg(&timer_base_.timer->CNT, 0);
    SetBit(&timer_base_.timer->CR1, TIM_CR1_CEN);

    SetBit(&timer_base_.timer->DIER, TIM_DIER_UIE);
}

void TimerBaseDriver::StartTimer_msec(int timeout_msec)
{
    SetBit(&timer_base_.rcc->APB1ENR, timer_base_.enable_bit);

    WriteReg(&timer_base_.timer->PSC, 999);
    WriteReg(&timer_base_.timer->ARR, 8 * timeout_msec);
    WriteReg(&timer_base_.timer->CNT, 0);
    SetBit(&timer_base_.timer->CR1, TIM_CR1_CEN);

    SetBit(&timer_base_.timer->DIER, TIM_DIER_UIE);
}

