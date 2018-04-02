#ifndef __SYSCALL_H__
#define __SYSCALL_H__

#include "rtos_def.h"

typedef enum {
    RTOS_SYSCALL_TYPE_RUN = 0,
    RTOS_SYSCALL_TYPE_EXIT,
}rtos_syscall_type_t;

typedef struct{
    union{
        struct{
            rtos_func_t func;
            char* name;
            int stacksize;
            int argc;
            char **argv;
            rtos_thread_id_t ret;
        }run;
        struct{
            int dummy;
        }exit;
    }un;
}rtos_syscall_param_t;

#endif
