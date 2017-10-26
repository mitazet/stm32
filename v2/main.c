#include "stm32f303x8.h"
#include "usart_driver.h"
#include "printf.h"

// Main -----------------------------------------------------------------------
int main(void)
{
    InitUSART();
    
    init_printf(PutcUSART);

    printf("Hello World\n");
    printf("Hello World\n");
    printf("Hello World\n");
    
	return 0;
}
