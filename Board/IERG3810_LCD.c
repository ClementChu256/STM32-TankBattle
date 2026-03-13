#include "stm32f10x.h"
#include "IERG3810_LCD.h"
#include "FONT.H"
//#include "CFONT.H"

typedef struct{
	u16 LCD_REG;
	u16 LCD_RAM;
} LCD_TypeDef;

#define LCD_BASE		((u32)(0x6C000000|0x000007FE))
#define LCD				((LCD_TypeDef *) LCD_BASE)

void IERG3810_TFTLCD_Init(void){
	RCC->AHBENR|=1<<8;
	RCC->APB2ENR|=1<<3;
	RCC->APB2ENR|=1<<5;
	RCC->APB2ENR|=1<<6;
	RCC->APB2ENR|=1<<8;
	GPIOB->CRL&=0xFFFFFFF0;
	GPIOB->CRL|=0x00000003;
	
	GPIOD->CRH&=0x00FFF000;
	GPIOD->CRH|=0xBB000BBB;
	GPIOD->CRL&=0xFF00FF00;
	GPIOD->CRL|=0x00BB00BB;
	
	GPIOE->CRH&=0x00000000;
	GPIOE->CRH|=0xBBBBBBBB;
	GPIOE->CRL&=0x0FFFFFFF;
	GPIOE->CRL|=0xB0000000;
	
	GPIOG->CRH&=0xFFF0FFFF;
	GPIOG->CRH|=0x000B0000;
	GPIOG->CRL&=0xFFFFFFF0;
	GPIOG->CRL|=0x0000000B;
	
	FSMC_Bank1->BTCR[6]=0x00000000;
	FSMC_Bank1->BTCR[7]=0x00000000;
	FSMC_Bank1E->BWTR[6]=0x00000000;
	FSMC_Bank1->BTCR[6]|=1<<12;
	FSMC_Bank1->BTCR[6]|=1<<14;
	FSMC_Bank1->BTCR[6]|=1<<4;
	FSMC_Bank1->BTCR[7]|=0<<28;
	FSMC_Bank1->BTCR[7]|=1<<0;
	FSMC_Bank1->BTCR[7]|=0xF<<8;
	FSMC_Bank1E->BWTR[6]|=0<<28;
	FSMC_Bank1E->BWTR[6]|=0<<0;
	FSMC_Bank1E->BWTR[6]|=3<<8;
	FSMC_Bank1->BTCR[6]|=1<<0;
	LCD_Set9341_Parameter();
	GPIOB->BSRR |= 1;//LCD_LIGHT_ON;
}

void LCD_Set9341_Parameter(void){
	LCD_WR_REG(0x01);
	LCD_WR_REG(0x11);
	
	LCD_WR_REG(0x3A);
	LCD_WR_DATA(0x55);
	
	LCD_WR_REG(0x29);
	
	LCD_WR_REG(0x36);
	LCD_WR_DATA(0xC8);
}

void LCD_WR_REG(u16 regval){
	LCD->LCD_REG=regval;
}

void LCD_WR_DATA(u16 data){
	LCD->LCD_RAM=data;
}

void IERG3810_TFTLCD_DrawDot(u16 x, u16 y, u16 color){
	LCD_WR_REG(0x2A);
		LCD_WR_DATA(x>>8);
		LCD_WR_DATA(x&0xFF);
		LCD_WR_DATA(0x01);
		LCD_WR_DATA(0x3F);
	LCD_WR_REG(0x2B);
		LCD_WR_DATA(y>>8);
		LCD_WR_DATA(y&0xFF);
		LCD_WR_DATA(0x01);
		LCD_WR_DATA(0xDF);
	LCD_WR_REG(0x2C);
	LCD_WR_DATA(color);
}

void IERG3810_TFTLCD_FillRectangle(u16 color, u16 start_x, u16 length_x, u16 start_y, u16 length_y){
	u32 index=0;
	LCD_WR_REG(0x2A);
		LCD_WR_DATA(start_x>>8);
		LCD_WR_DATA(start_x&0xFF);
		LCD_WR_DATA((length_x + start_x - 1)>>8);
		LCD_WR_DATA((length_x + start_x - 1)&0xFF);
	LCD_WR_REG(0x2B);
		LCD_WR_DATA(start_y>>8);
		LCD_WR_DATA(start_y&0xFF);
		LCD_WR_DATA((length_y + start_y - 1)>>8);
		LCD_WR_DATA((length_y + start_y - 1)&0xFF);
	LCD_WR_REG(0x2C);
	for(index=0;index<length_x * length_y;index++){
		LCD_WR_DATA(color);
	}
}

