#include <stm32f30x.h>
#include <stdio.h>
#include "printf.h"

void SystemInit(void)
{
  /* FPU settings ------------------------------------------------------------*/
  #if (__FPU_PRESENT == 1) && (__FPU_USED == 1)
    SCB->CPACR |= ((3UL << 10*2)|(3UL << 11*2));  /* set CP10 and CP11 Full Access */
  #endif

  /* Reset the RCC clock configuration to the default reset state ------------*/
  /* Set HSION bit */
  RCC->CR |= 0x00000001U;

  /* Reset CFGR register */
  RCC->CFGR &= 0xF87FC00CU;

  /* Reset HSEON, CSSON and PLLON bits */
  RCC->CR &= 0xFEF6FFFFU;

  /* Reset HSEBYP bit */
  RCC->CR &= 0xFFFBFFFFU;

  /* Reset PLLSRC, PLLXTPRE, PLLMUL and USBPRE bits */
  RCC->CFGR &= 0xFF80FFFFU;

  /* Reset PREDIV1[3:0] bits */
  RCC->CFGR2 &= 0xFFFFFFF0U;

  /* Reset USARTSW[1:0], I2CSW and TIMs bits */
  RCC->CFGR3 &= 0xFF00FCCCU;

  /* Disable all interrupts */
  RCC->CIR = 0x00000000U;

#ifdef VECT_TAB_SRAM
  //SCB->VTOR = SRAM_BASE | VECT_TAB_OFFSET; /* Vector Table Relocation in Internal SRAM */
#else
  //SCB->VTOR = FLASH_BASE | VECT_TAB_OFFSET; /* Vector Table Relocation in Internal FLASH */
#endif
}

void ConfigureLEDs(void)
{
	// Enable the GPIO clock for ports E (LEDs) and A (USART1, SPI1)
	//RCC->AHBENR |= RCC_AHBENR_GPIOEEN | RCC_AHBENR_GPIOAEN;
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

	// Configure the GPIOs for the on-board LEDs as outputs
	// LEDs are on PE8-PE15
	//GPIOE->MODER  = (GPIOE->MODER & 0x0000FFFF) | 0x55550000;
}

void ConfigureTIM6(void)
{
    RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;
    TIM6->PSC = 9999;
    TIM6->ARR = 4200 * 2;
    TIM6->CNT = 0;
    TIM6->CR1 |= TIM_CR1_CEN;
}

void ConfigureGPIOB4(void)
{
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN;

    GPIOB->MODER = GPIO_MODER_MODER4_0;
    GPIOB->OTYPER = 0x0000;
    GPIOB->PUPDR = 0x000000000;
}

void SetGPIOB4(void)
{
    GPIOB->ODR |= GPIO_ODR_4;
}

void ConfigureGPIOB3(void)
{
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN;

    GPIOB->MODER = GPIO_MODER_MODER3_0;
    GPIOB->OTYPER = 0x0000;
    GPIOB->PUPDR = 0x000000000;
    GPIOB->ODR = GPIO_ODR_3;
}

void ConfigureGPIOA0(void)
{
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

    GPIOA->MODER = GPIO_MODER_MODER0_0;
    GPIOA->OTYPER = 0x0000;
    GPIOA->PUPDR = 0x000000000;
    GPIOA->ODR = GPIO_ODR_0;
}

// USART ----------------------------------------------------------------------


void ConfigureUSART2(void)
{
	// Distribute clock to USART2
	RCC->APB1ENR |= RCC_APB1ENR_USART2EN;

	// Select AF7 (USART2) for PA2 and PA15
    GPIOA->AFR[0] = (GPIOA->AFR[0] & 0xFFFFF0FF) | 0x700;
	GPIOA->AFR[1] = (GPIOA->AFR[1] & 0x0FFFFFFF) | 0x70000000;
	//GPIOA->AFR[1] = (GPIOA->AFR[1] & 0xFFFFF00F) | 0x770;

	// PA2 and PA15 as AF
	GPIOA->MODER |= GPIO_MODER_MODER2_1 | GPIO_MODER_MODER15_1;

	// Configure baudrate
	USART2->BRR  = 8000000L/115200L;

	// Eable TX, RX and enable USART
	USART2->CR1 |= USART_CR1_RE | USART_CR1_TE | USART_CR1_UE | USART_CR1_RXNEIE;

    NVIC_SetPriority(USART2_IRQn, 1);
    NVIC_EnableIRQ(USART2_IRQn);
}


char ReadUSART2(void)
{
	// Wait for a char on the UART
	while (!(USART2->ISR & USART_ISR_RXNE));
	return USART2->RDR;
}


void WriteUSART2(char c)
{
	// Wait for a char on the UART
	while (!(USART2->ISR & USART_ISR_TXE));
	USART2->TDR = c;
}

void myputc(char c)
{
    if(c == '\n'){
        WriteUSART2('\r');
    }
    WriteUSART2(c);
}

// Main -----------------------------------------------------------------------


int main(void)
{
	ConfigureLEDs();
	ConfigureUSART2();
    ConfigureTIM6();
    //ConfigureGPIOB4();
    ConfigureGPIOB3();
    //ConfigureGPIOA0();
    
    init_printf(myputc);
	//ConfigureGravis();

	int oldck = 0;
	int newck = 0;
    int i = 0;

    // Loop forever
    while(1){
        if(TIM6->SR){
            TIM6->SR = 0;
            printf("%d:Hello, World\n",i);
#if 0       
            if(GPIOA->ODR & GPIO_ODR_0){
                GPIOA->ODR &= ~GPIO_ODR_0;
            }else{
                GPIOA->ODR |= GPIO_ODR_0;
            }
#endif
            if(GPIOB->ODR & GPIO_ODR_3){
                GPIOB->ODR &= ~GPIO_ODR_3;
            }else{
                GPIOB->ODR |= GPIO_ODR_3;
            }
            i++;
        }
	}
	return 0;
}
