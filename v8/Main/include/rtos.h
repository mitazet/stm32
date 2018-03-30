#ifndef __RTOS_H__
#define __RTOS_H__

#include "rtos_def.h"
#include "syscall.h"

extern void SVC_Handler(void);
extern void PendSV_Handler(void);

// System call
rtos_thread_id_t RtosRun(rtos_func_t func, char* name, int stacksize, int argc, char* argv[]);
void RtosTerminate(void);

// Library function
void RtosStart(rtos_func_t func, char* name, int stacksize, int argc, char* argv[]);
void RtosSysdown(void);
void RtosSyscall(rtos_syscall_type_t type, rtos_syscall_param_t* param);

// User Thread
int test08_1_main(int argc, char* argv[]);

#endif
