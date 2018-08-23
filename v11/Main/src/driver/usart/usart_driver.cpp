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
    rcc_base_ = RCC;
    usart_base_ = USART2;       
    tx_={{GPIOA, 2}, ALT_FUNC_USART2};
    rx_={{GPIOA, 15}, ALT_FUNC_USART2};
    clk_en = RCC_AHBENR_GPIOAEN;
}

void UsartDriver::SetBase(RCC_TypeDef* rcc_addr, GPIO_TypeDef* gpio_addr, USART_TypeDef* usart_addr)
{
    rcc_base_ = rcc_addr;
    usart_base_ = usart_addr;       
    tx_.pin.port = gpio_addr;
    rx_.pin.port = gpio_addr;
}

void UsartDriver::EnableUsart(void)
{
    // Enable GPIO Clock
    SetBit(&rcc_base_->AHBENR, clk_en);

    ClearReg(&tx_.pin.port->MODER);
    ClearReg(&tx_.pin.port->AFR[0]);
    ClearReg(&tx_.pin.port->AFR[1]);

    ClearReg(&rx_.pin.port->MODER);
    ClearReg(&rx_.pin.port->AFR[0]);
    ClearReg(&rx_.pin.port->AFR[1]);

    // Tx GPIO as AF 
    SetBit(&tx_.pin.port->MODER, MODER_ALTERNATE << tx_.pin.no * 2);
    // Rx GPIO  as AF
    SetBit(&rx_.pin.port->MODER, MODER_ALTERNATE << rx_.pin.no * 2);

    uint32_t shift_x4;
    // Select AF7 (USART) for Tx GPIO
    shift_x4 = (tx_.pin.no % 8) * 4;
    SetBit(&tx_.pin.port->AFR[tx_.pin.no/8], tx_.alt_func << shift_x4);
    // Select AF7 (USART) for Rx GPIO
    shift_x4 = (rx_.pin.no % 8) * 4;
    SetBit(&rx_.pin.port->AFR[rx_.pin.no/8], rx_.alt_func << shift_x4);
}

void UsartDriver::ConfigureUsart(void)
{
    // Distribute clock to USART2
    SetBit(&rcc_base_->APB1ENR, RCC_APB1ENR_USART2EN);

    // Configure baudrate
    WriteReg(&usart_base_->BRR, 8000000L/115200L);

    // Eable TX, RX and enable USART
    SetBit(&usart_base_->CR1, USART_CR1_RE | USART_CR1_TE | USART_CR1_UE);
}

void UsartDriver::Init(void)
{
    EnableUsart();
    ConfigureUsart();
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
