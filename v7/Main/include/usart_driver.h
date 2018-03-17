#ifndef __USART_DRIVER__
#define __USART_DRIVER__

#include "stm32f303x8.h"

extern void UsartCreate(RCC_TypeDef* rcc_addr, GPIO_TypeDef* gpio_addr, USART_TypeDef* usart_addr);
extern void UsartInit(void);
extern long UsartIsReadEnable(void);
extern char UsartRead(void);
extern void UsartWrite(char c);

#endif
