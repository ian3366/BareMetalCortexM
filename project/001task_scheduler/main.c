

#include <stdint.h>
#include <stdio.h>
#include "led.h"
#include "main.h"

//[prototype-s]
void idle_handler();
void task1_handler();
void task2_handler();
void task3_handler();
void task4_handler();

void init_systick_timer(uint32_t tickHz);
__attribute((naked)) void init_scheduler_stack(uint32_t sched_top_of_stack);
void init_task_stack();
void enable_process_faults();
__attribute((naked)) void switch_sp_to_psp();
uint32_t get_psp_value();
void save_psp_value(uint32_t current_psp_value);
void update_next_task();


void task_delay(uint32_t tick_count);
void schedule(void);
//[prototype-e]

typedef struct{
	uint32_t psp_value;
	uint32_t block_count;
	uint8_t current_state;
	void(*task_handler)();
}TCB_t;//task control black

TCB_t gUserTask[MAX_TASKS];

uint8_t gCurrentTask = 1;
uint32_t gTickCount = 0;

int main(void)
{

	enable_process_faults();

    init_scheduler_stack(SCHED_STACK_START);

	init_task_stack();

	init_led_all();

	init_systick_timer(TICK_HZ);

	switch_sp_to_psp();

	task1_handler();

    /* Loop forever */
	for(;;);
}

void idle_handler(){
	while(1);
}

void task1_handler(){

	while(1){
//		printf("this is task1 \n");
		led_on(LED_GREEN);
		task_delay(1000);
		led_off(LED_GREEN);
		task_delay(1000);


	}
}
void task2_handler(){
	while(1){
//		printf("this is task2 \n");
		led_on(LED_BLUE);
		task_delay(500);
		led_off(LED_BLUE);
		task_delay(500);

	}
}
void task3_handler(){
	while(1){
//		printf("this is task3 \n");
		led_on(LED_RED);
		task_delay(250);
		led_off(LED_RED);
		task_delay(250);

	}
}
void task4_handler(){
	while(1){
//		printf("this is task4 dark\n");
		task_delay(250);
	}
}

void init_systick_timer(uint32_t tickHz){

	uint32_t* pSTK_CTRL = (uint32_t*)0xE000E010;
	uint32_t* pSTK_LOAD = (uint32_t*)0xE000E014;
	//config time count
	uint32_t countValue = (SYS_TIME_CLK_FRQ/tickHz) - 1;
	*pSTK_LOAD &= ~(0x00FFFFFFF);
	*pSTK_LOAD |= countValue;
	//config time control reg
	*pSTK_CTRL |= 7;//111
}


__attribute((naked)) void init_scheduler_stack(uint32_t sched_top_of_stack){
    __asm volatile("MSR MSP,%0": :  "r" (sched_top_of_stack)  :   );
    __asm volatile("BX LR");
}

void init_task_stack(){

	gUserTask[0].current_state = TASK_READY_STATE;
	gUserTask[1].current_state = TASK_READY_STATE;
	gUserTask[2].current_state = TASK_READY_STATE;
	gUserTask[3].current_state = TASK_READY_STATE;
	gUserTask[4].current_state = TASK_READY_STATE;

	gUserTask[0].psp_value = IDLE_STACK_START;
	gUserTask[1].psp_value = T1_STACK_START;
	gUserTask[2].psp_value = T2_STACK_START;
	gUserTask[3].psp_value = T3_STACK_START;
	gUserTask[4].psp_value = T4_STACK_START;

	gUserTask[0].task_handler = idle_handler;
	gUserTask[1].task_handler = task1_handler;
	gUserTask[2].task_handler = task2_handler;
	gUserTask[3].task_handler = task3_handler;
	gUserTask[4].task_handler = task4_handler;

	uint32_t *pPSP;
	for(int i = 0;i<MAX_TASKS;i++){
		//XPSR
		pPSP = (uint32_t*)gUserTask[i].psp_value;
		pPSP--;
		*pPSP = DUMMY_XPSR;
		//PC
		pPSP--;
		*pPSP = (uint32_t)gUserTask[i].task_handler;
		//LR
		pPSP--;
		*pPSP = 0xFFFFFFFD; //excetion return
		for(int j = 0 ;j<13;j++){
			pPSP--;
			*pPSP = 0;
		}
		gUserTask[i].psp_value = (uint32_t)pPSP;
	}
}

