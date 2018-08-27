#ifndef __USART_DRIVER__
#define __USART_DRIVER__

#include "stm32f303x8.h"

extern void UsartInit(void);
extern char UsartRead(void);
extern void UsartWrite(char c);
extern void UsartPutc(char c);

#endif
