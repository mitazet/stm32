/* main.c */

#include <stdlib.h>
#include "stm32f303x8.h"
#include "printf.h"
#include "rtos.h"
#include "flash_driver.h"

/* ユーザスレッド */
static int flash(int argc, char *argv[])
{  
    flash_result_t ret;

    FlashDriver FlashDrv;

    ret = FlashDrv.PageErase((uint8_t*)0x0800F800);
    if(ret != FLASH_RESULT_OK){
        printf((char*)"ERASE ERROR!!\n");
        return 0;
    }

    printf((char*)"0x%X%X\n", FlashDrv.Read((uint8_t*)0x0800F801), FlashDrv.Read((uint8_t*)0x0800F800));

    ret = FlashDrv.Write((uint16_t*)0x0800F800, 0xBEEF);
    if(ret != FLASH_RESULT_OK){
        printf((char*)"WRITE ERROR!!\n");
        return 0;
    }

    printf((char*)"0x%X%X\n", FlashDrv.Read((uint8_t*)0x0800F801), FlashDrv.Read((uint8_t*)0x0800F800));

    return 0;
}

int main(void)
    __attribute__ ((section (".entry_point")));
int main(void){  

    printf((char*)"main boot succeed!\n");  

    SysTick_Config(SystemCoreClock/10); // 1/10秒（=100ms）ごとにSysTick割り込み
    NVIC_SetPriority(SVCall_IRQn, 0x80);    // SVCの優先度は中ほど
    NVIC_SetPriority(SysTick_IRQn, 0xc0);   // SysTickの優先度はSVCより低く
    NVIC_SetPriority(PendSV_IRQn, 0xff);    // PendSVの優先度を最低にしておく

    __enable_irq(); // enable interrupt

    RtosInit();
    RtosThreadCreate((rtos_func_t)flash, (char*)"flash", 0, 0x100, 0, NULL);

    /* OSの動作開始 */  
    RtosStart();  

    /* ここには戻ってこない */  
    return 0;
}

