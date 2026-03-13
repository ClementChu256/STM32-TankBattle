#include "stm32f10x.h"
#include "game.h"
#include "getInput.h"
#include <string.h>
#include <stdlib.h>
#include "entity.h"
#include "map.h"
#include "global.h"

#define player1shot shot[0]
#define player2shot shot[1]
#define com1shot shot[2]
#define com2shot shot[3]
#define com3shot shot[4]
#define com4shot shot[5]


//define some global variable
int Ingame=0;
int gameOver=0;
int live;
int nextTank=0;
int comCounter=0;
int spawnLocation=0;
int maxTank=MAXTANK;
int win;

u16 player1firecooldown=0;
u16 player1explosioncooldown=0;

u16 player2firecooldown=0;
u16 player2explosioncooldown=0;

u16 Complayerfirecooldown[4]={0};
u16 Complayerexplosioncooldown[4]={0};
u16 Complayercheckcollisioncooldown[4];
u16 explosionStateCounter[6];

u16 randomSeed=0;

u8 map1[30][30];
entity player1, player2;
entity shot[6];
entity Complayer[4];

void coverLastPosition(entity Entity){
	//function for covering the entity last position with bg color
	int startX,startY,endX,endY;
	int i,j;
	if(Entity.lastposX==Entity.posX){
		//move along y
		if(Entity.lastposY<Entity.posY){
			startY=Entity.lastposY;
			endY=Entity.posY;
		}else{
			startY=Entity.posY+Entity.sizeY;
			endY=Entity.lastposY+Entity.sizeY;
		}
		startX=Entity.lastposX;
		endX=Entity.lastposX+Entity.sizeX;
	}else if(Entity.lastposY==Entity.posY){
		//move along x
		if(Entity.lastposX<Entity.posX){
			startX=Entity.lastposX;
			endX=Entity.posX;
		}else{
			startX=Entity.posX+Entity.sizeX;
			endX=Entity.lastposX+Entity.sizeX;
		}
		startY=Entity.lastposY;
		endY=Entity.lastposY+Entity.sizeY;
	}
	
		//cover the entity last position
	if(Entity.lastposY==Entity.posY){
		for(i=startX;i<endX;i++){
			for(j=startY;j<endY;j++){
				if(map1[29-j/8][i/8]==3){
					if(bush[i%8][j%8]!=0x0000) continue;
					IERG3810_TFTLCD_DrawDot(i,j,0);
				}else IERG3810_TFTLCD_DrawDot(i,j,0);
			}
		}
	}else if(Entity.lastposX==Entity.posX){
		for(i=startX;i<endX;i++){
		for(j=startY;j<endY;j++){
					if(map1[29-j/8][i/8]==3){
					if(bush[i%8][j%8]!=0x0000) continue;
					IERG3810_TFTLCD_DrawDot(i,j,0);
				}else IERG3810_TFTLCD_DrawDot(i,j,0);
			}
		}
	}
}

void IERG3810_TIM4_Init(u16 arr, u16 psc){
	RCC->APB1ENR|=1<<2;				//enable TIM4 clock
	TIM4->ARR=arr;						//set arr
	TIM4->PSC=psc;						//set psc
	TIM4->DIER|=1<<0;					//enable interrupt
	NVIC->IP[30]=0x65;				//set priority
	NVIC->ISER[0]|=(1<<30);		//enable interrupt
}

void IERG3810_TIM5_Init(u16 arr, u16 psc){
	RCC->APB1ENR|=1<<3;				//enable TIM5 clock
	TIM5->ARR=arr;						//set arr
	TIM5->PSC=psc;						//set psc
	TIM5->DIER|=1<<0;					//enable interrupt
	NVIC->IP[50]=0x65;				//set priority
	NVIC->ISER[1]|=(1<<18);		//enable interrupt
}

void IERG3810_SYSTICK_Init10ms(){
	SysTick->CTRL=0;						//disable systick
	SysTick->LOAD=89999;				//value load to current value register
	SysTick->VAL=0;
	SysTick->CTRL|=3<<0;				//enable systick and handler, use external clock	
}

void playerInit(){
	//player init
	player1.dir=playerInitDir;
	player1.health=playerHealth;
	player1.posX=playerStartPosX;
	player1.posY=playerStartPosY;
	player1.lastposX=playerStartPosX;
	player1.lastposY=playerStartPosY;
	player1.speed=playerSpeed;
	player1.sizeX=playerSizeX;
	player1.sizeY=playerSizeY;
	player1.shot=0;
	player1.alive=1;
	player1.type=0;
	player1.firecooldown=0;
	player1.explosion=0;
	player2.posX=-100;
	player2.posY=-100;
	player1.state=0;
	player1.explosionState=0;
}

int comInit(int posx, int posy){
	//Complayer init
	int i,flag=0;
	for(i=0;i<4;i++){
		if(Complayer[i].alive==1||Complayer[i].explosion==1) continue;
		else{
			flag=1;
			Complayer[i].dir=2;
			Complayer[i].alive=1;
			Complayer[i].sizeX=16;
			Complayer[i].sizeY=16;
			Complayer[i].posX=posx;
			Complayer[i].posY=posy;
			Complayer[i].lastposX=posx;
			Complayer[i].lastposY=posy;
			Complayer[i].firecooldown=0;
			Complayer[i].type=maptank[map][nextTank]+2;
			Complayer[i].explosion=0;
			Complayercheckcollisioncooldown[i]=0;
			Complayer[i].ignoreCollision=1;
			Complayer[i].state=0;
			Complayer[i].explosionState=0;
			if(maptank[map][nextTank]==0){
				//print tank type 0
				Complayer[i].health=1;
				Complayer[i].speed=type0Speed;
				Complayer[i].shot=0;
			}else if(maptank[map][nextTank]==1){
				//print tank type 1
				Complayer[i].health=1;
				Complayer[i].speed=type1Speed;
				Complayer[i].shot=0;
			}else if(maptank[map][nextTank]==2){
				//print tank type 2
				Complayer[i].health=1;
				Complayer[i].speed=type0Speed;
				Complayer[i].shot=0;
			}else if(maptank[map][nextTank]==3){
				//print tank type 3
				Complayer[i].health=4;
				Complayer[i].speed=type3Speed;
				Complayer[i].shot=0;
			}
			break;
		}
	}
	return flag;
}

void printElement(u16 const buf[8][8], u16 x, u16 y, u16 xlen, u16 ylen){
	int i,j;
	for(i=x;i<x+xlen;i++) for(j=y;j<y+ylen;j++) IERG3810_TFTLCD_DrawDot(i,j,buf[i-x][j-y]);
}

void printbase(u16 const buf[16][16], u16 x, u16 y){
	int i,j;
	for(i=x;i<x+16;i++) for(j=y;j<y+16;j++) IERG3810_TFTLCD_DrawDot(i,j,buf[i-x][j-y]);
}

void printmap(u8 const map[30][30]){
	//print map element
	int i,j;
	for(i=0;i<30;i++){
		for(j=0;j<30;j++){
				if(map[29-j][i]==0||map[29-j][i]==4) continue;
				else if(map[29-j][i]==1) printElement(brick,8*i,8*j,8,8);
				else if(map[29-j][i]==2) printElement(steel,8*i,8*j,8,8);
				else if(map[29-j][i]==3) printElement(bush,8*i,8*j,8,8);
		}
	}
}

