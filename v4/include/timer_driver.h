#ifndef __TIMER_DRIVER__
#define __TIMER_DRIVER__

// Init Timer or Cancel Timer
extern void TimerInit(void);

// add timer counting by seconds
extern void TimerAdd_sec(int timeout_sec, void (*function)(void));

// add timer counting by milliseconds
extern void TimerAdd_msec(int timeout_msec, void (*function)(void));

#endif
