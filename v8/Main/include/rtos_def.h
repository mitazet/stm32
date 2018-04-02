#ifndef __RTOS_DEFINE_H__
#define __RTOS_DEFINE_H__

#include "stm32f303x8.h"
#include <stdlib.h>

typedef uint32_t rtos_thread_id_t;
typedef int (*rtos_func_t)(void);
typedef void (*rtos_handler_t)(void);

#endif