void IERG3810_TFTLCD_SevenSegment(u16 color, u16 start_x, u16 start_y, u8 digit){
	if(digit==0){
		IERG3810_TFTLCD_FillRectangle(color, start_x+8, 64, start_y+152, 8);				//print segment a
		IERG3810_TFTLCD_FillRectangle(color, start_x+72, 8, start_y+84, 68);				//print segment b
		IERG3810_TFTLCD_FillRectangle(color, start_x+72, 8, start_y+8, 68);					//print segment c
		IERG3810_TFTLCD_FillRectangle(color, start_x+8, 64, start_y, 8);						//print segment d
		IERG3810_TFTLCD_FillRectangle(color, start_x, 8, start_y+8, 68);						//print segment e
		IERG3810_TFTLCD_FillRectangle(color, start_x, 8, start_y+84, 68);						//print segment f
		IERG3810_TFTLCD_FillRectangle(0xFFFF, start_x+8, 64, start_y+76, 8);				//print segment g
	}else if(digit==1){
		IERG3810_TFTLCD_FillRectangle(0xFFFF, start_x+8, 64, start_y+152, 8);				//print segment a
		IERG3810_TFTLCD_FillRectangle(color, start_x+72, 8, start_y+84, 68);				//print segment b
		IERG3810_TFTLCD_FillRectangle(color, start_x+72, 8, start_y+8, 68);					//print segment c
		IERG3810_TFTLCD_FillRectangle(0xFFFF, start_x+8, 64, start_y, 8);						//print segment d
		IERG3810_TFTLCD_FillRectangle(0xFFFF, start_x, 8, start_y+8, 68);						//print segment e
		IERG3810_TFTLCD_FillRectangle(0xFFFF, start_x, 8, start_y+84, 68);					//print segment f
		IERG3810_TFTLCD_FillRectangle(0xFFFF, start_x+8, 64, start_y+76, 8);				//print segment g
	}else if(digit==2){
		IERG3810_TFTLCD_FillRectangle(color, start_x+8, 64, start_y+152, 8);				//print segment a
		IERG3810_TFTLCD_FillRectangle(color, start_x+72, 8, start_y+84, 68);				//print segment b
		IERG3810_TFTLCD_FillRectangle(0xFFFF, start_x+72, 8, start_y+8, 68);				//print segment c
		IERG3810_TFTLCD_FillRectangle(color, start_x+8, 64, start_y, 8);						//print segment d
		IERG3810_TFTLCD_FillRectangle(color, start_x, 8, start_y+8, 68);						//print segment e
		IERG3810_TFTLCD_FillRectangle(0xFFFF, start_x, 8, start_y+84, 68);					//print segment f
		IERG3810_TFTLCD_FillRectangle(color, start_x+8, 64, start_y+76, 8);					//print segment g
	}else if(digit==3){
		IERG3810_TFTLCD_FillRectangle(color, start_x+8, 64, start_y+152, 8);				//print segment a
		IERG3810_TFTLCD_FillRectangle(color, start_x+72, 8, start_y+84, 68);				//print segment b
		IERG3810_TFTLCD_FillRectangle(color, start_x+72, 8, start_y+8, 68);					//print segment c
		IERG3810_TFTLCD_FillRectangle(color, start_x+8, 64, start_y, 8);						//print segment d
		IERG3810_TFTLCD_FillRectangle(0xFFFF, start_x, 8, start_y+8, 68);						//print segment e
		IERG3810_TFTLCD_FillRectangle(0xFFFF, start_x, 8, start_y+84, 68);					//print segment f
		IERG3810_TFTLCD_FillRectangle(color, start_x+8, 64, start_y+76, 8);					//print segment g
	}else if(digit==4){
		IERG3810_TFTLCD_FillRectangle(0xFFFF, start_x+8, 64, start_y+152, 8);				//print segment a
		IERG3810_TFTLCD_FillRectangle(color, start_x+72, 8, start_y+84, 68);				//print segment b
		IERG3810_TFTLCD_FillRectangle(color, start_x+72, 8, start_y+8, 68);					//print segment c
		IERG3810_TFTLCD_FillRectangle(0xFFFF, start_x+8, 64, start_y, 8);						//print segment d
		IERG3810_TFTLCD_FillRectangle(0xFFFF, start_x, 8, start_y+8, 68);						//print segment e
		IERG3810_TFTLCD_FillRectangle(color, start_x, 8, start_y+84, 68);						//print segment f
		IERG3810_TFTLCD_FillRectangle(color, start_x+8, 64, start_y+76, 8);					//print segment g
	}else if(digit==5){
		IERG3810_TFTLCD_FillRectangle(color, start_x+8, 64, start_y+152, 8);				//print segment a
		IERG3810_TFTLCD_FillRectangle(0xFFFF, start_x+72, 8, start_y+84, 68);				//print segment b
		IERG3810_TFTLCD_FillRectangle(color, start_x+72, 8, start_y+8, 68);					//print segment c
		IERG3810_TFTLCD_FillRectangle(color, start_x+8, 64, start_y, 8);						//print segment d
		IERG3810_TFTLCD_FillRectangle(0xFFFF, start_x, 8, start_y+8, 68);						//print segment e
		IERG3810_TFTLCD_FillRectangle(color, start_x, 8, start_y+84, 68);						//print segment f
		IERG3810_TFTLCD_FillRectangle(color, start_x+8, 64, start_y+76, 8);					//print segment g
	}else if(digit==6){
		IERG3810_TFTLCD_FillRectangle(color, start_x+8, 64, start_y+152, 8);				//print segment a
		IERG3810_TFTLCD_FillRectangle(0xFFFF, start_x+72, 8, start_y+84, 68);				//print segment b
		IERG3810_TFTLCD_FillRectangle(color, start_x+72, 8, start_y+8, 68);					//print segment c
		IERG3810_TFTLCD_FillRectangle(color, start_x+8, 64, start_y, 8);						//print segment d
		IERG3810_TFTLCD_FillRectangle(color, start_x, 8, start_y+8, 68);						//print segment e
		IERG3810_TFTLCD_FillRectangle(color, start_x, 8, start_y+84, 68);						//print segment f
		IERG3810_TFTLCD_FillRectangle(color, start_x+8, 64, start_y+76, 8);					//print segment g
	}else if(digit==7){
		IERG3810_TFTLCD_FillRectangle(color, start_x+8, 64, start_y+152, 8);				//print segment a
		IERG3810_TFTLCD_FillRectangle(color, start_x+72, 8, start_y+84, 68);				//print segment b
		IERG3810_TFTLCD_FillRectangle(color, start_x+72, 8, start_y+8, 68);					//print segment c
		IERG3810_TFTLCD_FillRectangle(0xFFFF, start_x+8, 64, start_y, 8);						//print segment d
		IERG3810_TFTLCD_FillRectangle(0xFFFF, start_x, 8, start_y+8, 68);						//print segment e
		IERG3810_TFTLCD_FillRectangle(0xFFFF, start_x, 8, start_y+84, 68);					//print segment f
		IERG3810_TFTLCD_FillRectangle(0xFFFF, start_x+8, 64, start_y+76, 8);				//print segment g
	}else if(digit==8){
		IERG3810_TFTLCD_FillRectangle(color, start_x+8, 64, start_y+152, 8);				//print segment a
		IERG3810_TFTLCD_FillRectangle(color, start_x+72, 8, start_y+84, 68);				//print segment b
		IERG3810_TFTLCD_FillRectangle(color, start_x+72, 8, start_y+8, 68);					//print segment c
		IERG3810_TFTLCD_FillRectangle(color, start_x+8, 64, start_y, 8);						//print segment d
		IERG3810_TFTLCD_FillRectangle(color, start_x, 8, start_y+8, 68);						//print segment e
		IERG3810_TFTLCD_FillRectangle(color, start_x, 8, start_y+84, 68);						//print segment f
		IERG3810_TFTLCD_FillRectangle(color, start_x+8, 64, start_y+76, 8);					//print segment g
	}else if(digit==9){
		IERG3810_TFTLCD_FillRectangle(color, start_x+8, 64, start_y+152, 8);				//print segment a
		IERG3810_TFTLCD_FillRectangle(color, start_x+72, 8, start_y+84, 68);				//print segment b
		IERG3810_TFTLCD_FillRectangle(color, start_x+72, 8, start_y+8, 68);					//print segment c
		IERG3810_TFTLCD_FillRectangle(color, start_x+8, 64, start_y, 8);						//print segment d
		IERG3810_TFTLCD_FillRectangle(0xFFFF, start_x, 8, start_y+8, 68);						//print segment e
		IERG3810_TFTLCD_FillRectangle(color, start_x, 8, start_y+84, 68);						//print segment f
		IERG3810_TFTLCD_FillRectangle(color, start_x+8, 64, start_y+76, 8);					//print segment g
	}
}

