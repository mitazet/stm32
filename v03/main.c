#include "usart_driver.h"
#include "printf.h"
#include "button_driver.h"

// Main -----------------------------------------------------------------------
int main(void)
{
    InitUSART();
    InitButtonIntr();
    init_printf(PutcUSART);

    printf("Button Input Test\n");

    while(1);

    return 0;
}