void loadMap(){
	//print map
	int i,j,k,m;
	IERG3810_TFTLCD_FillRectangle(0x528A, 0, 240, 0, 320);
	IERG3810_TFTLCD_FillRectangle(0, 0, 240, 0, 240);
	for(k=274;k<=294;k=k+20){
		for(m=42;m<=222;m=m+20){
			for(i=m;i<m+16;i++){
				for(j=k;j<k+16;j++){
					if(tank[2][0][i-m][j-k]==0x0000) continue;
					IERG3810_TFTLCD_DrawDot(i,j,tank[2][0][i-m][j-k]);
				}
			}
		}
	}
	printmap(map1);
	printbase(base,112,0);
}

void gameInit(){
	//init all
	int i,j;
	Ingame=1;
	IERG3810_SYSTICK_Init10ms();
	playerInit();
	live=initLive;
	for(i=0;i<6;i++){
		shot[i].dir=0;
		shot[i].health=0;
		shot[i].posX=0;
		shot[i].posY=0;
		shot[i].lastposX=0;
		shot[i].lastposY=0;
		shot[i].shot=0;
		shot[i].sizeX=0;
		shot[i].sizeY=0;
		shot[i].speed=0;
		shot[i].explosion=0;
	}
	for(i=0;i<4;i++){
		Complayer[i].alive=0;
		Complayer[i].posX=-100;
		Complayer[i].posY=-100;
	}
	nextTank=0;
	spawnLocation=1;
	//copy map data to buffer for further modification
	for(i=0;i<30;i++){
		for(j=0;j<30;j++) map1[i][j]=mapdata[map][i][j];
	}
	loadMap(); //print map
	gameOver=0;
	comCounter=0;
	win=0;
	IERG3810_TIM4_Init(499,7199);  //get update 20hz
	IERG3810_TIM5_Init(39999, 7199); //spawn Complayer every 4 secs
	TIM4->CR1|=0x01;					//enable counter
	TIM5->CR1|=0x01;					//enable counter
}

void shotFire(entity Entity, int index){
	
	shot[index].dir=Entity.dir;
	shot[index].health=1;
	
	
	shot[index].posX=Entity.posX+6;
	shot[index].posY=Entity.posY+6;
	shot[index].lastposX=shot[index].posX;
	shot[index].lastposY=shot[index].posY;
	

	shot[index].shot=1;
	shot[index].sizeX=4;
	shot[index].sizeY=5;
	if(index>=2){
		if(Complayer[index-2].type==4||Complayer[index-2].type==3){
			shot[index].speed=bulletSpeed*2;
			return;
		}
	}
	shot[index].speed=bulletSpeed;
}

int checkvalid(entity Entity, u8 dir){
	//check tank valid to move forward, return 0 if not valid
	int i;
	u8 tmp[3];
	if(dir==0){
		if(Entity.posY>=240-Entity.sizeY) return 0;  //edge of the map
		tmp[0]=map1[29-(Entity.posY+Entity.speed+Entity.sizeY-1)/8][Entity.posX/8];
		tmp[1]=map1[29-(Entity.posY+Entity.speed+Entity.sizeY-1)/8][(Entity.posX+8)/8];
		if(Entity.posX%8!=0) tmp[2]=map1[29-(Entity.posY+Entity.speed+Entity.sizeY-1)/8][(Entity.posX+16)/8];
		else tmp[2]=0;
		for(i=0;i<3;i++) if(tmp[i]==1||tmp[i]==2||tmp[i]==4) return 0; //nextpos is obstacle 
		
	}else if(dir==1){
		if(Entity.posX>=240-Entity.sizeX) return 0;	//edge of the map
		tmp[0]=map1[29-(Entity.posY)/8][(Entity.posX+Entity.speed+Entity.sizeX-1)/8];
		tmp[1]=map1[29-(Entity.posY+8)/8][(Entity.posX+Entity.speed+Entity.sizeX-1)/8];
		if(Entity.posY%8!=0) tmp[2]=map1[29-(Entity.posY+16)/8][(Entity.posX+Entity.speed+Entity.sizeX-1)/8];
		else tmp[2]=0;
		for(i=0;i<3;i++) if(tmp[i]==1||tmp[i]==2||tmp[i]==4) return 0; //nextpos is obstacle
	}else if(dir==2){
		if(Entity.posY<=0) return 0;	//edge of the map
		tmp[0]=map1[29-(Entity.posY-Entity.speed)/8][Entity.posX/8];
		tmp[1]=map1[29-(Entity.posY-Entity.speed)/8][(Entity.posX+8)/8];
		if(Entity.posX%8!=0) tmp[2]=map1[29-(Entity.posY-Entity.speed)/8][(Entity.posX+16)/8];
		else tmp[2]=0;
		for(i=0;i<3;i++) if(tmp[i]==1||tmp[i]==2||tmp[i]==4) return 0;  //nextpos is obstacle
	}else if(dir==3){
		if(Entity.posX<=0) return 0;	//edge of the map
		tmp[0]=map1[29-(Entity.posY)/8][(Entity.posX-Entity.speed)/8];
		tmp[1]=map1[29-(Entity.posY+8)/8][(Entity.posX-Entity.speed)/8];
		if(Entity.posY%8!=0) tmp[2]=map1[29-(Entity.posY+16)/8][(Entity.posX-Entity.speed)/8];
		else tmp[2]=0;
		for(i=0;i<3;i++) if(tmp[i]==1||tmp[i]==2||tmp[i]==4) return 0;	//nextpos is obstacle
	}
	
	
	return 1;
}










