// USART ----------------------------------------------------------------------
#include "usart_driver.h"
#include "io_reg.h"

// Default:USART2
static RCC_TypeDef* rccAddress = RCC;
static GPIO_TypeDef* gpioAddress = GPIOA;
static USART_TypeDef* usartAddress = USART2;

void UsartCreate(RCC_TypeDef* rcc_addr, GPIO_TypeDef* gpio_addr, USART_TypeDef* usart_addr)
{
    rccAddress = rcc_addr;
    gpioAddress = gpio_addr;
    usartAddress = usart_addr;       
}

static void EnableUsart2(void)
{
    SetBit(&rccAddress->AHBENR, RCC_AHBENR_GPIOAEN);

    ClearReg(&gpioAddress->MODER);
    ClearReg(&gpioAddress->AFR[0]);
    ClearReg(&gpioAddress->AFR[1]);

    // PA2 and PA15 as AF
    SetBit(&gpioAddress->MODER, GPIO_MODER_MODER2_1 | GPIO_MODER_MODER15_1);

    // Select AF7 (USART2) for PA2 and PA15
    uint32_t val_af7 = 0x7;
    SetBit(&gpioAddress->AFR[0], val_af7 << GPIO_AFRL_AFRL2_Pos);
    SetBit(&gpioAddress->AFR[1], val_af7 << GPIO_AFRH_AFRH7_Pos);
}

static void ConfigureUsart(void)
{
    // Distribute clock to USART2
    SetBit(&rccAddress->APB1ENR, RCC_APB1ENR_USART2EN);

    // Configure baudrate
    WriteReg(&usartAddress->BRR, 8000000L/115200L);

    // Eable TX, RX and enable USART
    SetBit(&usartAddress->CR1, USART_CR1_RE | USART_CR1_TE | USART_CR1_UE);
}

void UsartInit(void)
{
    EnableUsart2();
    ConfigureUsart();
}

uint32_t UsartIsReadEnable(void)
{
    return ReadBit(&usartAddress->ISR, USART_ISR_RXNE);
}

uint32_t UsartIsWriteEnable(void)
{
    return ReadBit(&usartAddress->ISR, USART_ISR_TXE);
}

uint8_t UsartRead(void)
{
    // Wait for a char on the USART
    while (!UsartIsReadEnable());
    return ReadReg(&usartAddress->RDR);
}

void UsartWrite(uint8_t c)
{
    // Wait for a char on the USART
    while (!UsartIsWriteEnable());
    return WriteReg(&usartAddress->TDR, c);
}

