// Timer ----------------------------------------------------------------------
#include "timer_driver.h"
#include "intr_driver.h"
#include <stdlib.h>

// Default:TIM6
static RCC_TypeDef* RCC_address = RCC;
static TIM_TypeDef* TIM_address = TIM6;
static int IRQ_num = TIM6_DAC1_IRQn;

void (*callback)(void);

void TimerCreate(RCC_TypeDef* rcc_addr, TIM_TypeDef* tim_addr, int irq_num){
	RCC_address	= rcc_addr;
	TIM_address	= tim_addr;
	IRQ_num		= irq_num;
}

void TimerIntrHandler(void)
{
	if(callback != NULL){
		callback();
	}
    
    TIM6->SR = 0;
}

void TimerInit(void)
{
	RCC->APB1ENR &= ~RCC_APB1ENR_TIM6EN;
	TIM6->PSC = 0;
	TIM6->ARR = 0;
	TIM6->CNT = 0;
	TIM6->CR1 &= ~TIM_CR1_CEN;

	TIM6->DIER &= ~TIM_DIER_UIE;
	NVIC_DisableIRQ(TIM6_DAC_IRQn);
	callback = NULL;

    IntrHandlerSet(INTR_TYPE_TIMER, TimerIntrHandler);
}

static void StartTimer_sec(int timeout_sec)
{
	RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;
	TIM6->PSC = 9999;
	TIM6->ARR = 800 * timeout_sec;
	TIM6->CNT = 0;
	TIM6->CR1 |= TIM_CR1_CEN;

	TIM6->DIER = TIM_DIER_UIE;
	NVIC_EnableIRQ(TIM6_DAC_IRQn);
}

static void StartTimer_msec(int timeout_msec)
{
	RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;
	TIM6->PSC = 999;
	TIM6->ARR = 8 * timeout_msec;
	TIM6->CNT = 0;
	TIM6->CR1 |= TIM_CR1_CEN;

	TIM6->DIER = TIM_DIER_UIE;
	NVIC_EnableIRQ(TIM6_DAC_IRQn);
}

// add timer counting by seconds
int TimerAdd_sec(int timeout_sec, void (*function)(void))
{
	if(timeout_sec > 80){
		return -1;
	}

	callback = function;
	StartTimer_sec(timeout_sec);

	return 0;
}

// add timer counting by milliseconds
int TimerAdd_msec(int timeout_msec, void (*function)(void))
{
	if(timeout_msec > 8000){
		return -1;
	}

	callback = function;
	StartTimer_msec(timeout_msec);

	return 0;
}
