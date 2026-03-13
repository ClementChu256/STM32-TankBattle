#include "stm32f10x.h"
#include "IERG3810_clock.h"
void IERG3810_clock_tree_init(){
	u8 PLL=7;
	unsigned char temp=0;
	RCC->CFGR &=0xF8FF0000;
	RCC->CR &=0xFEF6FFFF;
	RCC->CR |=0x00010000;
	while(!(RCC->CR>>17));
	RCC->CFGR=0x00000400;					//PPRE1=100
	RCC->CFGR|=PLL<<18;						//PLL input clock*9
	RCC->CFGR|=1<<16;							//PLLSRC=1
	FLASH->ACR|=0x32;							//wait 2 states
	RCC->CR|=0x01000000;					//enable HSE
	while(!(RCC->CR>>25));			  //wait until HSE stable
	RCC->CFGR|=0x00000002;				//PLL selected as system clock
	while(temp!=0x02){						//while status!=PLL used as sysclk
		temp=RCC->CFGR>>2;
		temp&=0x03;
	}
}