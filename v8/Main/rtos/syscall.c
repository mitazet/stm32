#include "syscall.h"
#include "rtos.h"
#if 0
rtos_thread_id_t RtosRun(rtos_func_t func, char* name, int stacksize, int argc, char *argv[])
{
    rtos_syscall_param_t param;
    param.un.run.func = func;
    param.un.run.name = name;
    param.un.run.stacksize = stacksize;
    param.un.run.argc = argc;
    param.un.run.argv = argv;
    RtosSyscall(RTOS_SYSCALL_TYPE_RUN, &param);

    return param.un.run.ret;
}

void RtosTerminate(void)
{
    RtosSyscall(RTOS_SYSCALL_TYPE_EXIT, NULL);
}
#endif
