// USART ----------------------------------------------------------------------
#include "usart_driver.h"
#include "io_reg.h"
#include <stdlib.h>

extern uint32_t SystemCoreClock;

RCC_TypeDef* UsartDriver::rcc_base_ = Traits::rcc_base;
USART_TypeDef* UsartDriver::usart_base_ = Traits::usart_base;       
template<> GPIO_TypeDef* UsartPinSet<USART_2>::PinTx::gpio_base = UsartPinSet<USART_2>::PinTx::GPIOx_BASE;
template<> GPIO_TypeDef* UsartPinSet<USART_2>::PinRx::gpio_base = UsartPinSet<USART_2>::PinTx::GPIOx_BASE;

uint8_t UsartRead(void)
{
    return UsartDriver::GetInstance().Read();
}

void UsartWrite(uint8_t c)
{
    UsartDriver::GetInstance().Write(c);
}

UsartDriver::UsartDriver()
{
}

void UsartDriver::SetBase(RCC_TypeDef* rcc_addr, GPIO_TypeDef* gpio_addr, USART_TypeDef* usart_addr)
{
    rcc_base_ = rcc_addr;
    usart_base_ = usart_addr;       
    PinSet::PinTx::gpio_base = gpio_addr;
    PinSet::PinRx::gpio_base = gpio_addr;
}

void UsartDriver::EnableClocks(void)
{
    PinSet::EnableClock(rcc_base_);
    Traits::EnableClock(rcc_base_);
}

void UsartDriver::ConfigurePin(void)
{
    PinSet::PinTx::Alternate(PinSet::ALT_FUNC_USART);
    PinSet::PinRx::Alternate(PinSet::ALT_FUNC_USART);
}

void UsartDriver::ConfigureUsart(uint32_t baudrate)
{
    // Configure baudrate
    WriteReg(&usart_base_->BRR, SystemCoreClock/baudrate);

    // Eable TX, RX and enable USART
    SetBit(&usart_base_->CR1, USART_CR1_RE | USART_CR1_TE | USART_CR1_UE);
}

void UsartDriver::Init(uint32_t baudrate)
{
    EnableClocks();
    ConfigurePin();
    ConfigureUsart(baudrate);
}

uint32_t UsartDriver::IsReadEnable(void)
{
    return ReadBit(&usart_base_->ISR, USART_ISR_RXNE);
}

uint32_t UsartDriver::IsWriteEnable(void)
{
    return ReadBit(&usart_base_->ISR, USART_ISR_TXE);
}

uint8_t UsartDriver::Read(void)
{
    // Wait for a char on the USART
    while (!IsReadEnable());
    return ReadReg(&usart_base_->RDR);
}

void UsartDriver::Write(uint8_t c)
{
    // Wait for a char on the USART
    while (!IsWriteEnable());
    return WriteReg(&usart_base_->TDR, c);
}
