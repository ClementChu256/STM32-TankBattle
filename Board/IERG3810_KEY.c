#include "stm32f10x.h"
#include "IERG3810_KEY.h"
void IERG3810_KEY_Init(){
	RCC->APB2ENR |= 1<<6; 		//enable GPIO-E clock
	GPIOE->CRL &= 0xFFFFF0FF; //set bit 8-11 to 0
	GPIOE->CRL |= 0x00000800; //input with pull-up/pull-down, input mode
	GPIOE->BSRR |= 1 << 2;		//reset bit of pin-2
	
	GPIOE->CRL &= 0xFFFF0FFF; //set bit 12-15 to 0
	GPIOE->CRL |= 0x00008000; //input with pull-up/pull-down, input mode
	GPIOE->BSRR |= 1<<3;			//reset bit of pin-3
	
	GPIOE->CRL &= 0xFFF0FFFF; //set bit 16-19 to 0
	GPIOE->CRL |= 0x00080000; //input with pull-up/pull-down, input mode
	GPIOE->BSRR |= 1<<4;			//reset bit of pin-4
	
	RCC->APB2ENR |= 1<<2;  		//enable GPIO-A clock
	GPIOA->CRL &= 0xFFFFFFF0; //set bit 0-3 to 0
	GPIOA->CRL |= 8;					//input with pull-up/pull-down, input mode
	GPIOA->BRR |= 1;					//set bit of pin-0
}