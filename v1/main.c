#include "stm32f30x.h"

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
    GPIOB->ODR = GPIO_ODR_3;
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
