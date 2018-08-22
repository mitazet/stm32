#ifndef __TIMER_DRIVER__
#define __TIMER_DRIVER__

#include "stm32f303x8.h"
#include "../base/singleton.h"

typedef enum{
    TIMER_6,
    TIMER_7,
    TIMER_NUM,
}TimerId;

typedef struct{
    RCC_TypeDef *rcc;
    TIM_TypeDef *timer;
    uint32_t    clk_en;
    IRQn_Type   irq_num;
}TimerBase;


class TimerDriver : public Singleton<TimerDriver>
{
    public:
        friend class Singleton<TimerDriver>;

    public:
        void Init(void);
        int Add_sec(TimerId id, int timeout_sec, void (*function)(void));
        int Add_msec(TimerId id, int timeout_msec, void (*function)(void));
        void Delete(TimerId id);
        void SetBase(TimerBase base);
        static void (*TimeupFunction[TIMER_NUM])(void);

    private:
        void StopTimer(void);
        void StartTimer_sec(int timeout_sec);
        void StartTimer_msec(int timeout_msec);
        void SetTimeupFunction(TimerId id, void (*function)(void));
        void ClearTimeupFunction(TimerId id);

        TimerBase timer_base_;

    protected:
        TimerDriver(){}
};

#endif
