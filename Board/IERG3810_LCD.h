#include "stm32f10x.h"

void IERG3810_TFTLCD_Init(void);

void LCD_Set9341_Parameter(void);

void LCD_WR_REG(u16 regval);

void LCD_WR_DATA(u16 data);

void IERG3810_TFTLCD_DrawDot(u16 x, u16 y, u16 color);

void IERG3810_TFTLCD_FillRectangle(u16 color, u16 start_x, u16 length_x, u16 start_y, u16 length_y);

void IERG3810_TFTLCD_SevenSegment(u16 color, u16 start_x, u16 start_y, u8 digit);

void IERG3810_TFTLCD_ShowChar(u16 x, u16 y, u8 ascii, u16 color, u16 bgcolor);

void IERG3810_TFTLCD_ShowChinChar(u16 x, u16 y, u8 chi, u16 color, u16 bgcolor);

void IERG3810_TFTLCD_testShowChinChar(u16 x, u16 y, u8 chi, u16 color);

void IERG3810_TFTLCD_testShowChar(u16 x, u16 y, u8 ascii, u16 color);

void printString(u16 startX, u16 startY, char *string, int num, u16 color);

void printbuf(u16 **buf, u16 x, u16 y, u16 xlen, u16 ylen);

#define RED 31<<11
#define GREEN 63<<5
#define BLUE 31
#define WHITE 0xFFFF
#define BLACK 0
#define YELLOW GREEN|RED