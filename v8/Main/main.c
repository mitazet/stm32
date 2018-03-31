#include "printf.h"
#include "timer_driver.h"
#include "rtos.h"

static int test(void)
{
    printf("test!!\n");
}

int main(void)
	__attribute__ ((section (".entry_point")));

// Main -----------------------------------------------------------------------
int main(void)
{
    __disable_irq(); // disable interrupt

    TimerInit();

    __enable_irq(); // enable interrupt

    printf("Main Program booted!!\n");
    printf("test address:0x%x\n",test);

    RtosStart(test, "start", 0x100, 0, NULL);

    printf("Main Program end!!\n");
    while(1);

	return 0;
}
