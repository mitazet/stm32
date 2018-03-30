#include "printf.h"
#include "timer_driver.h"
#include "intr_driver.h"

int main(void)
	__attribute__ ((section (".entry_point")));

// Main -----------------------------------------------------------------------
int main(void)
{
    __disable_irq(); // disable interrupt

    TimerInit();

    __enable_irq(); // enable interrupt

    printf("Main Program booted!!\n");

    while(1);

	return 0;
}