int checkHit(int index){
	int i,j,k;
	int x1,x2,y1,y2;
	int bulletx1,bulletx2,bullety1,bullety2;
	bulletx1=shot[index].posX;
	bulletx2=shot[index].posX+shot[index].sizeX-1;
	bullety1=shot[index].posY;
	bullety2=shot[index].posY+shot[index].sizeY-1;
	
	//check hit other bullet	
	for(i=0;i<6;i++){
		if(i==index) continue;
		if(shot[i].shot==0) continue;
		x1=shot[i].posX;
		x2=shot[i].posX+shot[i].sizeX-1;
		y1=shot[i].posY;
		y2=shot[i].posY+shot[i].sizeY-1;
		if(((x1<=bulletx1&&bulletx1<=x2)&&(y1<=bullety1&&bullety1<=y2))||((x1<=bulletx2&&bulletx2<=x2)&&(y1<=bullety1&&bullety1<=y2))||((x1<=bulletx2&&bulletx2<=x2)&&(y1<=bullety2&&bullety2<=y2))||((x1<=bulletx1&&bulletx1<=x2)&&(y1<=bullety2&&bullety2<=y2))){
			shot[i].shot=0;
			shot[i].explosion=1;
			if(i==0){
				player1.shot=0;
				player1.firecooldown=1;
				player1firecooldown=0;
			}else if(i>=2){
				Complayer[i-2].shot=0;
				Complayer[i-2].firecooldown=1;
				Complayerfirecooldown[i-2]=0;
	
			} 
			/*for(k=shot[i].posX;k<shot[i].sizeX+shot[i].posX;k++){
				for(j=shot[i].posY;j<shot[i].sizeY+shot[i].posY;j++){
					if(map1[29-j/8][k/8]==3){
						if(bush[k%8][j%8]!=0x0000) continue;
						IERG3810_TFTLCD_DrawDot(k,j,0);
					}else IERG3810_TFTLCD_DrawDot(k,j,0);
				}
			}*/
			if(i>index){
				for(k=shot[i].posX;k<shot[i].sizeX+shot[i].posX;k++){
					for(j=shot[i].posY;j<shot[i].sizeY+shot[i].posY;j++){
						if(map1[29-j/8][k/8]==3){
							if(bush[k%8][j%8]!=0x0000) continue;
							IERG3810_TFTLCD_DrawDot(k,j,0);
						}else IERG3810_TFTLCD_DrawDot(k,j,0);
					}
				}
			}else if(i<index){
				for(k=shot[i].lastposX;k<shot[i].sizeX+shot[i].lastposX;k++){
					for(j=shot[i].lastposY;j<shot[i].sizeY+shot[i].lastposY;j++){
						if(map1[29-j/8][k/8]==3){
							if(bush[k%8][j%8]!=0x0000) continue;
							IERG3810_TFTLCD_DrawDot(k,j,0);
						}else IERG3810_TFTLCD_DrawDot(k,j,0);
					}
				}
			}
			
		return 1;
		}
	}
	//check hit any entity
	if(index<2){
		//bullet from player
		for(i=0;i<4;i++){
			if(Complayer[i].alive==0) continue;
			x1=Complayer[i].posX;
			x2=Complayer[i].posX+Complayer[i].sizeX-1;
			y1=Complayer[i].posY;
			y2=Complayer[i].posY+Complayer[i].sizeY-1;
			if(((x1<=bulletx1&&bulletx1<=x2)&&(y1<=bullety1&&bullety1<=y2))||((x1<=bulletx2&&bulletx2<=x2)&&(y1<=bullety1&&bullety1<=y2))||((x1<=bulletx2&&bulletx2<=x2)&&(y1<=bullety2&&bullety2<=y2))||((x1<=bulletx1&&bulletx1<=x2)&&(y1<=bullety2&&bullety2<=y2))){ //if collision
				if(Complayer[i].type==5&&Complayer[i].health>1) Complayer[i].health--;
				else{
					Complayer[i].alive=0;
					Complayer[i].explosion=1;
					explosionStateCounter[i+2]=0;
					Complayerexplosioncooldown[i]=0;
					comCounter--;
					//cover the last position
					coverLastPosition(Complayer[i]);
				}
			
			
			
			
				return 1;
			}
		}
	}else if(index>=2){
		//check if combullet hit player
		if(player1.alive==1){
			x1=player1.posX;
			x2=player1.posX+player1.sizeX-1;
			y1=player1.posY;
			y2=player1.posY+player1.sizeY-1;
			if(((x1<=bulletx1&&bulletx1<=x2)&&(y1<=bullety1&&bullety1<=y2))||((x1<=bulletx2&&bulletx2<=x2)&&(y1<=bullety1&&bullety1<=y2))||((x1<=bulletx2&&bulletx2<=x2)&&(y1<=bullety2&&bullety2<=y2))||((x1<=bulletx1&&bulletx1<=x2)&&(y1<=bullety2&&bullety2<=y2))){ //if collision
				player1.alive=0;
				player1.explosion=1;
				explosionStateCounter[0]=0;
				player1explosioncooldown=0;
				live--;
				//cover the last position
				coverLastPosition(player1);
			
			
			
			
			
				
				return 1;
			}
		}
		                  
	}
	return 0;
}


int checkbulletvalid(entity Entity, int dir, int index){
	//check if bullet is valid to move forward
	int i,flag;
	u8 tmp;
	flag=0;
	if(dir==0){
		if(Entity.posY+Entity.speed>=240-Entity.sizeY) return 0;  //edge of the map
		if(checkHit(index)) return 0;
		for(i=0;i<2;i++){
			tmp=map1[29-(Entity.posY+Entity.speed+Entity.sizeY-1)/8][(Entity.posX+i*3)/8];
			if(tmp==2) flag=1;	//nextpos is obstacle
			else if(tmp==1){
				flag=1;
				map1[29-(Entity.posY+Entity.speed+Entity.sizeY-1)/8][(Entity.posX+i*3)/8]=0;
				printElement(empty,((Entity.posX+i*3)/8)*8,((Entity.posY+Entity.speed+Entity.sizeY-1)/8)*8,8,8);
			}else if(tmp==4){
				flag=1;
				gameOver=1;
				printbase(destroyedBase,112,0);
			}
		}
	}else if(dir==1){
		if(Entity.posX+Entity.speed>=240-Entity.sizeX) return 0;	//edge of the map
		if(checkHit(index)) return 0;
		for(i=0;i<2;i++){
			tmp=map1[29-(Entity.posY+i*3)/8][(Entity.posX+Entity.speed+Entity.sizeX-1)/8];
			if(tmp==2) flag=1;	//nextpos is obstacle
			else if(tmp==1){
				flag=1;
				map1[29-(Entity.posY+i*3)/8][(Entity.posX+Entity.speed+Entity.sizeX-1)/8]=0;
				printElement(empty,((Entity.posX+Entity.speed+Entity.sizeX-1)/8)*8,((Entity.posY+i*3)/8)*8,8,8);
			}else if(tmp==4){
				flag=1;
				gameOver=1;
				printbase(destroyedBase,112,0);
			}
		}
	}else if(dir==2){
		if(Entity.posY-Entity.speed<=0) return 0;	//edge of the map
		if(checkHit(index)) return 0;
		for(i=0;i<2;i++){
			tmp=map1[29-(Entity.posY-Entity.speed)/8][(Entity.posX+i*3)/8];
			if(tmp==2) flag=1;	//nextpos is obstacle
			else if(tmp==1){
				flag=1;
				map1[29-(Entity.posY-Entity.speed)/8][(Entity.posX+i*3)/8]=0;
				printElement(empty,((Entity.posX+i*3)/8)*8,((Entity.posY-Entity.speed)/8)*8,8,8);
			}else if(tmp==4){
				flag=1;
				gameOver=1;
				printbase(destroyedBase,112,0);
			}
		}
	}else if(dir==3){
		if(Entity.posX-Entity.speed<=0) return 0;	//edge of the map
		if(checkHit(index)) return 0;
		for(i=0;i<2;i++){
			tmp=map1[29-(Entity.posY+i*3)/8][(Entity.posX-Entity.speed)/8];
			if(tmp==2) flag=1;	//nextpos is obstacle
			else if(tmp==1){
				flag=1;
				map1[29-(Entity.posY+i*3)/8][(Entity.posX-Entity.speed)/8]=0;
				printElement(empty,((Entity.posX-Entity.speed)/8)*8,((Entity.posY+i*3)/8)*8,8,8);
			}else if(tmp==4){
				flag=1;
				gameOver=1;
				printbase(destroyedBase,112,0);
			}
		}
	}
	
	if(flag==1) return 0;
	return 1;
}

