#ifndef __RTOS_H__
#define __RTOS_H__

#include "stm32f303x8.h"

typedef uint32_t rtos_thread_id_t;
typedef int (*rtos_func_t)(void);
typedef void (*rtos_handler_t)(void);

typedef enum {
    RTOS_SYSCALL_CHG_UNPRIVILEGE = 0,
	RTOS_SYSCALL_SLEEP,
	RTOS_SYSCALL_WAKEUP,
	RTOS_SYSCALL_CHG_PRIORITY
}rtos_syscall_type_t;

typedef struct{
    union{
        struct{
            rtos_func_t func;
            char* name;
			int priority;
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

extern void SVC_Handler(void);
extern void PendSV_Handler(void);
extern void SysTick_Handler(void);

// System call
rtos_thread_id_t RtosRun(rtos_func_t func, char* name, int stacksize, int argc, char* argv[]);
void RtosTerminate(void);

// Library function
void RtosInit(void);
void RtosStart(void);
void RtosSysdown(void);
void RtosSyscall(rtos_syscall_type_t type, rtos_syscall_param_t* param);
void RtosThreadCreate(rtos_func_t func, char* name, int priority, int stacksize, int argc, char* argv[]);

// User Thread
int test08_1_main(int argc, char* argv[]);

#endif
