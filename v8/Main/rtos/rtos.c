#include "printf.h"
#include "rtos.h"
#include "lib.h"

#define THREAD_NUM 2
#define THREAD_NAME_SIZE 15

#if 0
typedef struct _rtos_context{
    uint32_t sp;
}rtos_context;

typedef struct _rtos_thread{
    struct _rtos_thread *next;
    char name[THREAD_NAME_SIZE + 1];
    char *stack;

    rtos_func_t func;

    struct{
        rtos_syscall_type_t type;
        rtos_syscall_param_t *param;
    }syscall;

    rtos_context context;
}rtos_thread;

static struct{
    rtos_thread *head;
    rtos_thread *tail;
}readyque;

static rtos_thread *current;
static rtos_thread threads[THREAD_NUM];

static int getcurrent(void)
{
    if(current == NULL){
        return -1;
    }

    readyque.head = current->next;
    if(readyque.head == NULL){
        readyque.tail = NULL;
    }
    current->next = NULL;

    return 0;
}

static int putcurrent(void)
{
    if(current == NULL){
        return -1;
    }

    if(readyque.tail){
        readyque.tail->next = current;
    }else{
        readyque.head = current;
    }
    readyque.tail = current;

    return 0;
}

static void thread_end(void)
{
    RtosTerminate();
}

static void thread_init(rtos_thread *thp)
{
    thp->func();
}

static rtos_thread_id_t thread_run(rtos_func_t func, char* name, int stacksize, int argc, char* argv[])
{
    int i;
    rtos_thread *thp;
    uint32_t *sp;
    extern char _userstack; // from Linker script
    static char *thread_stack = &_userstack;

    for(i = 0; i < THREAD_NUM; i++){
        thp = &threads[i];
        if(!thp->func)
            break;
    }
    if(i == THREAD_NUM)
        return -1;

    memset(thp, 0, sizeof(*thp));

    strcpy(thp->name, name);
    thp->next = NULL;

    thp->func = func;

    /* allocate stack area */
    memset(thread_stack, 0, stacksize);
    thread_stack += stacksize;

    thp->stack = thread_stack; //stack bottom

    sp = (uint32_t*)thp->stack - 16;

    sp[15] = (uint32_t)0x01000000; // xPSR
    sp[14] = (uint32_t)func;
    
    thp->context.sp = (uint32_t)sp;
    printf("%s sp:0x%x\n", __FUNCTION__, sp);

    putcurrent();

    current = thp;
    putcurrent();

    return (rtos_thread_id_t)current;
}

static int thread_exit(void)
{
    printf("%s EXIT.\n", current->name);
    memset(current, 0, sizeof(*current));
    return 0;
}

static void call_functions(rtos_syscall_type_t type, rtos_syscall_param_t *p)
{
    switch(type){
        case RTOS_SYSCALL_TYPE_RUN:
            p->un.run.ret = thread_run(p->un.run.func, 
                                       p->un.run.name,
                                       p->un.run.stacksize,
                                       p->un.run.argc,
                                       p->un.run.argv);
            break;

        case RTOS_SYSCALL_TYPE_EXIT:
            thread_exit();
            break;
        default:
            break;
    }
}

static void syscall_proc(rtos_syscall_type_t type, rtos_syscall_param_t *p)
{
    getcurrent();
    call_functions(type, p);
}

static void schedule(void)
{
    printf("%s\n", __FUNCTION__);
    if(!readyque.head)
        RtosSysdown();

    current = readyque.head;

    // pend sv call
    SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
}

static void syscall_intr(void)
{
    syscall_proc(current->syscall.type, current->syscall.param);
}

static void softerr_intr(void)
{
    printf("%s DOWN.\n", current->name);
    getcurrent();
    thread_exit();
}

static void thread_intr(void)
{
    //syscall_intr();
 
   __asm(
           "ldmia %0!, {r4-r11};"
           "msr    PSP, %0;"
           "orr    lr, lr, #0xD;" // Return back to user mode
           "bx     lr;"
           :
           : "r"   (current->context)
      );
}

void RtosStart(rtos_func_t func, char *name, int stacksize, int argc, char *argv[]) __attribute__ ((naked));
void RtosStart(rtos_func_t func, char *name, int stacksize, int argc, char *argv[])
{
    printf("%s\n", __FUNCTION__);
    current = NULL;

    readyque.head = readyque.tail = NULL;
    memset(threads, 0, sizeof(threads));

    current = (rtos_thread*)thread_run(func, name, stacksize, argc, argv);
    RtosRun(func, name, stacksize, argc, argv);
}

void RtosSysdown(void)
{
    printf("system error!\n");
    while(1);
}

void RtosSyscall(rtos_syscall_type_t type, rtos_syscall_param_t *param)
{
    current->syscall.type = type;
    current->syscall.param = param;
    
    __asm volatile ("svc 0");
}

void SVC_Handler(void) __attribute__ ((naked));
void SVC_Handler(void)
{
   __asm(
           "ldmia %0!, {r4-r11};"
           "msr    PSP, %0;"
           "orr    lr, lr, #0xD;" // Return back to user mode
           "bx     lr;"
           :
           : "r"   (current->context)
      );
}

void PendSV_Handler(void) __attribute__ ((naked));
void PendSV_Handler(void)
{
    printf("%s\n", __FUNCTION__);
   __asm(
           "mrs     r0, PSP;"
           "stmdb   r0, {r4-r11};"
           "str     r0, [%0];"
           :
           : "r"    (&current->context)
           : "r0"
      );

   // 次のスレッドのスケジューリングが必要
   __asm(
           "ldmia  %0!, {r4-r11};"
           "msr    PSP, %0;"
           "orr    lr, #0xD;" // Return back to user mode
           "bx     lr;"
           :
           : "r"   (current->context)
      );
}
#endif
