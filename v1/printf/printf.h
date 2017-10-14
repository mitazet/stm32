#ifndef __TFP_PRINTF__
#define __TFP_PRINTF__

#include <stdarg.h>

void init_printf(void (*putf)(char));

void tfp_printf(char *fmt, ...);

void tfp_format(void (*putf)(char), char *fmt, va_list va);

#define printf tfp_printf

#endif
