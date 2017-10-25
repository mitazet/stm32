// USART ----------------------------------------------------------------------
#include "usart_driver.h"

static void EnablePinUSART(void)
{

    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

    GPIOA->MODER = 0;
    GPIOA->AFR[0] = 0;
    GPIOA->AFR[1] = 0;

    // PA2 and PA15 as AF
    GPIOA->MODER |= GPIO_MODER_MODER2_1;
    GPIOA->MODER |= GPIO_MODER_MODER15_1;

    // Select AF7 (USART2) for PA2 and PA15
	GPIOA->AFR[0] = (GPIOA->AFR[0] & 0xFFFFF0FF) | 0x700;
	GPIOA->AFR[1] = (GPIOA->AFR[1] & 0x0FFFFFFF) | 0x70000000;
}

static void ConfigureUSART(void)
{
	// Distribute clock to USART2
	RCC->APB1ENR |= RCC_APB1ENR_USART2EN;

	// Configure baudrate
	USART2->BRR  = 8000000L/115200L;

	// Eable TX, RX and enable USART
	USART2->CR1 |= USART_CR1_RE | USART_CR1_TE | USART_CR1_UE;
}

void InitUSART(void)
{
	EnablePinUSART();
	ConfigureUSART();
}

char ReadUSART(void)
{
	// Wait for a char on the UART
	while (!(USART2->ISR & USART_ISR_RXNE));
	return USART2->RDR;
}

void WriteUSART(char c)
{
	// Wait for a char on the UART
	while (!(USART2->ISR & USART_ISR_TXE));
	USART2->TDR = c;
}

void PutcUSART(char c)
{
    if(c == '\n'){
        WriteUSART('\r');
    }
    WriteUSART(c);
}


