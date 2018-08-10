// USART ----------------------------------------------------------------------
#include "usart_driver.h"
#include "io_reg.h"
#include <stdlib.h>

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
    rccAddress_ = RCC;
    gpioAddress_ = GPIOA;
    usartAddress_ = USART2;       
}

void UsartDriver::SetBase(RCC_TypeDef* rcc_addr, GPIO_TypeDef* gpio_addr, USART_TypeDef* usart_addr)
{
    rccAddress_ = rcc_addr;
    gpioAddress_ = gpio_addr;
    usartAddress_ = usart_addr;       
}

void UsartDriver::EnableUsart2(void)
{
    SetBit(&rccAddress_->AHBENR, RCC_AHBENR_GPIOAEN);

    ClearReg(&gpioAddress_->MODER);
    ClearReg(&gpioAddress_->AFR[0]);
    ClearReg(&gpioAddress_->AFR[1]);

    // PA2 and PA15 as AF
    SetBit(&gpioAddress_->MODER, GPIO_MODER_MODER2_1 | GPIO_MODER_MODER15_1);

    // Select AF7 (USART2) for PA2 and PA15
    uint32_t val_af7 = 0x7;
    SetBit(&gpioAddress_->AFR[0], val_af7 << GPIO_AFRL_AFRL2_Pos);
    SetBit(&gpioAddress_->AFR[1], val_af7 << GPIO_AFRH_AFRH7_Pos);
}

void UsartDriver::ConfigureUsart(void)
{
    // Distribute clock to USART2
    SetBit(&rccAddress_->APB1ENR, RCC_APB1ENR_USART2EN);

    // Configure baudrate
    WriteReg(&usartAddress_->BRR, 8000000L/115200L);

    // Eable TX, RX and enable USART
    SetBit(&usartAddress_->CR1, USART_CR1_RE | USART_CR1_TE | USART_CR1_UE);
}

void UsartDriver::Init(void)
{
    EnableUsart2();
    ConfigureUsart();
}

uint32_t UsartDriver::IsReadEnable(void)
{
    return ReadBit(&usartAddress_->ISR, USART_ISR_RXNE);
}

uint32_t UsartDriver::IsWriteEnable(void)
{
    return ReadBit(&usartAddress_->ISR, USART_ISR_TXE);
}

uint8_t UsartDriver::Read(void)
{
    // Wait for a char on the USART
    while (!IsReadEnable());
    return ReadReg(&usartAddress_->RDR);
}

void UsartDriver::Write(uint8_t c)
{
    // Wait for a char on the USART
    while (!IsWriteEnable());
    return WriteReg(&usartAddress_->TDR, c);
}
