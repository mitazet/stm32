#ifndef __TIMER_DRIVER__
#define __TIMER_DRIVER__

#include "stm32f303x8.h"

typedef enum{
    TIMER_0,
    TIMER_1,
    TIMER_NUM,
}TimerId;

typedef struct{
    RCC_TypeDef *rcc;
    TIM_TypeDef *timer;
    uint32_t enable_bit;
    IRQn_Type irq_num;
}TimerBase;

class TimerBaseDriver
{
    public:
        void Init(void);
        int Start_sec(int timeout_sec, void (*function)(void));
        int Start_msec(int timeout_msec, void (*function)(void));
        void Cancel(void);
        void SetBase(TimerBase base);
        static void (*TimeupFunction[TIMER_NUM])(void);

    private:
        void StopTimer(void);
        void StartTimer_sec(int timeout_sec);
        void StartTimer_msec(int timeout_msec);
        virtual void SetTimeupFunction(void (*function)(void)){};
        virtual void ClearTimeupFunction(void){};

    protected:
        TimerBase timer_base_;
};

#endif
