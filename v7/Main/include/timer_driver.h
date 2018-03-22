#ifndef __TIMER_DRIVER__
#define __TIMER_DRIVER__

#include "stm32f303x8.h"
// Create Timer Driver
extern void TimerCreate(RCC_TypeDef* rcc_addr, TIM_TypeDef* tim_addr, int irq_num);

// Init Timer
extern void TimerInit(void);

// add timer counting by seconds
extern int TimerAdd_sec(int timeout_sec, void (*function)(void));

// add timer counting by milliseconds
extern int TimerAdd_msec(int timeout_msec, void (*function)(void));

// cancel timer
extern void TimerCancel(void);

#endif
