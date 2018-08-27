// Timer ----------------------------------------------------------------------
#include "timer_driver.h"
#include <stdlib.h>
#include "io_reg.h"

// Default:TIM6
static RCC_TypeDef* RCC_ref = RCC;
static TIM_TypeDef* TIM_ref = TIM6;
static IRQn_Type IRQ_num = TIM6_DAC1_IRQn;

void (*TimerTimeupFunction)(void) = NULL;

void TimerCreate(RCC_TypeDef* rcc_addr, TIM_TypeDef* tim_addr, int irq_num){
    RCC_ref = rcc_addr;
    TIM_ref = tim_addr;
    IRQ_num = irq_num;
}

void TIM6_DAC1_IRQHandler()
{
    if(TimerTimeupFunction != NULL){
        TimerTimeupFunction();
    }

    TIM6->SR = 0;
}

static void StopTimer(void)
{
    ClearBit(&RCC_ref->APB1ENR, RCC_APB1ENR_TIM6EN);

    ClearReg(&TIM_ref->PSC);
    ClearReg(&TIM_ref->ARR);
    ClearReg(&TIM_ref->CNT);

    ClearBit(&TIM_ref->CR1, TIM_CR1_CEN);

    ClearBit(&TIM_ref->DIER, TIM_DIER_UIE);
}

static void SetTimeupFunction(void (*function)(void))
{
    TimerTimeupFunction = function;
}

static void ClearTimeupFunction(void)
{
    TimerTimeupFunction = NULL;
}

void TimerInit(void)
{
    StopTimer();
    NVIC_DisableIRQ(IRQ_num);
    ClearTimeupFunction();
}

static void StartTimer_sec(int timeout_sec)
{
    SetBit(&RCC_ref->APB1ENR, RCC_APB1ENR_TIM6EN);
    WriteReg(&TIM_ref->PSC, 9999);
    WriteReg(&TIM_ref->ARR, 800 * timeout_sec);
    WriteReg(&TIM_ref->CNT, 0);
    SetBit(&TIM_ref->CR1, TIM_CR1_CEN);

    SetBit(&TIM_ref->DIER, TIM_DIER_UIE);
}

static void StartTimer_msec(int timeout_msec)
{
    SetBit(&RCC_ref->APB1ENR, RCC_APB1ENR_TIM6EN);

    WriteReg(&TIM_ref->PSC, 999);
    WriteReg(&TIM_ref->ARR, 8 * timeout_msec);
    WriteReg(&TIM_ref->CNT, 0);
    SetBit(&TIM_ref->CR1, TIM_CR1_CEN);

    SetBit(&TIM_ref->DIER, TIM_DIER_UIE);
}

// add timer counting by seconds
int TimerAdd_sec(int timeout_sec, void (*function)(void))
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
int TimerAdd_msec(int timeout_msec, void (*function)(void))
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
void TimerCancel(void)
{
    StopTimer();
    NVIC_DisableIRQ(IRQ_num);
    ClearTimeupFunction();
}
