/* main.c */

#include <stdlib.h>
#include "stm32f303x8.h"
#include "lib.h"
#include "printf.h"
#include "rtos.h"
#include "flash_driver.h"
#include "led_driver.h"
#include "timer0_driver.h"
#include "init.h"

static void ToggleLed()
{
    LedDriver& LedDrv = LedDriver::GetInstance();

    if(LedDrv.IsOn()){
        LedDrv.Off();
    }else{
        LedDrv.On();
    }

    Timer0Driver& Timer0 = Timer0Driver::GetInstance();

    int timeout_sec = 3;
    Timer0.Start_sec(timeout_sec, ToggleLed);
}

/* ユーザスレッド */
static int Led(int argc, char *argv[])
{  
    ToggleLed();

    return 0;
}

int main(void)
    __attribute__ ((section (".entry_point")));
int main(void)
{  
    Init();

    printf((char*)"main boot succeed!\n");  

    RtosThreadCreate((rtos_func_t)Led, (char*)"Led", 0, 0x100, 0, NULL);

    /* OSの動作開始 */  
    RtosStart();  

    /* ここには戻ってこない */  
    return 0;
}
