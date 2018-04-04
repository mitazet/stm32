/*
===============================================================================
 Name        : main.c
 Author      : 
 Version     :
 Copyright   : Copyright (C)
 Description : main definition
===============================================================================
*/

#include "stm32f303x8.h"
#include "printf.h"

//
// Cortex-M4が例外発生時に自動的にPUSHするレジスタ類
//
struct ESTK_STRUC {
	unsigned int	r_r0;
	unsigned int	r_r1;
	unsigned int	r_r2;
	unsigned int	r_r3;
	unsigned int	r_r12;
				// r13はSP
	unsigned int	r_lr;	// r14	スレッド中のLR
	unsigned int	r_pc;	// r15
	unsigned int	r_xpsr;
};
typedef struct ESTK_STRUC	ESTK;

//
// タスクのスタック（タスクが非スケジュール状態の時のスタック上のデータ）
//
// r13はSPなのでスタックには積まないでTCBで管理する
//
struct TSTK_STRUC {
	unsigned int	r_r4;
	unsigned int	r_r5;
	unsigned int	r_r6;
	unsigned int	r_r7;
	unsigned int	r_r8;
	unsigned int	r_r9;
	unsigned int	r_r10;
	unsigned int	r_r11;

//------ここから先はCortex-M4が自動的に積む分------
	unsigned int	r_r0;
	unsigned int	r_r1;
	unsigned int	r_r2;
	unsigned int	r_r3;
	unsigned int	r_r12;
	unsigned int	r_lr;	// r14	スレッド中のLR
	unsigned int	r_pc;	// r15
	unsigned int	r_xpsr;
};
typedef struct TSTK_STRUC TSTK;

#define	TSTKSIZE	((sizeof (struct TSTK_STRUC))/sizeof (int))	// 各タスクが非スケジュール時に使用するスタックサイズ
#define	ESTKSIZE	((sizeof (struct ESTK_STRUC))/sizeof (int))	// Cortex-M4が例外発生時に自動的に使用するスタックサイズ
#define	UPUSHSIZE	(TSTKSIZE-ESTKSIZE)

//
//　タスクの状態コード
//
#define	STATE_FREE	0x00
#define	STATE_IDLE	0x01
#define	STATE_READY	0x02
#define	STATE_SLEEP	0x03

#define	EOQ	0xff	// End Of Queue：キューの最後であることを示す。

#define	MAX_TASKNUM 4 


//
// TCB（Task Control Block)の定義
//
struct TCTRL_STRUC {
	unsigned char	link;
	unsigned char	state;
	unsigned char	msg_q;
	unsigned char	param0;
	TSTK		*sp;
	unsigned int	param1;
	unsigned int	param2;
};
typedef struct TCTRL_STRUC TCTRL;	// typedefすると見た目がちょっと格好良いかもっていう程度
TCTRL	tcb[MAX_TASKNUM];	// TCBをタスク数分確保

#define	STKSIZE	64			// タスク用スタックサイズ（64ワード：256バイト）
unsigned int	stk_task[MAX_TASKNUM][STKSIZE];	// タスク用スタックエリア


//===============================================
//= 		TCB関係処理			=
//===============================================
unsigned char	q_pending[2];	// 処理待ちタスク（ON/OFF処理などで使用）
unsigned char	q_ready;	// 起動状態
unsigned char	q_sleep;	// スリープ状態（タイマ待ち）

unsigned char task_start;
unsigned char c_tasknum;	// 現在スケジュール中のタスク番号
TCTRL	*c_task;


//
// キューの最後に指定されたTCBをつなぐ
//
unsigned char tcbq_append(unsigned char *queue, unsigned char tcbnum)
{
	unsigned char ctasknum, ptasknum;
	if (tcbnum == EOQ)			// EOQだったらなにもしない
		return(EOQ);
	if ((ctasknum = *queue) == EOQ) {	// いま接続されている先は無い
		*queue = tcbnum;		// 与えられたTCBをつなぐ
	} else {
		do {				// 最後を探す
			ptasknum = ctasknum;	// PreviousTCBにCurrentTCB番号を保存
		} while((ctasknum = tcb[ptasknum].link) != EOQ);	// ptasknumが最後？
		tcb[ptasknum].link = tcbnum;	// 最後のTCBに指定されたTCBをつなぐ
	}
	tcb[tcbnum].link = EOQ;			// キューの終わりになるので、EOQ
	return(tcbnum);
}