void IERG3810_TFTLCD_ShowChar(u16 x, u16 y, u8 ascii, u16 color, u16 bgcolor){
	u8 i, j;
	u8 index;
	u8 height=16, length=8;
	if(ascii<32 || ascii>127) return;
	ascii -=32;
	LCD_WR_REG(0x2A);
		LCD_WR_DATA(x>>8);
		LCD_WR_DATA(x&0xFF);
		LCD_WR_DATA((length+x-1)>>8);
		LCD_WR_DATA((length+x-1)&0xFF);
	LCD_WR_REG(0x2B);
		LCD_WR_DATA(y>>8);
		LCD_WR_DATA(y&0xFF);
		LCD_WR_DATA((height+y-1)>>8);
		LCD_WR_DATA((height+y-1)&0xFF);
	LCD_WR_REG(0x2C);
	for(j=0;j<height/8; j++){
		for(i=0;i<height/2;i++){
			for(index=0;index<length;index++){
				if((asc2_1608[ascii][index*2+1-j]>>i)&0x01) LCD_WR_DATA(color);
				else LCD_WR_DATA(bgcolor);
			}
		}
	}
}

/*void IERG3810_TFTLCD_ShowChinChar(u16 x, u16 y, u8 chi, u16 color, u16 bgcolor){
	u8 i, j;
	u8 index;
	u8 height=16, length=16;
	LCD_WR_REG(0x2A);
		LCD_WR_DATA(x>>8);
		LCD_WR_DATA(x&0xFF);
		LCD_WR_DATA((length+x-1)>>8);
		LCD_WR_DATA((length+x-1)&0xFF);
	LCD_WR_REG(0x2B);
		LCD_WR_DATA(y>>8);
		LCD_WR_DATA(y&0xFF);
		LCD_WR_DATA((height+y-1)>>8);
		LCD_WR_DATA((height+y-1)&0xFF);
	LCD_WR_REG(0x2C);
	for(j=0;j<height/8; j++){
		for(i=0;i<height/2;i++){
			for(index=0;index<length;index++){
				if((chi_1616[chi][index*2+1-j]>>i)&0x01) LCD_WR_DATA(color);
				else LCD_WR_DATA(bgcolor);
			}
		}
	}
}*/


