#ifndef __TIMER_DRIVER__
#define __TIMER_DRIVER__

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f303x8.h"

extern void TIM6_DAC1_IRQHandler(void);

#ifdef __cplusplus
}
#endif

class TimerDriver
{
    public:
        TimerDriver(RCC_TypeDef* rcc_addr = RCC, TIM_TypeDef* tim_addr = TIM6, IRQn_Type irq_num = TIM6_DAC1_IRQn);

        // Init Timer
        void Init(void);

        // add timer counting by seconds
        int Add_sec(int timeout_sec, void (*function)(void));

        // add timer counting by milliseconds
        int Add_msec(int timeout_msec, void (*function)(void));

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
