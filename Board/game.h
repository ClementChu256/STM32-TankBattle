#include "stm32f10x.h"
#define MAXTANK 4;


typedef struct{ 				//structure for different entity
	u16 posX;							//current X position of entity (bottom left corner)
	u16 posY;							//current Y position of entity
	u16 lastposX;					//last X position of entity
	u16 lastposY;					//last Y position of entity
	u8 dir;								//direction of the entity
	u8 health;						//remain lives of this entity
	u8 speed;							//the moving speed of this entity
	u8 sizeX;							//the X size of entity
	u8 sizeY;							//the Y size of entity
	u8 shot;							//for tanks: indicate the tank shooing status, for bullet: indicate the bullet vallid
	u8 alive;							//for tank: indicate the tank alive
	u8 firecooldown;			//for tank: indicate it is in a firecooldown
	u8 type;							//for tank: indicate the type of the tank
	u8 explosion;					//for tank: indicate the tank is exploded
	u8 cooldownTime;			//for Complayer tank: store the random cooldowntime to wait before next fire
	u8 ignoreCollision;		//for tank: indicate it is in a spawnIgnoreCollision status
	u8 state;							//for tank: indicate printing which state of the tank
	u8 explosionState;		//for tank: indicate printing which state of explosion effect
}entity;

extern int Ingame;			//global variable indicate ingame 

extern int map;					//global variable indicate map chosen

extern int gameOver;		//global variable indacate the player lose

extern int live;				//global variable indicate the player remain live

extern int nextTank;		//global variable indicate the next Complayer tank to be spawned

extern int comCounter;	//global variable indicate the number of Complayer on the arena

extern int spawnLocation;		//global variable indicate the Complayer spawn location

extern int maxTank;			//global variable indicate the maximum Complayer tank allowed on the arena

extern int win;					//global variable indicate the player win
