#include "stm32f10x.h"
#include "getInput.h"

int input[8];

void joystickInit(){
	//enable io clock b,d
	RCC->APB2ENR|= 1<<3;
	RCC->APB2ENR|=1<<5;
	
	GPIOB->CRH&=0xFFFF00FF;
	GPIOB->CRH|=0x00003800;
	
	GPIOD->CRL&=0xFFFF0FFF;
	GPIOD->CRL|=0x00003000;
	
	GPIOB->BSRR|=0x00000400;
	
}

void IERG3810_TIM3_Init(u16 arr, u16 psc){
	RCC->APB1ENR|=1<<1;				//enable TIM3 clock
	TIM3->ARR=arr;						//set arr
	TIM3->PSC=psc;						//set psc
	TIM3->DIER|=1<<0;					//enable interrupt
	TIM3->CR1|=0x01;					//enable counter
	NVIC->IP[29]=0x45;				//set priority
	NVIC->ISER[0]|=(1<<29);		//enable interrupt
}

void TIM3_IRQHandler(){
	int i,j;
	u8 tmp;
	u16 x=0,y=0;
	if(TIM3->SR&1<<0){
		GPIOB->ODR^=1<<11;	
		Delay(10);
		GPIOB->ODR^=1<<11;
		tmp=(GPIOB->IDR&0x00000400)>>10;
		tmp=~tmp&0x01;
		input[0]=tmp;
		x+=8;
		for(i=0;i<8;i++){
			Delay(10);
			GPIOD->ODR^=1<<3;
			Delay(10);
			GPIOD->ODR^=1<<3;
			if(i==7) break;
			tmp=(GPIOB->IDR&0x00000400)>>10;
			tmp=~tmp&0x01;
			input[i+1]=tmp;
			x+=8;
		}
	}
	TIM3->SR&=~(1<<0);
}