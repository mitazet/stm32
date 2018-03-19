// USART ----------------------------------------------------------------------
#include "usart_driver.h"
#include "reg_io.h"

static RCC_TypeDef* rccAddress = RCC;
static GPIO_TypeDef* gpioAddress = GPIOA;
static USART_TypeDef* usartAddress = USART2;

void UsartCreate(RCC_TypeDef* rcc_addr, GPIO_TypeDef* gpio_addr, USART_TypeDef* usart_addr)
{
    rccAddress = rcc_addr;
    gpioAddress = gpio_addr;
    usartAddress = usart_addr;       
}

static void UsartEnablePin(void)
{
    rccAddress->AHBENR |= RCC_AHBENR_GPIOAEN;

    gpioAddress->MODER = 0;
    gpioAddress->AFR[0] = 0;
    gpioAddress->AFR[1] = 0;

    // PA2 and PA15 as AF
    gpioAddress->MODER |= GPIO_MODER_MODER2_1;
    gpioAddress->MODER |= GPIO_MODER_MODER15_1;

    // Select AF7 (USART2) for PA2 and PA15
	gpioAddress->AFR[0] = (gpioAddress->AFR[0] & 0xFFFFF0FF) | 0x700;
	gpioAddress->AFR[1] = (gpioAddress->AFR[1] & 0x0FFFFFFF) | 0x70000000;
}

static void UsartConfigure(void)
{
	// Distribute clock to USART2
	rccAddress->APB1ENR |= RCC_APB1ENR_USART2EN;

	// Configure baudrate
	usartAddress->BRR  = 8000000L/115200L;

	// Eable TX, RX and enable USART
	usartAddress->CR1 |= USART_CR1_RE | USART_CR1_TE | USART_CR1_UE;
}

void UsartInit(void)
{
	UsartEnablePin();
	UsartConfigure();
}

long UsartIsReadEnable(void)
{
	return RegRead((uint32_t*)&usartAddress->ISR) & USART_ISR_RXNE;
}

long UsartIsWriteEnable(void)
{
	return RegRead((uint32_t*)&usartAddress->ISR) & USART_ISR_TXE;
}

char UsartRead(void)
{
	// Wait for a char on the USART
	while (!UsartIsReadEnable());
	return RegRead((uint32_t*)&usartAddress->RDR);
}

void UsartWrite(char c)
{
	// Wait for a char on the USART
	while (!UsartIsWriteEnable());
	return RegWrite((uint32_t*)&usartAddress->TDR, c);
}

