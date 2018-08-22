#include "init.h"
#include "flash_driver.h"
#include "usart_driver.h"
#include "led_driver.h"
#include "timer_driver.h"
#include "lib.h"
#include "printf.h"
#include "rtos.h"

extern uint8_t UsartRead(void);
extern void UsartWrite(uint8_t c);

int Init(void)
{
    FlashDriver::GetInstance().Init();
    //UsartDriver::GetInstance().Init(); // Initialized in bootloader
    LedDriver::GetInstance().Init();
    TimerDriver::GetInstance().Init();

    init_myputc((void (*)(char))UsartWrite);
    init_mygetc((char (*)(void))UsartRead);

    init_printf(myputc);

    SysTick_Config(SystemCoreClock/10);     // 1/10秒（=100ms）ごとにSysTick割り込み
    NVIC_SetPriority(SVCall_IRQn, 0x80);    // SVCの優先度は中ほど
    NVIC_SetPriority(SysTick_IRQn, 0xc0);   // SysTickの優先度はSVCより低く
    NVIC_SetPriority(PendSV_IRQn, 0xff);    // PendSVの優先度を最低にしておく

    __enable_irq(); // enable interrupt

    RtosInit();

    return 0;
}
