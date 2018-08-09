#include "led_driver.h"
#include "io_reg.h"

LedDriver::LedDriver()
{
    rcc_base_ = RCC;
    gpio_base_ = GPIOB;
}

void LedDriver::SetBase(RCC_TypeDef* rcc_addr, GPIO_TypeDef* gpio_addr)
{
    rcc_base_ = rcc_addr;
    gpio_base_ = gpio_addr;
}

void LedDriver::Init()
{
    SetBit(&rcc_base_->AHBENR, RCC_AHBENR_GPIOBEN);

    // set General purpose output mode
    ClearBit(&gpio_base_->MODER, GPIO_MODER_MODER3_Msk);
    SetBit(&gpio_base_->MODER, GPIO_MODER_MODER3_0);

    // set Output push-pull
    ClearBit(&gpio_base_->OTYPER, GPIO_OTYPER_OT_3);

    // set No pull-up, pull-down
    ClearBit(&gpio_base_->PUPDR, GPIO_PUPDR_PUPDR3_Msk);

    // set initial Output data
    ClearBit(&gpio_base_->ODR, GPIO_ODR_3);
}

void LedDriver::On()
{
    SetBit(&gpio_base_->ODR, GPIO_ODR_3);
}

void LedDriver::Off()
{
    ClearBit(&gpio_base_->ODR, GPIO_ODR_3);
}
