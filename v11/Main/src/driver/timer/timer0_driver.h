#ifndef __TIMER_0_DRIVER__
#define __TIMER_0_DRIVER__

#include "timer_base_driver.h"
#include "../base/singleton.h"

class Timer0Driver : public TimerBaseDriver,
                     public Singleton<Timer0Driver>
{
    public:
        friend class Singleton<Timer0Driver>;

    public:
        void SetTimeupFunction(void (*function)(void));
        void ClearTimeupFunction(void);

    protected:
        Timer0Driver();
};

#endif
