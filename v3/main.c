#include "stm32f303x8.h"
#include "usart_driver.h"
#include "printf.h"
#include "button_driver.h"

// Main -----------------------------------------------------------------------
int main(void)
{
	char c;

    InitUSART();
    InitButtonIntr();
    init_printf(PutcUSART);

    printf("Button Input Test\n");

    while(1);
#if 0
    while(1){
        if(GPIOB->IDR & GPIO_IDR_0){
        }else{
            printf("pushed\n");
        }
    }
#endif
	return 0;
}