void bulletMovement(){
	int i,j,k;
	int negative=1;
	for(i=0;i<6;i++){
		if(shot[i].shot==1){
			if(shot[i].dir==2||shot[i].dir==3) negative=-1;
			if(!checkbulletvalid(shot[i],shot[i].dir,i)){
				//explode
				shot[i].shot=0;
				shot[i].explosion=1;
				for(k=shot[i].posX;k<shot[i].sizeX+shot[i].posX;k++){
					for(j=shot[i].posY;j<shot[i].sizeY+shot[i].posY;j++){
						if(map1[29-j/8][k/8]==3){
								if(bush[k%8][j%8]!=0x0000) continue;
							IERG3810_TFTLCD_DrawDot(k,j,0);
						}else IERG3810_TFTLCD_DrawDot(k,j,0);
					}
				}
				if(i==0){
					player1.shot=0;
					player1.firecooldown=1;
					player1firecooldown=0;
				}
				if(i>=2){
					Complayer[i-2].shot=0;
					Complayer[i-2].firecooldown=1;
					Complayerfirecooldown[i-2]=0;
					srand(randomSeed);
					if(Complayer[i-2].type==3) Complayer[i-2].cooldownTime=50+rand()%750;
					else if(Complayer[i-2].type==4) Complayer[i-2].cooldownTime=50+rand()%250;
					else Complayer[i-2].cooldownTime=50+rand()%500;
				}
				continue;
			}
			if(shot[i].dir==0){
				shot[i].posY+=shot[i].speed;
			}else if(shot[i].dir==1){
				shot[i].posX+=shot[i].speed;
			}else if(shot[i].dir==2){
				shot[i].posY-=shot[i].speed;
			}else if(shot[i].dir==3){
				shot[i].posX-=shot[i].speed;
			}
		}else if(shot[i].explosion==1){
			for(k=shot[i].posX;k<shot[i].sizeX+shot[i].posX;k++){
				for(j=shot[i].posY;j<shot[i].sizeY+shot[i].posY;j++){
					if(map1[29-j/8][k/8]==3){
						if(bush[k%8][j%8]!=0x0000) continue;
						IERG3810_TFTLCD_DrawDot(k,j,0);
					}else IERG3810_TFTLCD_DrawDot(k,j,0);
				}
			}
			shot[i].explosion=0;
		}
	}
}
int ComplayerCheckInsideCollision(int index){
	//check collision function for complayer to avoid stuck at spawnpoint
	int x1,y1,x2,y2,tmpx1,tmpx2,tmpy1,tmpy2;
	int i;
	x1=Complayer[index].posX;
	x2=Complayer[index].posX+Complayer[index].sizeX-1;
	y1=Complayer[index].posY;
	y2=Complayer[index].posY+Complayer[index].sizeY-1;
	
	tmpx1=player1.posX;
	tmpx2=player1.posX+player1.sizeX-1;
	tmpy1=player1.posY;
	tmpy2=player1.posY+player1.sizeY-1;
	if(((x1<=tmpx1&&tmpx1<=x2)&&(y1<=tmpy1&&tmpy1<=y2))||((x1<=tmpx2&&tmpx2<=x2)&&(y1<=tmpy1&&tmpy1<=y2))||((x1<=tmpx2&&tmpx2<=x2)&&(y1<=tmpy2&&tmpy2<=y2))||((x1<=tmpx1&&tmpx1<=x2)&&(y1<=tmpy2&&tmpy2<=y2))){ //if collision
			return 1;
	}
	for(i=0;i<4;i++){
		if(i==index) continue;
		if(Complayer[i].alive==0) continue;
		tmpx1=Complayer[i].posX;
		tmpx2=Complayer[i].posX+Complayer[i].sizeX-1;
		tmpy1=Complayer[i].posY;
		tmpy2=Complayer[i].posY+Complayer[i].sizeY-1;
		if(((x1<=tmpx1&&tmpx1<=x2)&&(y1<=tmpy1&&tmpy1<=y2))||((x1<=tmpx2&&tmpx2<=x2)&&(y1<=tmpy1&&tmpy1<=y2))||((x1<=tmpx2&&tmpx2<=x2)&&(y1<=tmpy2&&tmpy2<=y2))||((x1<=tmpx1&&tmpx1<=x2)&&(y1<=tmpy2&&tmpy2<=y2))){ //if collision
			return 1;
		}
	}
	return 0;
}

int ComplayerCheckCollision(int index){
	//Check collsion function for complayer, same logic as player
	int x1,y1,x2,y2,tmpx1,tmpx2,tmpy1,tmpy2;
	int i;
	if(Complayer[index].dir==0){
		x1=Complayer[index].posX;
		x2=Complayer[index].posX+Complayer[index].sizeX-1;
		y1=Complayer[index].posY+Complayer[index].speed;
		y2=Complayer[index].posY+Complayer[index].sizeY+Complayer[index].speed-1;
	}else if(Complayer[index].dir==1){
		y1=Complayer[index].posY;
		y2=Complayer[index].posY+Complayer[index].sizeY-1;
		x1=Complayer[index].posX+Complayer[index].speed;
		x2=Complayer[index].posX+Complayer[index].sizeX+Complayer[index].speed-1;
	}else if(Complayer[index].dir==2){
		x1=Complayer[index].posX;
		x2=Complayer[index].posX+Complayer[index].sizeX-1;
		y1=Complayer[index].posY-Complayer[index].speed;
		y2=Complayer[index].posY+Complayer[index].sizeY-Complayer[index].speed-1;
	}else if(Complayer[index].dir==3){
		y1=Complayer[index].posY;
		y2=Complayer[index].posY+Complayer[index].sizeY-1;
		x1=Complayer[index].posX-Complayer[index].speed;
		x2=Complayer[index].posX+Complayer[index].sizeX-Complayer[index].speed-1;
	}
	tmpx1=player1.posX;
	tmpx2=player1.posX+player1.sizeX-1;
	tmpy1=player1.posY;
	tmpy2=player1.posY+player1.sizeY-1;
	if(((x1<=tmpx1&&tmpx1<=x2)&&(y1<=tmpy1&&tmpy1<=y2))||((x1<=tmpx2&&tmpx2<=x2)&&(y1<=tmpy1&&tmpy1<=y2))||((x1<=tmpx2&&tmpx2<=x2)&&(y1<=tmpy2&&tmpy2<=y2))||((x1<=tmpx1&&tmpx1<=x2)&&(y1<=tmpy2&&tmpy2<=y2))){ //if collision
			return 1;
	}
	for(i=0;i<4;i++){
		if(i==index) continue;
		if(Complayer[i].alive==0) continue;
		tmpx1=Complayer[i].posX;
		tmpx2=Complayer[i].posX+Complayer[i].sizeX-1;
		tmpy1=Complayer[i].posY;
		tmpy2=Complayer[i].posY+Complayer[i].sizeY-1;
		if(((x1<=tmpx1&&tmpx1<=x2)&&(y1<=tmpy1&&tmpy1<=y2))||((x1<=tmpx2&&tmpx2<=x2)&&(y1<=tmpy1&&tmpy1<=y2))||((x1<=tmpx2&&tmpx2<=x2)&&(y1<=tmpy2&&tmpy2<=y2))||((x1<=tmpx1&&tmpx1<=x2)&&(y1<=tmpy2&&tmpy2<=y2))){ //if collision
			return 1;
		}
	}
	return 0;
}


