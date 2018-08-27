#include "stm32f303x8.h"

void ConfigureTIM6(void)
{
    RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;
    TIM6->PSC = 9999;
    TIM6->ARR = 4200 * 2;
    TIM6->CNT = 0;
    TIM6->CR1 |= TIM_CR1_CEN;
}

void ConfigureGPIOB3(void)
{
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN;

    GPIOB->MODER = GPIO_MODER_MODER3_0;
    GPIOB->OTYPER = 0x0000;
    GPIOB->PUPDR = 0x000000000;
}

// Main -----------------------------------------------------------------------
int main(void)
{
    ConfigureTIM6();
    ConfigureGPIOB3();
    
    int i = 0;

    // Loop forever
    while(1){
        if(TIM6->SR){
            TIM6->SR = 0;
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
