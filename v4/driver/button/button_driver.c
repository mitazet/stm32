#include "button_driver.h"
#include "stm32f303x8.h"
#include "printf.h"

static void InitEXTI()
{
    SYSCFG->EXTICR[0] = SYSCFG_EXTICR1_EXTI0_PB;
    EXTI->IMR = EXTI_IMR_MR0;
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

void ButtonInitIntr()
{
    InitGPIO();
    InitEXTI();
}

void EXTI0_IRQHandler()
{
    EXTI->PR |= EXTI_PR_PR0;
    printf("pushed\n");
}