//
// キューの先頭を取り出す
//
unsigned char tcbq_get(unsigned char *queue)
{
	unsigned char tcbnum;
	if ((tcbnum = *queue) != EOQ)		// キューの先頭を取り出す
		*queue = tcb[tcbnum].link;	// EOQでないなら、リンク先につなぎなおし
	return(tcbnum);				// もし、キューの先頭がEOQならEOQが返るだけ
}

//
// 指定されたTCBをキューから外す
//
unsigned char tcbq_remove(unsigned char *queue, unsigned char tcbnum)
{
	unsigned char ctasknum,ptasknum;
	ctasknum = *queue;
	if (tcbnum == EOQ)			// EOQだったらなにもしない
		return(EOQ);
	if (ctasknum == EOQ)	// キューに何もつながってない
		return(EOQ);	// ならEOQで終了
	if (ctasknum == tcbnum) {	//　いきなり先頭
		*queue = tcb[tcbnum].link;	// つなぎ変えて
		return(ctasknum);		// 終了
	}
	do {						// マッチするものを探すループ
		ptasknum = ctasknum;			// PreviousにCurrentの値をコピー
		if ((ctasknum = tcb[ptasknum].link) != EOQ)	// リンク先がEOQだったら
			return(EOQ);			// EOQを返す
	} while(ctasknum != tcbnum);			// リンク先が一致するまでループ
	tcb[ptasknum].link = tcb[ctasknum].link;	// ctasknumのリンクを繋ぎ替え
	return(ctasknum);
}

//
// タスクON処理
//
unsigned char process_taskon(unsigned char tasknum)
{
	unsigned char c;
	c = tcb[tasknum].state;
	if ((c == STATE_FREE) || (c == STATE_IDLE)) {		// FreeかIdleのもの専用
		tcb[tasknum].state = STATE_READY;
		tcbq_append(&q_ready,tasknum);
		return(tasknum);
	}
	return(EOQ);
}

//
// タスクOFF処理
//　　指定されたタスクがREADYまたはSLEEP状態であれば、はずしてIDLEステートにする
//
unsigned char process_taskoff(unsigned char tasknum)
{
	unsigned char c;
	switch(c = tcb[tasknum].state) {
		case	STATE_READY:
			tcbq_remove(&q_ready, tasknum);
			tcb[tasknum].state = STATE_IDLE;
			return(tasknum);
		case	STATE_SLEEP:
			tcbq_remove(&q_sleep, tasknum);
			tcb[tasknum].state = STATE_IDLE;
			return(tasknum);
		default:
			return(EOQ);
	}
}

//
// スリープ処理
//
void process_sleep(void)
{
	unsigned char tasknum;
	tasknum = q_sleep;
	while(tasknum != EOQ) {		// キューの最後までスキャン
		if (tcb[tasknum].param1 == 0) {	//　タイムアップした
			tcbq_remove(&q_sleep, tasknum);	// Sleep_Qから外す
			tcb[tasknum].state = STATE_READY;	// READY状態にする
			tcbq_append(&q_ready, tasknum);	// Ready_Qにつなぐ
		} else {
			tcb[tasknum].param1--;	// デクリメント
		}
		tasknum = tcb[tasknum].link;
	}
}

//
// システムタイマ割り込みの処理
//
unsigned char rq_timer;

unsigned int systick_count;
unsigned int sticks = 3;
void SysTick_Handler()
{
	if (systick_count)
		systick_count--;
	
	if (task_start) {
		if (sticks)
			sticks--;
		else {
			sticks = 3;
			rq_timer = 1;
			SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
		}
	}
}

void pend_sv(void)
{
	SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
}

