#ifndef SPECE
#define SPECE

#include "GLCD.h"
#include <stdbool.h>
#include <math.h>
#include <stdlib.h>

//sprites are 6x8 bitmaps, scaled by SPRITE_SCALE
#define SPRITE_ROWS 6
#define SPRITE_COLS 8
#define SPRITE_SCALE 3

//size of the screen
#define VERTICAL_LIM 320
#define HORIZONTAL_LIM 240

//speeds
#define LASER_SPEED SPRITE_ROWS*SPRITE_SCALE

//randomly selecting when the enemy can shoot
#define ENEMY_SHOT_CHANCE 25000



typedef struct actor_struct{
	int verticalPosition;
	int horizontalPosition;
	char* sprite;
	int dir; //direction of motion, used for enemies but not for player
	int speed; //number of pixels to move each time
}actor;

//Prints a sprite starting at that sprite's top-left corner
void printSpriteAt(int x, int y,char* spriteBMP);

/*
	draws a scale x scale square of pixels. This function allows us
	to specify sprites as 6x8 bitmaps but scale them as much as we want, 
	which saves memory at the expense of CPU cycles
*/
void drawBigPixelAt(int x, int y, int scale);

//updates x and y position of the enemy
void updateEnemyPosition(actor* act);

//prints an enemy
void printEnemy(actor* enemy);

//prints the player
void printPlayer(actor* play);

//prints laser beams...pew pew
void printLaser(actor* beam);

//determines if any part of act1 overlaps with act2
bool checkCollision(actor* act1, actor* act2,bool isLaser);

#endif
