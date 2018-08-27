#if 0
#include "rtos.h"
#include "intr_driver.h"

#define THREAD_NUM 2
#define THREAD_NAME_SIZE 15

typedef struct _rtos_context{
    uint32_t sp;
}rtos_context;

typedef struct _rtos_thread{
    struct _rtos_thread *next;
    char name[THREAD_NAME_SIZE + 1];
    char *stack;

    struct{
        rtos_func_t func;
        int argc;
        char** argv;
    }init;

    struct{
        rtos_syscall_type_t type;
        rtos_syscall_param_t *param;
    }syscall;

    rtos_context context;
}rtos_thread;

static struct{
    rtos_thread *head;
    rtos_thread *tail;
}ready_que;

static rtos_thread *current;
static rtos_thread threads[THREAD_NUM];
static rtos_thread_handler_t handlers[INTR_TYPE_NUM];

//void dispatch(rtos_context *context);
#endif