int playerCheckCollision(int player){
	int x1,y1,x2,y2,tmpx1,tmpx2,tmpy1,tmpy2;
	int i;
	if(player1.dir==0){
		x1=player1.posX;
		x2=player1.posX+player1.sizeX-1;
		y1=player1.posY+player1.speed;
		y2=player1.posY+player1.sizeY+player1.speed-1;
	}else if(player1.dir==1){
		y1=player1.posY;
		y2=player1.posY+player1.sizeY-1;
		x1=player1.posX+player1.speed;
		x2=player1.posX+player1.sizeX+player1.speed-1;
	}else if(player1.dir==2){
		x1=player1.posX;
		x2=player1.posX+player1.sizeX-1;
		y1=player1.posY-player1.speed;
		y2=player1.posY+player1.sizeY-player1.speed-1;
	}else if(player1.dir==3){
		y1=player1.posY;
		y2=player1.posY+player1.sizeY-1;
		x1=player1.posX-player1.speed;
		x2=player1.posX+player1.sizeX-player1.speed-1;
	}
	
	
	
	if(player==1){
		if(player1.dir==0){
			x1=player1.posX;
			x2=player1.posX+player1.sizeX-1;
			y1=player1.posY+player1.speed;
			y2=player1.posY+player1.sizeY+player1.speed-1;
		}else if(player1.dir==1){
			y1=player1.posY;
			y2=player1.posY+player1.sizeY-1;
			x1=player1.posX+player1.speed;
			x2=player1.posX+player1.sizeX+player1.speed-1;
		}else if(player1.dir==2){
			x1=player1.posX;
			x2=player1.posX+player1.sizeX-1;
			y1=player1.posY-player1.speed;
			y2=player1.posY+player1.sizeY-player1.speed-1;
		}else if(player1.dir==3){
			y1=player1.posY;
			y2=player1.posY+player1.sizeY-1;
			x1=player1.posX-player1.speed;
			x2=player1.posX+player1.sizeX-player1.speed-1;
		}
		tmpx1=player2.posX;
		tmpx2=player2.posX+player2.sizeX-1;
		tmpy1=player2.posY;
		tmpy2=player2.posY+player2.sizeY-1;
		}else if(player==2){ //ignore player2 as not implemented
		if(player2.dir==0){
			x1=player2.posX;
			x2=player2.posX+player2.sizeX-1;
			y1=player2.posY+player2.speed;
			y2=player2.posY+player2.sizeY+player2.speed-1;
		}else if(player2.dir==1){
			y1=player2.posY;
			y2=player2.posY+player2.sizeY-1;
			x1=player2.posX+player2.speed;
			x2=player2.posX+player2.sizeX+player2.speed-1;
		}else if(player2.dir==2){
			x1=player2.posX;
			x2=player2.posX+player2.sizeX-1;
			y1=player2.posY-player2.speed;
			y2=player2.posY+player2.sizeY-player2.speed-1;
		}else if(player2.dir==3){
			y1=player2.posY;
			y2=player2.posY+player2.sizeY-1;
			x1=player2.posX-player2.speed;
			x2=player2.posX+player2.sizeX-player2.speed-1;
		}
		tmpx1=player1.posX;
		tmpx2=player1.posX+player1.sizeX-1;
		tmpy1=player1.posY;
		tmpy2=player1.posY+player1.sizeY-1;
	}
	//if collision, return 1
	if(((x1<=tmpx1&&tmpx1<=x2)&&(y1<=tmpy1&&tmpy1<=y2))||((x1<=tmpx2&&tmpx2<=x2)&&(y1<=tmpy1&&tmpy1<=y2))||((x1<=tmpx2&&tmpx2<=x2)&&(y1<=tmpy2&&tmpy2<=y2))||((x1<=tmpx1&&tmpx1<=x2)&&(y1<=tmpy2&&tmpy2<=y2))){ //if collision
		return 1;
	}
	for(i=0;i<4;i++){
		if(Complayer[i].alive==0) continue;
		tmpx1=Complayer[i].posX;
		tmpx2=Complayer[i].posX+Complayer[i].sizeX-1;
		tmpy1=Complayer[i].posY;
		tmpy2=Complayer[i].posY+Complayer[i].sizeY-1;
		//if collision, return 1
		if(((x1<=tmpx1&&tmpx1<=x2)&&(y1<=tmpy1&&tmpy1<=y2))||((x1<=tmpx2&&tmpx2<=x2)&&(y1<=tmpy1&&tmpy1<=y2))||((x1<=tmpx2&&tmpx2<=x2)&&(y1<=tmpy2&&tmpy2<=y2))||((x1<=tmpx1&&tmpx1<=x2)&&(y1<=tmpy2&&tmpy2<=y2))){ //if collision
			return 1;
		}
	}
	//valid, return 0
	return 0;
}

void player1move(int dir){
	//player move accoring to the player direction
	int negative=1;
	player1.dir=dir;
	if(dir==2||dir==3) negative=-1; //if dir=2 or 3, move in negative direction 
	if(dir%2==1){	
		if(!checkvalid(player1,dir)||playerCheckCollision(1)){
			return;
		}else{
			player1.posX+=player1.speed*negative; 
		}
	}
	if(dir%2==0){
		if(!checkvalid(player1,dir)||playerCheckCollision(1)){
			return;
		}else{
			player1.posY+=player1.speed*negative;
		}
	}
}

void getPlayerMovement(){
	int i,j;
	if(live==0) gameOver=1;	//if remaining live=0, player lose
	if(player1.explosion==1&&player1explosioncooldown<50) player1.explosionState=explosionStateCounter[0]/10;
	else if(player1.explosion==1&&player1explosioncooldown>=50){	//if player exploded and cooldown ended, respawn the player
		player1.explosion=0;
		//cover the explosion effect
		for(i=player1.posX;i<player1.sizeX+player1.posX;i++){
      for(j=player1.posY;j<player1.sizeY+player1.posY;j++){
				if(map1[29-j/8][i/8]==3){
					if(bush[i%8][j%8]!=0x0000) continue;
					IERG3810_TFTLCD_DrawDot(i,j,0);
				}else IERG3810_TFTLCD_DrawDot(i,j,0);
			}
    }
		playerInit();
	}else if(player1.alive==0) return;	//if not valid, return
	else{
		//get player input to do action
		if(player1.firecooldown==1&&player1firecooldown>=playerFireCooldown) player1.firecooldown=0;
		if(keyStart) Ingame=0;
		if(keyLeft) player1move(3);
		else if(keyRight) player1move(1);
		else if(keyUp) player1move(0); 
		else if(keyDown) player1move(2);
		if(keyA||keyB){
			if(player1.shot==0&&player1.firecooldown==0&&shot[0].shot!=1){
				shotFire(player1,0);
				player1.shot=1;
			}
		}
	}	//for(i=0;i<8;i++) input[i]=0;
}

