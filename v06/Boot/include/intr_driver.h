#ifndef _INTR_DRIVER_H_
#define _INTR_DRIVER_H_

typedef enum{
    INTR_TYPE_USART,
    INTR_TYPE_TIMER,
    INTR_TYPE_NUM,
}intr_type_t;

extern int _common;
#define SOFTVEC_ADDR (&_common)

typedef void (*intr_handler_t)(void);

#define SOFTVECS ((intr_handler_t*)SOFTVEC_ADDR)

// Initialize software vector
int IntrInit(void);
// Set software vector handler
void IntrHandlerSet(intr_type_t type, intr_handler_t handler);
// Execute Intterupt
void IntrHandlerExec(intr_type_t type);

void USART2_IRQHandler(void);
void TIM6_DAC1_IRQHandler(void);

#endif
