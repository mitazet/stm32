#ifndef __USART__
#define __USART__

#include "stm32f303x8.h"

extern void ConfigureUSART2(void);
extern char ReadUSART2(void);
extern void WriteUSART2(char c);

#endif