void ComMovement(){	//the movement of Complayer
	int i,j,k,dir;
	int negative=1;
	for(k=0;k<4;k++){
		//change the explosion state indicator
		if(Complayer[k].explosion==1&&Complayerexplosioncooldown[k]<50) Complayer[k].explosionState=explosionStateCounter[k+2]/10;
		//if explosion ended, cover the effect with bg color
		else if(Complayer[k].explosion==1&&Complayerexplosioncooldown[k]>=50){
			Complayer[k].explosion=0;
			
			for(i=Complayer[k].posX;i<Complayer[k].sizeX+Complayer[k].posX;i++){
        for(j=Complayer[k].posY;j<Complayer[k].sizeY+Complayer[k].posY;j++){
					if(map1[29-j/8][i/8]==3){
						if(bush[i%8][j%8]!=0x0000) continue;
						IERG3810_TFTLCD_DrawDot(i,j,0);
					}else IERG3810_TFTLCD_DrawDot(i,j,0);
				}
      }
			Complayer[k].posX=-100;
			Complayer[k].posY=-100;
		}else if(Complayer[k].alive==0) continue;	//if not valid, skip
		else{
			//if cooldown end, cooldown indicator set to 0
			if(Complayer[k].firecooldown==1&&Complayerfirecooldown[k]>=Complayer[k].cooldownTime) Complayer[k].firecooldown=0;
			if(Complayer[k].shot==0&&Complayer[k].firecooldown==0&&shot[k+2].shot!=1){	//if not cooldown, fire
				shotFire(Complayer[k],k+2);
				Complayer[k].shot=1;
			}		
			//complayer move
			//check valid
			if(Complayercheckcollisioncooldown[k]>=300&&Complayer[k].ignoreCollision==1) Complayer[k].ignoreCollision=0;
			if(Complayer[k].ignoreCollision==1&&ComplayerCheckInsideCollision(k)){
				//if the complayer is in ignore collision state and it is inside of other tank, move forward to leave other tank
				dir=Complayer[k].dir;
				if(dir==0){
					Complayer[k].posY+=Complayer[k].speed;
				}else if(dir==1){
					Complayer[k].posX+=Complayer[k].speed;
				}else if(dir==2){
					Complayer[k].posY-=Complayer[k].speed;
				}else if(dir==3){
					Complayer[k].posX-=Complayer[k].speed;
				}
				continue;
			}else if(ComplayerCheckCollision(k)||ComplayerCheckInsideCollision(k)){
					//if not valid to move forward, change direcgtion
					Complayer[k].dir=rand()%4;
			}else if(!checkvalid(Complayer[k],Complayer[k].dir)){
				//if obstacle:random dir
				Complayer[k].dir=rand()%4;
			}
			if(!checkvalid(Complayer[k],Complayer[k].dir)||ComplayerCheckCollision(k)) continue;	//if still not valid after changing direction, skip
			else{
				//move forward
				dir=Complayer[k].dir;
				if(dir==0){
					Complayer[k].posY+=Complayer[k].speed;
				}else if(dir==1){
					Complayer[k].posX+=Complayer[k].speed;
				}else if(dir==2){
					Complayer[k].posY-=Complayer[k].speed;
				}else if(dir==3){
					Complayer[k].posX-=Complayer[k].speed;
				}
			}
		}
	}
}

