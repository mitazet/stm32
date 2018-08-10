#ifndef __USART_DRIVER__
#define __USART_DRIVER__

#include "stm32f303x8.h"
#include "../base/singleton.h"

class UsartDriver : public Singleton<UsartDriver>
{
    public:
        friend class Singleton<UsartDriver>;

    public:
        void Init(void);
        uint32_t IsReadEnable(void);
        uint32_t IsWriteEnable(void);
        uint8_t Read(void);
        void Write(uint8_t c);
        void SetBase(RCC_TypeDef* rcc_addr, GPIO_TypeDef* gpio_addr, USART_TypeDef* usart_addr);

    private:
        RCC_TypeDef* rccAddress_;
        GPIO_TypeDef* gpioAddress_;
        USART_TypeDef* usartAddress_;

        void EnableUsart2(void);
        void ConfigureUsart(void);

    protected:
        UsartDriver();
};

#endif
