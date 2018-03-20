// USART ----------------------------------------------------------------------
#include "usart_driver.h"
#include "reg_io.h"

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

static void EnablePin(void)
{
    RegWrite((uint32_t*)&rccAddress->AHBENR, RCC_AHBENR_GPIOAEN);

    RegClear((uint32_t*)&gpioAddress->MODER);
    RegClear((uint32_t*)&gpioAddress->AFR[0]);
    RegClear((uint32_t*)&gpioAddress->AFR[1]);

    // PA2 and PA15 as AF
    RegWrite((uint32_t*)&gpioAddress->MODER, GPIO_MODER_MODER2_1 | GPIO_MODER_MODER15_1);

    // Select AF7 (USART2) for PA2 and PA15
	RegWrite((uint32_t*)&gpioAddress->AFR[0], 0x700);
	RegWrite((uint32_t*)&gpioAddress->AFR[1], 0x70000000);
}

static void Configure(void)
{
	// Distribute clock to USART2
	RegWrite((uint32_t*)&rccAddress->APB1ENR, RCC_APB1ENR_USART2EN);

	// Configure baudrate
	RegWrite((uint32_t*)&usartAddress->BRR, 8000000L/115200L);

	// Eable TX, RX and enable USART
	RegWrite((uint32_t*)&usartAddress->CR1, USART_CR1_RE | USART_CR1_TE | USART_CR1_UE);
}

void UsartInit(void)
{
	EnablePin();
	Configure();
}

uint32_t UsartIsReadEnable(void)
{
	return RegRead((uint32_t*)&usartAddress->ISR) & USART_ISR_RXNE;
}

uint32_t UsartIsWriteEnable(void)
{
	return RegRead((uint32_t*)&usartAddress->ISR) & USART_ISR_TXE;
}

uint8_t UsartRead(void)
{
	// Wait for a char on the USART
	while (!UsartIsReadEnable());
	return RegRead((uint32_t*)&usartAddress->RDR);
}

void UsartWrite(uint8_t c)
{
	// Wait for a char on the USART
	while (!UsartIsWriteEnable());
	return RegWrite((uint32_t*)&usartAddress->TDR, c);
}