void printEntity(entity Entity, int type){
	u16 startX,endX,startY,endY;
	int i,j;
	int explosionState;
	
	if(Entity.explosion==1){	//if the entity is not valid anymore, print explosion effect state according to the indicator of the entity 
		if(Entity.explosionState<5){
			explosionState=Entity.explosionState;
		}
		if(Entity.dir==0){
            for(i=Entity.posX;i<Entity.sizeX+Entity.posX;i++){
                for(j=Entity.posY;j<Entity.sizeY+Entity.posY;j++){
									if(map1[29-j/8][i/8]==3){
										if(bush[i%8][j%8]!=0x0000) continue;
										IERG3810_TFTLCD_DrawDot(i,j,explosion[explosionState][i-Entity.posX][j-Entity.posY]);
									}else IERG3810_TFTLCD_DrawDot(i,j,explosion[explosionState][i-Entity.posX][j-Entity.posY]);
								}
            }
        }else if(Entity.dir==1){
            for(i=Entity.posX;i<Entity.sizeX+Entity.posX;i++){
                for(j=Entity.posY;j<Entity.sizeY+Entity.posY;j++){
									if(map1[29-j/8][i/8]==3){
										if(bush[i%8][j%8]!=0x0000) continue;
										IERG3810_TFTLCD_DrawDot(i,j,explosion[explosionState][i-Entity.posX][j-Entity.posY]);
									}else IERG3810_TFTLCD_DrawDot(i,j,explosion[explosionState][i-Entity.posX][j-Entity.posY]);
								}
            }
        }else if(Entity.dir==2){
            for(i=Entity.posX;i<Entity.sizeX+Entity.posX;i++){
               for(j=Entity.posY;j<Entity.sizeY+Entity.posY;j++){
								 if(map1[29-j/8][i/8]==3){
										if(bush[i%8][j%8]!=0x0000) continue;
										IERG3810_TFTLCD_DrawDot(i,j,explosion[explosionState][i-Entity.posX][j-Entity.posY]);
									}else IERG3810_TFTLCD_DrawDot(i,j,explosion[explosionState][i-Entity.posX][j-Entity.posY]);
							 }
            }
        }else if(Entity.dir==3){
            for(i=Entity.posX;i<Entity.sizeX+Entity.posX;i++){
                for(j=Entity.posY;j<Entity.sizeY+Entity.posY;j++){
									if(map1[29-j/8][i/8]==3){
										if(bush[i%8][j%8]!=0x0000) continue;
										IERG3810_TFTLCD_DrawDot(i,j,explosion[explosionState][i-Entity.posX][j-Entity.posY]);
									}else IERG3810_TFTLCD_DrawDot(i,j,explosion[explosionState][i-Entity.posX][j-Entity.posY]);
								}
            }
			}
				return;
	}else if(Entity.alive==0) return;
		
	if(Entity.lastposX==Entity.posX){
		//move along y
		if(Entity.lastposY<Entity.posY){
			startY=Entity.lastposY;
			endY=Entity.posY;
		}else{
			startY=Entity.posY+Entity.sizeY;
			endY=Entity.lastposY+Entity.sizeY;
		}
		startX=Entity.lastposX;
		endX=Entity.lastposX+Entity.sizeX;
	}else if(Entity.lastposY==Entity.posY){
		//move along x
		if(Entity.lastposX<Entity.posX){
			startX=Entity.lastposX;
			endX=Entity.posX;
		}else{
			startX=Entity.posX+Entity.sizeX;
			endX=Entity.lastposX+Entity.sizeX;
		}
		startY=Entity.lastposY;
		endY=Entity.lastposY+Entity.sizeY;
	}
	
		//cover the entity last position
	if(Entity.lastposY==Entity.posY){
		for(i=startX;i<endX;i++){
			for(j=startY;j<endY;j++){
				if(map1[29-j/8][i/8]==3){
					if(bush[i%8][j%8]!=0x0000) continue;
					IERG3810_TFTLCD_DrawDot(i,j,0);
				}else IERG3810_TFTLCD_DrawDot(i,j,0);
			}
		}
	}else if(Entity.lastposX==Entity.posX){
		for(i=startX;i<endX;i++){
		for(j=startY;j<endY;j++){
					if(map1[29-j/8][i/8]==3){
					if(bush[i%8][j%8]!=0x0000) continue;
					IERG3810_TFTLCD_DrawDot(i,j,0);
				}else IERG3810_TFTLCD_DrawDot(i,j,0);
			}
		}
	}	
	
		
		
		
		
		if(Entity.dir==0){
            for(i=Entity.posX;i<Entity.sizeX+Entity.posX;i++){
                for(j=Entity.posY;j<Entity.sizeY+Entity.posY;j++){
									// IERG3810_TFTLCD_DrawDot(i,j,player[i-Entity.posX][j-Entity.posY]);
									if(map1[29-j/8][i/8]==3){
										if(bush[i%8][j%8]!=0x0000) continue;
										IERG3810_TFTLCD_DrawDot(i,j,tank[type][Entity.state][i-Entity.posX][j-Entity.posY]);
									}else IERG3810_TFTLCD_DrawDot(i,j,tank[type][Entity.state][i-Entity.posX][j-Entity.posY]);
								}
            }
        }else if(Entity.dir==1){
            for(i=Entity.posX;i<Entity.sizeX+Entity.posX;i++){
                for(j=Entity.posY;j<Entity.sizeY+Entity.posY;j++){
									//IERG3810_TFTLCD_DrawDot(i,j,player[j-Entity.posY][i-Entity.posX]);
									if(map1[29-j/8][i/8]==3){
										if(bush[i%8][j%8]!=0x0000) continue;
										IERG3810_TFTLCD_DrawDot(i,j,tank[type][Entity.state][j-Entity.posY][i-Entity.posX]);
									}else IERG3810_TFTLCD_DrawDot(i,j,tank[type][Entity.state][j-Entity.posY][i-Entity.posX]);
								}
            }
        }else if(Entity.dir==2){
            for(i=Entity.posX;i<Entity.sizeX+Entity.posX;i++){
               for(j=Entity.posY;j<Entity.sizeY+Entity.posY;j++){
								 //IERG3810_TFTLCD_DrawDot(i,j,player[Entity.sizeX-i-1+Entity.posX][Entity.sizeY-j-1+Entity.posY]);
								 if(map1[29-j/8][i/8]==3){
										if(bush[i%8][j%8]!=0x0000) continue;
										IERG3810_TFTLCD_DrawDot(i,j,tank[type][Entity.state][Entity.sizeX-i-1+Entity.posX][Entity.sizeY-j-1+Entity.posY]);
									}else IERG3810_TFTLCD_DrawDot(i,j,tank[type][Entity.state][Entity.sizeX-i-1+Entity.posX][Entity.sizeY-j-1+Entity.posY]);
							 }
            }
        }else if(Entity.dir==3){
            for(i=Entity.posX;i<Entity.sizeX+Entity.posX;i++){
                for(j=Entity.posY;j<Entity.sizeY+Entity.posY;j++){
									//IERG3810_TFTLCD_DrawDot(i,j,player[Entity.sizeY-1-j+Entity.posY][Entity.sizeX-1-i+Entity.posX]);
									if(map1[29-j/8][i/8]==3){
										if(bush[i%8][j%8]!=0x0000) continue;
										IERG3810_TFTLCD_DrawDot(i,j,tank[type][Entity.state][Entity.sizeY-1-j+Entity.posY][Entity.sizeX-1-i+Entity.posX]);
									}else IERG3810_TFTLCD_DrawDot(i,j,tank[type][Entity.state][Entity.sizeY-1-j+Entity.posY][Entity.sizeX-1-i+Entity.posX]);
								}
            }
			}
}

void printbullet(entity Entity){
	u16 startX,endX,startY,endY;
	int i,j;
	if(Entity.shot==0){	//if the bullet is not valid anymore, cover it with bg color
		return;
		if(Entity.dir==0){
            for(i=Entity.posX;i<Entity.sizeX+Entity.posX;i++){
                for(j=Entity.posY;j<Entity.sizeY+Entity.posY;j++){
									if(map1[29-j/8][i/8]==3){
										if(bush[i%8][j%8]!=0x0000) continue;
										IERG3810_TFTLCD_DrawDot(i,j,0x0000);
									}else IERG3810_TFTLCD_DrawDot(i,j,0x0000);
								}
            }
        }else if(Entity.dir==1){
            for(i=Entity.posX;i<Entity.sizeX+Entity.posX;i++){
                for(j=Entity.posY;j<Entity.sizeY+Entity.posY;j++){
									if(map1[29-j/8][i/8]==3){
										if(bush[i%8][j%8]!=0x0000) continue;
										IERG3810_TFTLCD_DrawDot(i,j,0x0000);
									}else IERG3810_TFTLCD_DrawDot(i,j,0x0000);
								}
            }
        }else if(Entity.dir==2){
            for(i=Entity.posX;i<Entity.sizeX+Entity.posX;i++){
               for(j=Entity.posY;j<Entity.sizeY+Entity.posY;j++){
								 if(map1[29-j/8][i/8]==3){
										if(bush[i%8][j%8]!=0x0000) continue;
										IERG3810_TFTLCD_DrawDot(i,j,0x0000);
									}else IERG3810_TFTLCD_DrawDot(i,j,0x0000);
							 }
            }
        }else if(Entity.dir==3){
            for(i=Entity.posX;i<Entity.sizeX+Entity.posX;i++){
                for(j=Entity.posY;j<Entity.sizeY+Entity.posY;j++){
									if(map1[29-j/8][i/8]==3){
										if(bush[i%8][j%8]!=0x0000) continue;
										IERG3810_TFTLCD_DrawDot(i,j,0x0000);
									}else IERG3810_TFTLCD_DrawDot(i,j,0x0000);
								}
            }
			}
				return;
	}
	
	if(Entity.lastposX==Entity.posX){
		//move along y
		if(Entity.lastposY<Entity.posY){
			startY=Entity.lastposY;
			endY=Entity.posY;
		}else{
			startY=Entity.posY+Entity.sizeY;
			endY=Entity.lastposY+Entity.sizeY;
		}
		startX=Entity.lastposX;
		endX=Entity.lastposX+Entity.sizeX;
	}else if(Entity.lastposY==Entity.posY){
		//move along x
		if(Entity.lastposX<Entity.posX){
			startX=Entity.lastposX;
			endX=Entity.posX;
		}else{
			startX=Entity.posX+Entity.sizeX;
			endX=Entity.lastposX+Entity.sizeX;
		}
		startY=Entity.lastposY;
		endY=Entity.lastposY+Entity.sizeY;
	}
	
	//cover the bullet last position
	if(Entity.lastposY==Entity.posY){
		for(i=startX;i<endX;i++){
			for(j=startY;j<endY;j++){
				if(map1[29-j/8][i/8]==3){
					if(bush[i%8][j%8]!=0x0000) continue;
					IERG3810_TFTLCD_DrawDot(i,j,0);
				}else IERG3810_TFTLCD_DrawDot(i,j,0);
			}
		}
	}else if(Entity.lastposX==Entity.posX){
		for(i=startX;i<endX;i++){
			for(j=startY;j<endY;j++){
				if(map1[29-j/8][i/8]==3){
					if(bush[i%8][j%8]!=0x0000) continue;
					IERG3810_TFTLCD_DrawDot(i,j,0);
				}else IERG3810_TFTLCD_DrawDot(i,j,0);
			}
		}
	}
		
		
	//print the entity bullet at new position
	if(Entity.dir==0){
		for(i=Entity.posX;i<Entity.sizeX+Entity.posX;i++){
				for(j=Entity.posY;j<Entity.sizeY+Entity.posY;j++){
				if(map1[29-j/8][i/8]==3){
					if(bush[i%8][j%8]!=0x0000) continue;
					IERG3810_TFTLCD_DrawDot(i,j,cannon[i-Entity.posX][j-Entity.posY]);
				}else IERG3810_TFTLCD_DrawDot(i,j,cannon[i-Entity.posX][j-Entity.posY]);
			}
		}
	}else if(Entity.dir==1){
     for(i=Entity.posX;i<Entity.sizeX+Entity.posX;i++){
			for(j=Entity.posY;j<Entity.sizeY+Entity.posY;j++){
				if(map1[29-j/8][i/8]==3){
					if(bush[i%8][j%8]!=0x0000) continue;
					IERG3810_TFTLCD_DrawDot(i,j,cannon[j-Entity.posY][i-Entity.posX]);
				}else IERG3810_TFTLCD_DrawDot(i,j,cannon[j-Entity.posY][i-Entity.posX]);
			}
     }
   }else if(Entity.dir==2){
     for(i=Entity.posX;i<Entity.sizeX+Entity.posX;i++){
       for(j=Entity.posY;j<Entity.sizeY+Entity.posY;j++){
				if(map1[29-j/8][i/8]==3){
					if(bush[i%8][j%8]!=0x0000) continue;
					IERG3810_TFTLCD_DrawDot(i,j,cannon[Entity.sizeX-i-1+Entity.posX][Entity.sizeY-j-1+Entity.posY]);
				}else IERG3810_TFTLCD_DrawDot(i,j,cannon[Entity.sizeX-i-1+Entity.posX][Entity.sizeY-j-1+Entity.posY]);
			}
		}
	}else if(Entity.dir==3){
		for(i=Entity.posX;i<Entity.sizeX+Entity.posX;i++){
			for(j=Entity.posY;j<Entity.sizeY+Entity.posY;j++){
				if(map1[29-j/8][i/8]==3){
					if(bush[i%8][j%8]!=0x0000) continue;
					IERG3810_TFTLCD_DrawDot(i,j,cannon[Entity.sizeY-1-j+Entity.posY][Entity.sizeX-1-i+Entity.posX]);
				}else IERG3810_TFTLCD_DrawDot(i,j,cannon[Entity.sizeY-1-j+Entity.posY][Entity.sizeX-1-i+Entity.posX]);
			}
		}
	}
}	

