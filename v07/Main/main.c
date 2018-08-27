#include "printf.h"
#include "timer_driver.h"
#include "intr_driver.h"

int main(void)
    __attribute__ ((section (".entry_point")));

void timeup_function(void)
{
    printf("timeup!!\n");
}

void SvcHandlerTest(void)
{
    printf("SVC Handler Test!!\n");
}

// Main -----------------------------------------------------------------------
int main(void)
{
    __disable_irq(); // disable interrupt

    TimerInit();
    IntrHandlerSet(INTR_TYPE_SVC, SvcHandlerTest);

    __enable_irq(); // enable interrupt

    printf("Main Program booted!!\n");

    __asm volatile ("svc 0");

    while(1);

    return 0;
}
