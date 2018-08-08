#ifndef __TIMER_DRIVER__
#define __TIMER_DRIVER__

#include "stm32f303x8.h"

class TimerDriver
{
    public:
        TimerDriver(RCC_TypeDef* rcc_addr = RCC, TIM_TypeDef* tim_addr = TIM6, IRQn_Type irq_num = TIM6_DAC1_IRQn);

        // Init Timer
        void Init(void);

        // add timer counting by seconds
        int Start_sec(int timeout_sec, void (*function)(void));

        // add timer counting by milliseconds
        int Start_msec(int timeout_msec, void (*function)(void));

        // cancel timer
        void Cancel(void);

    private:
        RCC_TypeDef* rcc_ref_;
        TIM_TypeDef* tim_ref_;
        IRQn_Type irq_num_;
        
        void StopTimer(void);
        void SetTimeupFunction(void (*function)(void));
        void ClearTimeupFunction(void);
        void StartTimer_sec(int timeout_sec);
        void StartTimer_msec(int timeout_msec);
};

#endif
