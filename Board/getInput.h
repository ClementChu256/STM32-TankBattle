#include "stm32f10x.h"
#define keyA input[0]
#define keyB input[1]
#define keySelect input[2]
#define keyStart input[3]
#define keyUp input[4]
#define keyDown input[5]
#define keyLeft input[6]
#define keyRight input[7]

extern int input[8];

void joystickInit();

void IERG3810_TIM3_Init(u16 arr, u16 psc);

void TIM3_IRQHandler();

