#ifndef __TFP_PRINTF__
#define __TFP_PRINTF__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdarg.h>

extern void init_printf(void (*putf)(char));

extern void tfp_printf(char *fmt, ...);

extern void tfp_format(void (*putf)(char), char *fmt, va_list va);

#define printf tfp_printf

#ifdef __cplusplus
}
#endif

#endif
