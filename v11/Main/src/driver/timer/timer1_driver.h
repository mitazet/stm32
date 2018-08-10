#ifndef __TIMER_1_DRIVER__
#define __TIMER_1_DRIVER__

#include "timer_base_driver.h"
#include "../base/singleton.h"

class Timer1Driver : public TimerBaseDriver,
                     public Singleton<Timer1Driver>
{
    public:
        friend class Singleton<Timer1Driver>;

    public:
        void SetTimeupFunction(void (*function)(void));
        void ClearTimeupFunction(void);

    protected:
        Timer1Driver();
};

#endif