unsigned int pendsv_count;
// 
// PendSVのハンドラ（タスクスイッチングの実行）
//
// ところで、
// __attribute__ ((naked));
// は、
// indicate that the specified function does not need prologue/epilogue
// sequences generated by the compiler
// ということで、コンパイラによるレジスタ退避などを行わせないためのアトリビュート。
// コンパイラで使うスタック上のワーク領域なども自動確保するコード生成をしないので
// __asm()の中で書いてやらなければならない。
// コンパイラがどれだけスタック領域を使うのかは実際__attribute__を外してコンパイル
// してみて確認するしかない。
//
unsigned char swstart = 0;
void schedule(void)
{
	unsigned char ctasknum,ntasknum;
	// デバッグ用にスイッチング回数をカウント
	pendsv_count++;
	// c_taskを切り替え（タスクスイッチング）
	if (swstart) {
		if (rq_timer) {
			process_sleep();
			rq_timer = 0;
		}
		if ((ctasknum = q_pending[0]) !=EOQ) {		// Pendingされているタスク制御処理がある
			switch(q_pending[1]) {
				case STATE_READY:		// Ready状態にしたい
					process_taskon(ctasknum);	// TASKON処理をする
					break;
				case STATE_IDLE:		// Idle状態にしたい
					process_taskoff(ctasknum);
					break;
				default:
					break;
			}
			q_pending[0] = EOQ;			// 処理したので、クリア
		}
		ctasknum = c_tasknum;			// 今までスケジュールしていたタスク番号を退避
		ntasknum = tcbq_get(&q_ready);		// Ready_Qから取り出す
		if (ntasknum == EOQ) {			// Ready_Qが無い
			if (c_tasknum == EOQ)		// スケジュールしていたものもない！？？
				c_tasknum = 0;		// ありえないけど、0にしておく
		} else {					// Ready_Qにつながっていた
			c_tasknum = ntasknum;			// Ready_Qから取り出したのをスケジュール状態にする
			switch(tcb[ctasknum].state) {
				case	STATE_READY:	tcbq_append(&q_ready, ctasknum); break;
				case	STATE_SLEEP:	tcbq_append(&q_sleep, ctasknum); break;
				default:		break;
			}
		}
		for (ctasknum = 0; ctasknum < MAX_TASKNUM; ctasknum++) {
			if ((tcb[ctasknum].state == STATE_IDLE) && (tcb[ctasknum].msg_q != EOQ))
				process_taskon(ctasknum);
		}
	} else {
		swstart = 1;
	}
	c_task = &tcb[c_tasknum];
}

void PendSV_Handler(void) __attribute__ ((naked));
void PendSV_Handler()
{
	// 前半部分では、
	// ・自動退避されない汎用レジスタをR12でstmdb（Dec. Before)を使ってPSP上に退避
	// ・c_task（現在実行中のタスクのTCBをさしている）にR12を退避
	// を実行
	__asm(						// R12をワーク用スタックとして利用
			"mrs	r12,psp;"			// R12にPSPの値をコピー
			"stmdb	r12!,{r4-r11};"		// 自動退避されないR4～R11を退避
			"movw	r2,#:lower16:c_task;"	// *(ctask->sp) = R12;
			"movt	r2,#:upper16:c_task;"
			"ldr	r0,[r2,#0];"
			"str	r12,[r0,#4];"
		 );

	// 次にスケジュールするタスクの選択
	__asm(
			"push	{lr};"
			"bl		schedule;"
			"pop	{lr};"
		 );

	// 後半部分では
	// 新しいc_taskの指す先（次に動かすタスクのTCB）からレジスタを復帰
	// 今度は
	// ・R12を取り出し
	// ・汎用レジスタを復旧（ldmia（Inc. After）を利用）
	// 元に戻る
	__asm (
			"movw	r2,#:lower16:c_task;"	// R12 = *(c_task->sp);
			"movt	r2,#:upper16:c_task;"
			"ldr	r0,[r2,#0];"
			"ldr	r12,[r0,#4];"

			"ldmia	r12!,{r4-r11};"		// R4～R11を復帰

			"msr	psp,r12;"			// PSP = R12;
			"bx		lr;"				// (RETURN)
		  );
}

