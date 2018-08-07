// Timer ----------------------------------------------------------------------
#include "timer_driver.h"
#include <stdlib.h>
#include "io_reg.h"

void (*TimerTimeupFunction)(void) = NULL;

extern void TIM6_DAC1_IRQHandler()
{
    if(TimerTimeupFunction != NULL){
        TimerTimeupFunction();
    }

    TIM6->SR = 0;
}

TimerDriver::TimerDriver(RCC_TypeDef* rcc_addr, TIM_TypeDef* tim_addr, IRQn_Type irq_num){
    rcc_ref_ = rcc_addr;
    tim_ref_ = tim_addr;
    irq_num_ = irq_num;
}

void TimerDriver::StopTimer(void)
{
    ClearBit(&rcc_ref_->APB1ENR, RCC_APB1ENR_TIM6EN);

    ClearReg(&tim_ref_->PSC);
    ClearReg(&tim_ref_->ARR);
    ClearReg(&tim_ref_->CNT);

    ClearBit(&tim_ref_->CR1, TIM_CR1_CEN);

    ClearBit(&tim_ref_->DIER, TIM_DIER_UIE);
}

void TimerDriver::SetTimeupFunction(void (*function)(void))
{
    TimerTimeupFunction = function;
}

void TimerDriver::ClearTimeupFunction(void)
{
    TimerTimeupFunction = NULL;
}

void TimerDriver::Init(void)
{
    StopTimer();
    NVIC_DisableIRQ(irq_num_);
    ClearTimeupFunction();
}

void TimerDriver::StartTimer_sec(int timeout_sec)
{
    SetBit(&rcc_ref_->APB1ENR, RCC_APB1ENR_TIM6EN);
    WriteReg(&tim_ref_->PSC, 9999);
    WriteReg(&tim_ref_->ARR, 800 * timeout_sec);
    WriteReg(&tim_ref_->CNT, 0);
    SetBit(&tim_ref_->CR1, TIM_CR1_CEN);

    SetBit(&tim_ref_->DIER, TIM_DIER_UIE);
}

void TimerDriver::StartTimer_msec(int timeout_msec)
{
    SetBit(&rcc_ref_->APB1ENR, RCC_APB1ENR_TIM6EN);

    WriteReg(&tim_ref_->PSC, 999);
    WriteReg(&tim_ref_->ARR, 8 * timeout_msec);
    WriteReg(&tim_ref_->CNT, 0);
    SetBit(&tim_ref_->CR1, TIM_CR1_CEN);

    SetBit(&tim_ref_->DIER, TIM_DIER_UIE);
}

// add timer counting by seconds
int TimerDriver::Add_sec(int timeout_sec, void (*function)(void))
{
    if(timeout_sec > 80){
        return -1;
    }

    SetTimeupFunction(function);
    NVIC_EnableIRQ(TIM6_DAC_IRQn);
    StartTimer_sec(timeout_sec);

    return 0;
}

// add timer counting by milliseconds
int TimerDriver::Add_msec(int timeout_msec, void (*function)(void))
{
    if(timeout_msec > 8000){
        return -1;
    }

    SetTimeupFunction(function);
    NVIC_EnableIRQ(TIM6_DAC_IRQn);
    StartTimer_msec(timeout_msec);

    return 0;
}

// cancel timer
void TimerDriver::Cancel(void)
{
    StopTimer();
    NVIC_DisableIRQ(irq_num_);
    ClearTimeupFunction();
}
