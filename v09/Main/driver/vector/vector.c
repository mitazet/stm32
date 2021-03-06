#include "rtos.h"
#include "timer_driver.h"
#include <stdlib.h>

void (*vectors[])(void)
    __attribute__ ((section (".app_vector")));

void (*vectors[])(void) = {
    NULL, // _estack
    NULL, // Reset_Handler
    NULL, // NMI_Handler
    NULL, // HardFault_Handler
    NULL, // MemManage_Handler
    NULL, // BusFault_Handler
    NULL, // UsageFault_Handler
    NULL, // 0
    NULL, // 0
    NULL, // 0
    NULL, // 0
    SVC_Handler, // SVC_Handler
    NULL, // DebugMon_Handler
    NULL, // 0
    PendSV_Handler, // PendSV_Handler
    SysTick_Handler, // SysTick_Handler
    NULL, // WWDG_IRQHandler
    NULL, // PVD_IRQHandler
    NULL, // TAMP_STAMP_IRQHandler
    NULL, // RTC_WKUP_IRQHandler
    NULL, // FLASH_IRQHandler
    NULL, // RCC_IRQHandler
    NULL, // EXTI0_IRQHandler
    NULL, // EXTI1_IRQHandler
    NULL, // EXTI2_TSC_IRQHandler
    NULL, // EXTI3_IRQHandler
    NULL, // EXTI4_IRQHandler
    NULL, // DMA1_Channel1_IRQHandler
    NULL, // DMA1_Channel2_IRQHandler
    NULL, // DMA1_Channel3_IRQHandler
    NULL, // DMA1_Channel4_IRQHandler
    NULL, // DMA1_Channel5_IRQHandler
    NULL, // DMA1_Channel6_IRQHandler
    NULL, // DMA1_Channel7_IRQHandler
    NULL, // ADC1_2_IRQHandler
    NULL, // CAN_TX_IRQHandler
    NULL, // CAN_RX0_IRQHandler
    NULL, // CAN_RX1_IRQHandler
    NULL, // CAN_SCE_IRQHandler
    NULL, // EXTI9_5_IRQHandler
    NULL, // TIM1_BRK_TIM15_IRQHandler
    NULL, // TIM1_UP_TIM16_IRQHandler
    NULL, // TIM1_TRG_COM_TIM17_IRQHandler
    NULL, // TIM1_CC_IRQHandler
    NULL, // TIM2_IRQHandler
    NULL, // TIM3_IRQHandler
    NULL, // 0
    NULL, // I2C1_EV_IRQHandler
    NULL, // I2C1_ER_IRQHandler
    NULL, // 0
    NULL, // 0
    NULL, // SPI1_IRQHandler
    NULL, // 0
    NULL, // USART1_IRQHandler
    NULL, // USART2_IRQHandler
    NULL, // USART3_IRQHandler
    NULL, // EXTI15_10_IRQHandler
    NULL, // RTC_Alarm_IRQHandler
    NULL, // 0
    NULL, // 0
    NULL, // 0
    NULL, // 0
    NULL, // 0
    NULL, // 0
    NULL, // 0
    NULL, // 0
    NULL, // 0
    NULL, // 0
    NULL, // 0
    NULL, // 0
    TIM6_DAC1_IRQHandler, // TIM6_DAC1_IRQHandler
    NULL, // TIM7_DAC2_IRQHandler
    NULL, // 0
    NULL, // 0
    NULL, // 0
    NULL, // 0
    NULL, // 0
    NULL, // 0
    NULL, // 0
    NULL, // 0
    NULL, // COMP2_IRQHandler
    NULL, // COMP4_6_IRQHandler
    NULL, // 0
    NULL, // 0
    NULL, // 0
    NULL, // 0
    NULL, // 0
    NULL, // 0
    NULL, // 0
    NULL, // 0
    NULL, // 0
    NULL, // 0
    NULL, // 0
    NULL, // 0
    NULL, // 0
    NULL, // 0
    NULL, // 0
    NULL  // FPU_IRQHandler
};
