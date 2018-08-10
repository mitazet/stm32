#ifndef __LED_DRIVER__
#define __LED_DRIVER__

#include "stm32f303x8.h"
#include "../base/singleton.h"

class LedDriver : public Singleton<LedDriver>
{
    public:
        friend class Singleton<LedDriver>;

    public:
        void Init();
        void On();
        void Off();
        uint32_t IsOn();
        void SetBase(RCC_TypeDef* rcc_addr = RCC, GPIO_TypeDef* gpio_addr = GPIOB);

    private:
        RCC_TypeDef* rcc_base_;
        GPIO_TypeDef* gpio_base_;

    protected:
        LedDriver();
};

#endif