unsigned int svcop;
unsigned int svcparam[2];
void SVC_Handler(void) __attribute__ ((naked));
void SVC_Handler()
{
	__asm(
		"movw	r2,#:lower16:svcparam;"	// svcparam[0] = R0;
		"movt	r2,#:upper16:svcparam;"	// svcparam[1] = R1;
		"str	r0,[r2,#0];"
		"str	r1,[r2,#4];"
		"mov	r0,lr;"		// if ((R0 = LR & 0x04) != 0) {
		"ands	r0,#4;" 		//			// LRのビット4が'0'ならハンドラモードでSVC
		"beq	.L0000;"		//			// '1'ならスレッドモードでSVC
		"mrs	r1,psp;"		// 	R1 = PSP;	// プロセススタックをコピー
		"b		.L0001;"		//
		".L0000:;"			// } else {
		"mrs	r1,msp;"		//	R1 = MSP;	// メインスタックをコピー
		".L0001:;"			// }
		"ldr	r2,[r1,#24];"	// R2 = R1->PC;
		"ldr	r0,[r2,#-2];"	// R0 = *(R2-2);	// SVC(SWI)命令の下位バイトが引数部分

		"movw	r2,#:lower16:svcop;"	// svcop = R0;		// svcop変数にコピー
		"movt	r2,#:upper16:svcop;"
		"str	r0,[r2,#0];"
	);

	switch(svcop & 0xff) {			// SVCの引数（リクエストコード）を取得
		case 0x00:	// NULL（再スケジューリングするだけ）
			SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;		// PendSVを発生させてスケジューリング
			break;
		case 0x10:	// SLEEP
			tcb[c_tasknum].param1 = svcparam[0];		// パラメータを積んで
			tcb[c_tasknum].state = STATE_SLEEP;		// スリープ状態にしておく
			SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;		// PendSVを発生させてスケジューリング
			break;
		case 0x11:	// TASKON
			q_pending[0] = svcparam[0];			// タスク番号をPendingにセット
			q_pending[1] = STATE_READY;			// READY状態に遷移させたい
			SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;		// PendSVを発生させてスケジューリング
			break;
		case 0x12:	// TASKOFF
			q_pending[0] = svcparam[0];			// タスク番号をPendingにセット
			q_pending[1] = STATE_IDLE;			// IDLE状態に遷移させたい
			SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;		// PendSVを発生させてスケジューリング
			break;
		case 0x19:	// TASKIDGET
			__asm(
				"movw	r0,#:lower16:c_tasknum;"	// R0=c_tasknum
				"movt	r0,#:upper16:c_tasknum;"
				"ldrb	r0,[r0,#0];"
				"str	r0,[r1,#0];"			// return(R0);
			);
			break;
		case 0xf0:	// CHG_IDLE
			tcb[c_tasknum].state = STATE_IDLE;
			SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;		// PendSVを発生させてスケジューリング
			break;
		case 0xff:	// CHG_UNPRIVILEGE
			__asm(
				"movw	r2,#:lower16:c_task;"	// psp = *(c_task->sp);
				"movt	r2,#:upper16:c_task;"
				"ldr	r0,[r2,#0];"
				"ldr	r2,[r0,#4];"
				"msr	psp,r2;"
				"orr	lr,lr,#4;"			// LR |= 0x04;
									// スレッドモードに移行
									// 1001:msp使用(プロセス） 1101:psp使用（スレッド）
									// なので、セットするとスレッドモードになる
			);
			task_start = 1;					// 以降はタスクスイッチング実行
			SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;		// PendSVを発生させる
			break;
		default:
			break;
	}

	__asm(
		"bx     lr;"
		);
}


//=======================================
//= ここからはタスク			=
//=======================================
//

//===============================
//=				=
//= SVCの定義			=
//=				=
//===============================


#define	SYSCALL_NULL	__asm("svc #0x00;")

#define	SYSCALL_IDLE	__asm("svc #0xf0;")
#define	SYSCALL_CHG_UNPRIVILEGE	__asm("svc #0xff;")

#define SYSCALL_SLEEP(x)	{register int p0 __asm("r0"); p0=x; __asm ("svc #0x10"::"r"(p0));}
#define	SYSCALL_TASKON(x)	{register int p0 __asm("r0"); p0=x; __asm ("svc #0x11"::"r"(p0));}
#define	SYSCALL_TASKOFF(x)	{register int p0 __asm("r0"); p0=x; __asm ("svc #0x12"::"r"(p0));}
#define	SYSCALL_TASKIDGET	__asm("svc #0x19;")


//===============================
//=				=
//= SVCインターフェースAPI	=
//=				=
//===============================

//
// NULL:スケジューラを呼び出し、次のタスクを起動する
//
void SVC_NULL(void)
{
	SYSCALL_NULL;
}

//
// IDLE:IDLEステートに移行する（メッセージ待ちなどで利用）
//
void SVC_IDLE(void)
{
	SYSCALL_IDLE;
}

//
// スレッドモードに移行（main()からMicroMultiを起動する時に利用するだけ）
//
void SVC_CHG_UNPRIVILEGE(void)
{
	SYSCALL_CHG_UNPRIVILEGE;
}

//
// SLEEP：指定した回数分システムタイマ割り込みがくるまでスリープ
//
void SVC_SLEEP(unsigned int times)
{
	SYSCALL_SLEEP(times);
}

//
// TASKON：指定したタスク番号のタスクを起動
//
void SVC_TASKON(unsigned int tasknum)
{
	SYSCALL_TASKON(tasknum);
}

//
// TASKOFF：指定したタスク番号のタスクを停止
//
void SVC_TASKOFF(unsigned int tasknum)
{
	SYSCALL_TASKOFF(tasknum);
}