/*void IERG3810_TFTLCD_testShowChinChar(u16 x, u16 y, u8 chi, u16 color){										
	u8 i, j;
	u8 index;
	u8 height=16, length=16;
	for(j=0;j<height/8; j++){
		for(i=0;i<height/2;i++){
			for(index=0;index<length;index++){
				if((chi_1616[chi][index*2+1-j]>>i)&0x01) IERG3810_TFTLCD_DrawDot(x+index, y+i+(j*8),color);	//if this pixel is the part of the word then draw a dot on it
				else continue;																																							//else skip this pixel
			}
		}
	}
}*/

void IERG3810_TFTLCD_testShowChar(u16 x, u16 y, u8 ascii, u16 color){										
	u8 i, j;
	u8 index;
	u8 height=16, length=8;
	if(ascii<32 || ascii>127) return;
	ascii -=32;
	for(j=0;j<height/8; j++){
		for(i=0;i<height/2;i++){
			for(index=0;index<length;index++){
				if((asc2_1608[ascii][index*2+1-j]>>i)&0x01) IERG3810_TFTLCD_DrawDot(x+index, y+i+(j*8),color);	//if this pixel is the part of the word then draw a dot on it
				else continue;																																							//else skip this pixel
			}
		}
	}
}

void printString(u16 startX, u16 startY, char *string, int num, u16 color){
	int i,j;
	for(i=0;i<num;i++){
		IERG3810_TFTLCD_testShowChar(i*8+startX,startY,string[i],color);
	}
}

