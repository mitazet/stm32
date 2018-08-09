#include "timer_base_driver.h"

class Timer0Driver : public TimerBaseDriver
{
    public:
        Timer0Driver();
        void SetTimeupFunction(void (*function)(void));
        void ClearTimeupFunction(void);
};
