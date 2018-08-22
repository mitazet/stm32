// Timer ----------------------------------------------------------------------
#include "timer_driver.h"
#include <stdlib.h>
#include "io_reg.h"


TimerBase timer_def[TIMER_NUM] = {
    {RCC, TIM6, RCC_APB1ENR_TIM6EN, TIM6_DAC1_IRQn},
    {RCC, TIM7, RCC_APB1ENR_TIM7EN, TIM7_DAC2_IRQn}
};

void (*TimerDriver::TimeupFunction[TIMER_NUM])(void) = {NULL, NULL};

void TimerIRQHandler(TimerId id)
{
    if(TimerDriver::TimeupFunction[id] != NULL){
        TimerDriver::TimeupFunction[id]();
    }

    (timer_def[id].timer)->SR = 0;
}

extern "C" void TIM6_DAC1_IRQHandler()
{
    TimerIRQHandler(TIMER_6);
}

extern "C" void TIM7_DAC2_IRQHandler()
{
    TimerIRQHandler(TIMER_7);
}

void TimerDriver::SetBase(TimerBase base)
{
    timer_base_ = base;
}

void TimerDriver::Init(void)
{
    for(uint32_t i = 0; i < TIMER_NUM; i++){
        Delete((TimerId)i);
    }
}

// start timer counting by seconds
timer_result_t TimerDriver::Add_sec(TimerId id, int timeout_sec, void (*function)(void))
{
    if(id >= TIMER_NUM){
        return TIMER_ILLEGAL_ID;
    }

    if(timeout_sec > 80){
        return TIMER_OUT_OF_RANGE;
    }

    SetBase(timer_def[id]);

    SetTimeupFunction(id, function);
    NVIC_EnableIRQ(timer_base_.irq_num);
    StartTimer_sec(timeout_sec);

    return TIMER_OK;
}

// start timer counting by milliseconds
timer_result_t TimerDriver::Add_msec(TimerId id, int timeout_msec, void (*function)(void))
{
    if(id >= TIMER_NUM){
        return TIMER_ILLEGAL_ID;
    }

    if(timeout_msec > 8000){
        return TIMER_OUT_OF_RANGE;
    }

    SetBase(timer_def[id]);

    SetTimeupFunction(id, function);
    NVIC_EnableIRQ(timer_base_.irq_num);
    StartTimer_msec(timeout_msec);

    return TIMER_OK;
}

// cancel timer
timer_result_t TimerDriver::Delete(TimerId id)
{
    if(id >= TIMER_NUM){
        return TIMER_ILLEGAL_ID;
    }

    SetBase(timer_def[id]);

    StopTimer();
    NVIC_DisableIRQ(timer_base_.irq_num);
    ClearTimeupFunction(id);

    return TIMER_OK;
}

void TimerDriver::StopTimer(void)
{
    ClearBit(&timer_base_.rcc->APB1ENR, timer_base_.clk_en);

    ClearReg(&timer_base_.timer->PSC);
    ClearReg(&timer_base_.timer->ARR);
    ClearReg(&timer_base_.timer->CNT);

    ClearBit(&timer_base_.timer->CR1, TIM_CR1_CEN);

    ClearBit(&timer_base_.timer->DIER, TIM_DIER_UIE);
}

void TimerDriver::StartTimer_sec(int timeout_sec)
{
    SetBit(&timer_base_.rcc->APB1ENR, timer_base_.clk_en);

    WriteReg(&timer_base_.timer->PSC, 9999);
    WriteReg(&timer_base_.timer->ARR, 800 * timeout_sec);
    WriteReg(&timer_base_.timer->CNT, 0);
    SetBit(&timer_base_.timer->CR1, TIM_CR1_CEN);

    SetBit(&timer_base_.timer->DIER, TIM_DIER_UIE);
}

void TimerDriver::StartTimer_msec(int timeout_msec)
{
    SetBit(&timer_base_.rcc->APB1ENR, timer_base_.clk_en);

    WriteReg(&timer_base_.timer->PSC, 999);
    WriteReg(&timer_base_.timer->ARR, 8 * timeout_msec);
    WriteReg(&timer_base_.timer->CNT, 0);
    SetBit(&timer_base_.timer->CR1, TIM_CR1_CEN);

    SetBit(&timer_base_.timer->DIER, TIM_DIER_UIE);
}

void TimerDriver::SetTimeupFunction(TimerId id, void (*function)(void))
{
    TimerDriver::TimeupFunction[id] = function;
}

void TimerDriver::ClearTimeupFunction(TimerId id)
{
    TimerDriver::TimeupFunction[id] = NULL;
}
