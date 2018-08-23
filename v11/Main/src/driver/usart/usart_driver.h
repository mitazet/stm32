#ifndef __USART_DRIVER__
#define __USART_DRIVER__

#include "stm32f303x8.h"
#include "../base/singleton.h"

enum UsartId
{
    USART_1,
    USART_2,
    USART_3,
    USART_NUM,
};

struct GpioPin
{
    GPIO_TypeDef*   port;
    int             no;
};

struct UsartPin
{
    GpioPin         pin;
    PinAltFunction  alt_func;
};

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
        RCC_TypeDef* rcc_base_;
        USART_TypeDef* usart_base_;
        UsartPin rx_;
        UsartPin tx_;
        uint32_t clk_en;
        
        void EnableUsart(void);
        void ConfigureUsart(void);

    protected:
        UsartDriver();
};

#endif
