#ifndef __USART_DRIVER__
#define __USART_DRIVER__

#include "stm32f303x8.h"
#include "../base/singleton.h"
#include "io_reg.h"

enum UsartId
{
    USART_1,
    USART_2,
    USART_NUM,
};

// Pin definition
template<char port> struct port_gpio_t;

template<> struct port_gpio_t<'A'>
{
    constexpr static GPIO_TypeDef* GPIOx_BASE = (GPIO_TypeDef*)GPIOA_BASE;
};

template<> struct port_gpio_t<'B'>
{
    constexpr static GPIO_TypeDef* GPIOx_BASE = (GPIO_TypeDef*)GPIOB_BASE;
};

template<char port, int pin_no> struct Pin;
template<char port, int pin_no>
struct Pin
{
    constexpr static int pin = pin_no;
    constexpr static GPIO_TypeDef* GPIOx_BASE = port_gpio_t<port>::GPIOx_BASE;
    
    static GPIO_TypeDef* gpio_base;

    static void Alternate(PinAltFunction alt_func){
        SetBit(&gpio_base->MODER, MODER_ALTERNATE << pin_no * 2);

        uint32_t shift_x4;
        shift_x4 = (pin_no % 8) * 4;
        SetBit(&gpio_base->AFR[pin_no/8], alt_func << shift_x4);
    }
};

//USART Pin definition
template<UsartId> struct UsartPinSet;

template<> struct UsartPinSet<USART_1>
{
    typedef Pin<'A',  9> PinTx;
    typedef Pin<'A', 10> PinRx;
    constexpr static PinAltFunction ALT_FUNC_USART = ALT_FUNC_USART1;

    static void EnableClock(RCC_TypeDef *rcc_addr){
        SetBit(&rcc_addr->AHBENR, RCC_AHBENR_GPIOAEN);
    }
};

template<> struct UsartPinSet<USART_2>
{
    typedef Pin<'A',  2> PinTx;
    typedef Pin<'A', 15> PinRx;
    constexpr static PinAltFunction ALT_FUNC_USART = ALT_FUNC_USART2;

    static void EnableClock(RCC_TypeDef *rcc_addr){
        SetBit(&rcc_addr->AHBENR, RCC_AHBENR_GPIOAEN);
    }
};

// USART Traits template
template<UsartId id> struct UsartTraits;

template<> struct UsartTraits<USART_1>
{
    constexpr static RCC_TypeDef* rcc_base = RCC;
    constexpr static USART_TypeDef* usart_base = (USART_TypeDef*)USART1_BASE;    

    static void EnableClock(RCC_TypeDef *rcc_addr){
        SetBit(&rcc_addr->APB2ENR, RCC_APB2ENR_USART1EN);
    }
};

template<> struct UsartTraits<USART_2>
{
    constexpr static RCC_TypeDef* rcc_base = RCC;
    constexpr static USART_TypeDef* usart_base = (USART_TypeDef*)USART2_BASE;    

    static void EnableClock(RCC_TypeDef *rcc_addr){
        SetBit(&rcc_addr->APB1ENR, RCC_APB1ENR_USART2EN);
    }
};

//USART driver (USART_2)
class UsartDriver : public Singleton<UsartDriver>
{
    public:
        friend class Singleton<UsartDriver>;

    public:
        void Init(uint32_t baudrate = 115200);
        uint32_t IsReadEnable(void);
        uint32_t IsWriteEnable(void);
        uint8_t Read(void);
        void Write(uint8_t c);
        void SetBase(RCC_TypeDef* rcc_addr, GPIO_TypeDef* gpio_addr, USART_TypeDef* usart_addr);
        static RCC_TypeDef* rcc_base_;
        static USART_TypeDef* usart_base_;

    private:
        typedef UsartPinSet<USART_2> PinSet;
        typedef UsartTraits<USART_2> Traits;
        
        void EnableClocks(void);
        void ConfigurePin(void);
        void ConfigureUsart(uint32_t baudrate);

    protected:
        UsartDriver();
};

#endif
