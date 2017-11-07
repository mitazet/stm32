#include "button_driver.h"
#include "stm32f303x8.h"
#include "printf.h"

static void InitEXTI()
{
    SYSCFG->EXTICR[0] = 0x00000001;
    EXTI->IMR = 0x00000001;
    EXTI->FTSR = EXTI_FTSR_TR0;
    NVIC_EnableIRQ(EXTI0_IRQn);
}

static void InitGPIO()
{
    // distibute clock
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
    
    // GPIOB0 Input mode
    GPIOB->MODER = 0;
}

void InitButtonIntr()
{
    InitEXTI();
    InitGPIO();
}

void EXTI0_IRQHandler()
{
    printf("pushed\n");
}