void printStat(){	//display statistic
	int i,j;
	IERG3810_TFTLCD_FillRectangle(0x528A,30,8,274,16);
	for(i=10;i<10+16;i++){
		for(j=274;j<274+16;j++){
			if(tank[0][0][i-10][j-274]==0x0000) continue;
			IERG3810_TFTLCD_DrawDot(i,j,tank[0][0][i-10][j-274]);
		}
	}
	if(live>0) IERG3810_TFTLCD_testShowChar(30,274,live-1+48,0x0000);
	if(nextTank>=1&&nextTank<=10) IERG3810_TFTLCD_FillRectangle(0x528A,42+(nextTank-1)*20,16,274,16);
	if(nextTank>=11&&nextTank<=20) IERG3810_TFTLCD_FillRectangle(0x528A,42+(nextTank-11)*20,16,274+20,16);
}

void screenUpdate(){
	int i;
	//print bullet
	for(i=0;i<6;i++){
		printbullet(shot[i]);
		shot[i].lastposX=shot[i].posX;
		shot[i].lastposY=shot[i].posY;
	}
	//print Complayer tank
	for(i=0;i<4;i++){
		printEntity(Complayer[i],Complayer[i].type);
		Complayer[i].lastposX=Complayer[i].posX;
		Complayer[i].lastposY=Complayer[i].posY;
		Complayer[i].state=(Complayer[i].state+1)%2;
	}
	//print player
	printEntity(player1,0);
	player1.lastposX=player1.posX;
	player1.lastposY=player1.posY;
	player1.state=(player1.state+1)%2;
	
	printStat();

}



void TIM5_IRQHandler(void){	//spawn the Complayer tank
	int posx,posy;
	if(TIM5->SR&1<<0){			//if overflow
		if(comCounter==0&&nextTank==20) win=1;
		if(comCounter==maxTank){
			TIM5->SR&=~(1<<0);
			return;
		}
		if(nextTank==20){
			TIM5->SR&=~(1<<0);
			return;
		}	
		posx=mapspawn[spawnLocation]*8;
		posy=28*8;
		if(!comInit(posx,posy)){
			TIM5->SR&=~(1<<0);
			return;
		}
		
		spawnLocation++;
		if(spawnLocation==3) spawnLocation%=3;
		nextTank++;
		comCounter++;
	}
	TIM5->SR&=~(1<<0);
}

void TIM4_IRQHandler(void){	//update position and LCD display
	if(TIM4->SR&1<<0){			//if overflow
		getPlayerMovement();
		ComMovement();
		bulletMovement();
		screenUpdate();
	}
	TIM4->SR&=~(1<<0);
}

void EXTI2_IRQHandler(void){	//interrupt to end the game
	Ingame=0;
	EXTI->PR=1<<2;
}

void youWin(){
	printString(92,112,"YOU",3,0xC020);
	printString(124,112,"WIN",3,0xC020);
	Delay(50000000);
}

void youLose(){
	printString(92-4,112,"YOU",3,0xC020);
	printString(124-4,112,"LOSE",4,0xC020);
	Delay(50000000);
}

void GameStart(){	
	u8 i,j;
	gameInit();
	while(Ingame&&!gameOver&&!win){
		if(randomSeed>50000) randomSeed=0;
	}
	TIM4->CR1&=~0x01;					//disable counter
	TIM5->CR1&=~0x01;					//disable counter
	if(gameOver){
		//print gameover
		youLose();
	}else if(win){
		youWin();
	}else if(!Ingame) return;
	//print end page
	IERG3810_TFTLCD_FillRectangle(0x0000,0,240,0,320);
	printString(10,14,"Press",5,0xFFFF);
	printString(90,14,"To",2,0xFFFF);
	printString(90+24,14,"Return",6,0xFFFF);
	for(i=10+8+40;i<10+8+40+24;i++) for(j=10;j<10+24;j++) IERG3810_TFTLCD_DrawDot(i,j,buttonA[i-10-8-40][j-10]);
	while(!keyA);
}
