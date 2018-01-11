// Timer ----------------------------------------------------------------------
#include "timer_driver.h"
#include "printf.h"

void (*callback)(void);

void TimerInit(void)
{
	RCC->APB1ENR &= ~RCC_APB1ENR_TIM6EN;
	TIM6->PSC = 0;
	TIM6->ARR = 0;
	TIM6->CNT = 0;
	TIM6->CR1 &= ~TIM_CR1_CEN;

	TIM6->DIER &= ~TIM_DIER_UIE;
	callback = NULL;
}

static void TimerStart_sec(int timeout_sec)
{
	RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;
	TIM6->PSC = 9999;
	TIM6->ARR = 800 * timeout_sec;
	TIM6->CNT = 0;
	TIM6->CR1 |= TIM_CR1_CEN;

	TIM6->DIER = TIM_DIER_UIE;
	NVIC_EnableIRQ(TIM6_DAC_IRQn);
}

static void TimerStart_msec(int timeout_msec)
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
		printf("Error : Over 80 seconds");
		return -1;
	}

	timeout = timeout_sec; 
	callback = function;
	TimerStart_sec(timeout);

	return 0
}

// add timer counting by milliseconds
int TimerAdd_msec(int timeout_msec, void (*function)(void))
{
	if(timeout_msec > 8000){
		printf("Error : Over 8000 milliseconds");
		return -1;
	}

	timeout = timeout_msec; 
	callback = function;
	TimerStart_msec(timeout);

	return 0
}

void TIM6_DAC1_IRQHandler()
{
	if(callback != NULL){
		callback();
	}
}
