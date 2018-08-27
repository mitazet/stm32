#include "intr_driver.h"
#include <stdlib.h>

int IntrInit(void)
{
    int type;
    for(type = 0; type < INTR_TYPE_NUM; type++){
        IntrHandlerSet(type, NULL);
    }
    return 0;
}

void IntrHandlerSet(intr_type_t type, intr_handler_t handler)
{
    SOFTVECS[type] = handler;
}

void IntrHandlerExec(intr_type_t type)
{
    intr_handler_t handler = SOFTVECS[type];
    if(handler){
        handler();
    }
}

void USART2_IRQHandler(void)
{
    IntrHandlerExec(INTR_TYPE_USART);
}

void TIM6_DAC1_IRQHandler(void)
{
    IntrHandlerExec(INTR_TYPE_TIMER);
}
