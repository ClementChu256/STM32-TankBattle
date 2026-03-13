#include "stm32f10x.h"
#include "IERG3810_Interrupt.h"
void IERG3810_NVIC_SetPriorityGroup(u8 prigroup){
	u32 temp, temp1;
	temp1=prigroup & 0x00000007;
	temp1<<=8;
	temp=SCB->AIRCR;
	temp&=0x0000F8FF;
	temp|=0x05FA0000;													//key, without this, the changes will be ignore
	temp|=temp1;															
	SCB->AIRCR=temp;													//set 10-8 bit to 101, which is 2 bits for preempt, 2 bits for subpriority
}