void enable_process_faults(){
	uint32_t* pSHCSR = (uint32_t*) 0xE000ED24;
	*pSHCSR |= 0x70000;
}


uint32_t get_psp_value(){
	return gUserTask[gCurrentTask].psp_value;
}

void save_psp_value(uint32_t current_psp_value){
	gUserTask[gCurrentTask].psp_value = current_psp_value;
}



void update_next_task(){

	int state = TASK_BLOCKED_STATE;

	for(int i= 0 ; i < (MAX_TASKS) ; i++)
	{
		gCurrentTask++;
	    gCurrentTask %= MAX_TASKS;
		state = gUserTask[gCurrentTask].current_state;
		if( (state == TASK_READY_STATE) && (gCurrentTask != 0) ) // find next ready and not idle task
			break;
	}

	if(state != TASK_READY_STATE)
		gCurrentTask = 0;

}

__attribute((naked)) void switch_sp_to_psp(){
	//get value of psp of current task
	__asm volatile("PUSH {LR}"); //reserve LR which connect back to main()
	__asm volatile("BL get_psp_value"); //now get_psp_value return value in r0
	__asm volatile("MSR PSP , R0");//init psp
	__asm volatile("POP {LR}");
	//change sp to psp by control register
	__asm volatile("MOV R0 , #0X02");
	__asm volatile("MSR CONTROL , R0");
	__asm volatile("BX LR");
}

void schedule(void)
{
	//pend the pendsv exception
	uint32_t *pICSR = (uint32_t*)0xE000ED04;
	*pICSR |= ( 1 << 28);

}

void task_delay(uint32_t tick_count)
{
	//disable interrupt
	INTERRUPT_DISABLE();

	if(gCurrentTask)
	{
	   gUserTask[gCurrentTask].block_count = gTickCount + tick_count;
	   gUserTask[gCurrentTask].current_state = TASK_BLOCKED_STATE;
	   schedule();
	}

	//enable interrupt
	INTERRUPT_ENABLE();
}





void update_global_tick_count(void)
{
	gTickCount++;
}

void unblock_tasks(void)
{
	for(int i = 1 ; i < MAX_TASKS ; i++)
	{
		if(gUserTask[i].current_state != TASK_READY_STATE)
		{
			if(gUserTask[i].block_count == gTickCount)
			{
				gUserTask[i].current_state = TASK_READY_STATE;
			}
		}

	}

}

void SysTick_Handler(){

	uint32_t *pICSR = (uint32_t*)0xE000ED04;

    update_global_tick_count();

    unblock_tasks();

    //pend the pendsv exception
    *pICSR |= ( 1 << 28);

}

__attribute__((naked)) void PendSV_Handler(void){
	/*Save the current task*/
	//1. Get current running task's psp value
	__asm volatile("MRS R0 , PSP");
	//2. Using the psp value stor SF2 (R4 to R11)
	__asm volatile("STMDB R0!,{R4-R11}"); //registr -> memory
	//3. Save LR
	__asm volatile("PUSH {LR}");
	//4. Save the current of psp
	__asm volatile("BL save_psp_value"); //now psp -> r4



	/*Retrieve the context of next task*/
	//1. Decide next task to run
	__asm volatile("BL update_next_task");
	//2. Get its past psp value
	__asm volatile("BL get_psp_value"); //return value in r0
	//3. Using that psp value retrieve SF2(R4 ro R11)
	__asm volatile("LDMIA R0!,{R4-R11}");
	//4. update the psp and exit
	__asm volatile("MSR PSP , R0");
	//5. back
	__asm volatile("POP {LR}");
	__asm volatile("BX LR");
}



void MemManage_Handler(){
	// printf("In Mem fault handler\n");
	while(1);
}
void BusFault_Handler(){
	// printf("In bus fault handler\n");
	while(1);
}
void UsageFault_Handler(void){
	// printf("Exception: UsageFault\n");
	while(1);
}

