#include "printf.h"
#include "lib.h"
#include "rtos.h"

#define THREAD_NUM 3 
#define PRIORITY_NUM 8
#define MAX_SYS_TICKS 8
#define THREAD_NAME_SIZE 15

unsigned char rtos_start = 0;
unsigned int sticks = 3;

// thread context
typedef struct _rtos_context{
    uint32_t sp;
}rtos_context;

// Task Control Block (TCB)
typedef struct _rtos_thread{
    struct _rtos_thread *next;
    char name[THREAD_NAME_SIZE + 1];
	int priority;
    char *stack;
    
    struct {
        rtos_func_t func;
        int argc;
        char  **argv;
    }init;

    struct {
        rtos_syscall_type_t type;
        rtos_syscall_param_t *param;
    }syscall;
    
    rtos_context context;
}rtos_thread;

// Ready Que
static struct{
    rtos_thread *head;
    rtos_thread *tail;
}readyque;

static rtos_thread *current;
static rtos_thread threads[THREAD_NUM];

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

static void thread_init(rtos_thread *thp)
{
    thp->init.func();
}

static rtos_thread_id_t thread_run(rtos_func_t func, char* name, int priority, int stacksize, int argc, char* argv[])
{
    int i;
    rtos_thread *thp;
    uint32_t *sp;
    extern char _userstack; // from Linker script
    static char *thread_stack = &_userstack;

    for(i = 0; i < THREAD_NUM; i++){
        thp = &threads[i];
        if(!thp->init.func)
            break;
    }
    if(i == THREAD_NUM)
        return -1;

    memset(thp, 0, sizeof(*thp));

    /* set TCB */
    strcpy(thp->name, name);
    thp->next = NULL;
	thp->priority = priority;

    thp->init.func = func;
    thp->init.argc = argc;
    thp->init.argv = argv;

    /* allocate stack area */
    memset(thread_stack, 0, stacksize);
    thread_stack += stacksize;

    thp->stack = thread_stack;

    sp = (uint32_t*)thp->stack - 16;

    sp[15] = (uint32_t)0x01000000;  // xPSR
    sp[14] = (uint32_t)thread_init; // PC
    sp[8]  = (uint32_t)thp;         // r0:Argument
    
    thp->context.sp = (uint32_t)sp;

    current = thp;
    putcurrent();

    return (rtos_thread_id_t)current;
}

void schedule(void)
{
    if(!readyque.head)
        RtosSysdown();
    
    current = readyque.head;

	// タイムスライスを優先度に応じて設定
	sticks = MAX_SYS_TICKS - current->priority;

    readyque.head = current->next;
    readyque.tail->next = current;
    readyque.tail = current;
    //readyque[i].tail->next = NULL;
}

void RtosInit(void)
{
    current = NULL;

    memset(&readyque, 0, sizeof(readyque));
    memset(threads, 0, sizeof(threads));
}

void RtosStart(void)
{
    /* run first thread */
	current->context.sp = (uint32_t)((uint32_t*)current->context.sp + 8);
    __asm volatile ("svc 0");
}

void RtosThreadCreate(rtos_func_t func, char *name, int priority, int stacksize, int argc, char *argv[])
{
    thread_run(func, name, priority, stacksize, argc, argv);
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
    
    switch(type){
        case RTOS_SYSCALL_CHG_UNPRIVILEGE:
            __asm volatile ("svc 0");
            break;
		case RTOS_SYSCALL_SLEEP:
			break;
		case RTOS_SYSCALL_WAKEUP:
			break;
		case RTOS_SYSCALL_CHG_PRIORITY:
			break;
        default:
            break;
    }
}

unsigned int svcop;
void SVC_Handler(void) __attribute__ ((naked));
void SVC_Handler(void)
{
    __asm(
            "mov	r0,lr;"		// if ((R0 = LR & 0x04) != 0) {
            "ands	r0,#4;" 	//			// LRのビット4が'0'ならハンドラモードでSVC
            "beq	.L0000;"	//			// '1'ならスレッドモードでSVC
            "mrs	r1,psp;"	// 	R1 = PSP;	// プロセススタックをコピー
            "b		.L0001;"	//
            ".L0000:;"			// } else {
            "mrs	r1,msp;"	//	R1 = MSP;	// メインスタックをコピー
            ".L0001:;"			// }
            "ldr	r2,[r1,#24];"	// R2 = R1->PC;
            "ldr	r0,[r2,#-2];"	// R0 = *(R2-2);	// SVC(SWI)命令の下位バイトが引数部分

            "movw	r2,#:lower16:svcop;"	// svcop = R0;		// svcop変数にコピー
            "movt	r2,#:upper16:svcop;"
            "str	r0,[r2,#0];"
         );

    switch(svcop & 0xff){
        case RTOS_SYSCALL_CHG_UNPRIVILEGE:
            __asm(
                    "movw	r2,#:lower16:current;"
                    "movt	r2,#:upper16:current;"
                    "ldr	r0,[r2,#0];"
                    "ldr	r2,[r0,#48];"
                    "msr    PSP, r2;"
                    "orr    lr, lr, #4;" // Return back to user mode
                 );
            rtos_start = 1;
            SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
            break;
        default:
            break;
    }
    __asm(
            "bx     lr;"
         );
}

void PendSV_Handler(void) __attribute__ ((naked));
void PendSV_Handler(void)
{
    __asm(						// R12をワーク用スタックとして利用
            "mrs	r12,psp;"			// R12にPSPの値をコピー
            "stmdb	r12!,{r4-r11};"		// 自動退避されないR4～R11を退避
            "movw	r2,#:lower16:current;"	// *(current->context) = R12;
            "movt	r2,#:upper16:current;"
            "ldr	r0,[r2,#0];"
            "str	r12,[r0,#48];"
         );

   // 次スレッドのスケジューリング
   __asm(
           "push    {lr};"
           "bl      schedule;"
           "pop     {lr};"
        );

   __asm (
           "movw	r2,#:lower16:current;"	// R12 = *(current->context);
           "movt	r2,#:upper16:current;"
           "ldr     r0,[r2,#0];"
           "ldr     r12,[r0,#48];"

           "ldmia	r12!,{r4-r11};"		// R4～R11を復帰

           "msr     psp,r12;"			// PSP = R12;
           "bx		lr;"				// (RETURN)
         );
}

void SysTick_Handler()
{
	if (rtos_start) {
		if (sticks)
			sticks--;
		else {
			SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
		}
	}
}
