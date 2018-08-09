#include "timer_base_driver.h"

class Timer1Driver : public TimerBaseDriver
{
    public:
        Timer1Driver();
        void SetTimeupFunction(void (*function)(void));
        void ClearTimeupFunction(void);
};
