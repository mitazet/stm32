#include "printf.h"
#include "timer_driver.h"

int main(void)
    __attribute__ ((section (".entry_point")));

void timeup_function(void)
{
    printf("timeup!!\n");
}

// Main -----------------------------------------------------------------------
int main(void)
{
    __disable_irq(); // disable interrupt

    TimerInit();

    __enable_irq(); // enable interrupt

    printf("Main Program booted!!\n");

    TimerAdd_sec(1, timeup_function);

    while(1);

    return 0;
}
