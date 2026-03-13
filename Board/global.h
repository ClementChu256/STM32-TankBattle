#ifndef __GLOBAL
#define __GLOBAL
#include "stm32f10x.h"
//some global variable for calculating cooldown

extern u16 player1firecooldown;
extern u16 player1explosioncooldown;

extern u16 player2firecooldown;
extern u16 player2explosioncooldown;

extern u16 Complayerfirecooldown[4];
extern u16 Complayerexplosioncooldown[4];

extern u16 Complayercheckcollisioncooldown[4];

extern u16 explosionStateCounter[6];

extern u16 randomSeed;
#endif