/* main.c */

#include <stdlib.h>
#include "stm32f303x8.h"
#include "printf.h"
#include "rtos.h"
#include "flash_driver.h"

/* ユーザスレッド */
static int flash(int argc, char *argv[])
{  
    FlashInit();
    FlashWrite((uint16_t*)0x08002000, 0xBEEF);
    printf("0x%X%X\n", FlashRead((uint8_t*)0x08002001), FlashRead((uint8_t*)0x08002000));
    FlashPageErase((uint8_t*)0x08002000);
	return 0;
}

int main(void)
	__attribute__ ((section (".entry_point")));
int main(void){  

	printf("main boot succeed!\n");  

	SysTick_Config(SystemCoreClock/10);	// 1/10秒（=100ms）ごとにSysTick割り込み
	NVIC_SetPriority(SVCall_IRQn, 0x80);	// SVCの優先度は中ほど
	NVIC_SetPriority(SysTick_IRQn, 0xc0);	// SysTickの優先度はSVCより低く
	NVIC_SetPriority(PendSV_IRQn, 0xff);	// PendSVの優先度を最低にしておく

    __enable_irq(); // enable interrupt

    RtosInit();
    RtosThreadCreate((rtos_func_t)flash, "flash", 0, 0x100, 0, NULL);

	/* OSの動作開始 */  
	RtosStart();  

	/* ここには戻ってこない */  
	return 0;
}
