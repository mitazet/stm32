#ifndef __USART_DRIVER__
#define __USART_DRIVER__

#include "stm32f303x8.h"

extern void InitUSART(void);
extern char ReadUSART(void);
extern void WriteUSART(char c);
extern void PutcUSART(char c);

#endif
