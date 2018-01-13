#ifndef __TIMER_DRIVER__
#define __TIMER_DRIVER__

#include "stm32f303x8.h"

// Init Timer or Cancel Timer
extern void TimerInit(void);

// add timer counting by seconds
extern int TimerAdd_sec(int timeout_sec, void (*function)(void));

// add timer counting by milliseconds
extern int TimerAdd_msec(int timeout_msec, void (*function)(void));

#endif
