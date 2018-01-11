#include "usart_driver.h"
#include "printf.h"
#include "button_driver.h"
#include "timer_driver.h"

void timeup_function(void)
{
	printf("timeup!!\n");
}

// Main -----------------------------------------------------------------------
int main(void)
{
    UsartInit();
    ButtonInitIntr();
	TimerInit();

    init_printf(UsartPutc);

	TimerAdd_sec(1, timeup_function);

	while(1);

	return 0;
}