//
// 
//
unsigned char SVC_TASKIDGET()
{
	register unsigned char c __asm("r0");
	SYSCALL_TASKIDGET;
	return(c);
}

//===============================
//=				=
//=　タスク関数			=
//=				=
//===============================

unsigned char dbgdata[3];
void th_zero()
{
	printf("%s\n", __FUNCTION__);
	dbgdata[0] = SVC_TASKIDGET();
	while(1) {
        printf("Hey\n");
	}
}

void th_one()
{
	printf("%s\n", __FUNCTION__);
	dbgdata[1] = SVC_TASKIDGET();
	while(1) {
        printf("money\n");
	}
}

void th_two()
{
	printf("%s\n", __FUNCTION__);
	dbgdata[2] = SVC_TASKIDGET();
	while(1) {
        printf("gold\n");
	}
}


//===============================
//=				=
//=　スタートアップと初期化関数	=
//=				=
//===============================

//
//  TCBの初期化とタスク登録
//
//　XPSRのビット24は'1'にしないとハードエラーになるので注意
//
//　スタックデータ作成時直接キャストしてアクセスすると
//　コード生成を間違うようなので、一旦unsigned intに
//　キャストして代入しなおした。
//
unsigned int *p;
TSTK	*p_stk;

void init_tcb()
{
	unsigned char tasknum;
	for (tasknum = 0; tasknum <MAX_TASKNUM; tasknum++)  {
		tcb[tasknum].state = STATE_FREE;
		tcb[tasknum].link = EOQ;
		tcb[tasknum].msg_q = EOQ;
	}
}

void regist_tcb(unsigned char tasknum, void(*task)(void))
{
	p = stk_task[tasknum];
	p+=(STKSIZE-TSTKSIZE);
	p_stk = (TSTK*)(p);
	p_stk->r_r0 = 0x00;
	p_stk->r_r1 = 0x01;
	p_stk->r_r2 = 0x02;
	p_stk->r_lr = 0x00;
	p_stk->r_pc = (unsigned int)(task);
	p_stk->r_xpsr = 0x01000000;
	tcb[tasknum].sp = (TSTK*)(p);
}

//
// ここからメインルーチン
// 諸々の初期化をしてから、マルチタスクモードに移行
//
int main(void)
	__attribute__ ((section (".entry_point")));
int main(void)
{
    printf("Main Program booted!!\n");
	// TODO: insert code here
	unsigned int i;
	init_tcb();

	rq_timer = 0;
	task_start = 0;
	c_tasknum = 0;
	c_task = &tcb[c_tasknum];

	pendsv_count = 0;
	//systick_count = 10;
	systick_count = 0;
//	SysTick_Config(SystemCoreClock/100);	// 1/100秒（=10ms）ごとにSysTick割り込み
	SysTick_Config(SystemCoreClock/10);	// 1/10秒（=100ms）ごとにSysTick割り込み
	NVIC_SetPriority(SVCall_IRQn, 0x80);	// SVCの優先度は中ほど
	NVIC_SetPriority(SysTick_IRQn, 0xc0);	// SysTickの優先度はSVCより低く
	NVIC_SetPriority(PendSV_IRQn, 0xff);	// PendSVの優先度を最低にしておく

    __enable_irq(); // enable interrupt

	//　この段階ではまだ特権モードであり、MSPが使われている
	//　PSPプロセススタックは非特権モードで動いている状態
	// タスク#0（main()から直接切り替わって動くタスク）のスタック
	// については、ユーザが積む分だけ削っておく
	// CHG_UNPRIVILEGEでSVC=>PendSVの時にPSPに積みなおすことで
	// つじつまが合う
	q_ready = EOQ;
	q_sleep = EOQ;
	q_pending[0] = EOQ;

	regist_tcb(0, th_zero);
	tcb[0].sp = (TSTK *)(((unsigned int *)tcb[0].sp)+UPUSHSIZE);
	tcb[0].state = STATE_READY;

	regist_tcb(1, th_one);
	tcb[1].state = STATE_READY;

	regist_tcb(2, th_two);
	tcb[2].state = STATE_READY;

	// Ready_Qにつないでおく
	// 最初は#0が動くことにしたので、#0はいきなりCurrentTASKになるため、
	// Ready_Qにつなぐのは0以外
	tcbq_append(&q_ready, 1);
	tcbq_append(&q_ready, 2);

	tcb[3].state = STATE_FREE;
	tcb[3].link = EOQ;

	SVC_CHG_UNPRIVILEGE();	// スレッド#0に移行する

	while(1);		// 戻ってくることは無いけど、念のため
}

