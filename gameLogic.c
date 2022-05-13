#include "gameLogic.h"

/*
	These functions rely on the existence of global variables in order to simplify
	the threading code for lab 1. They are declared in main, so must be listed as extern here
*/
extern actor* player;
extern actor* enemy;
extern actor* lasers[2];
extern osThreadId_t animateID;

//sprites for the enemy, player, and laser bolt
char sprite[] = {0x81,0xA5,0xFF,0xE7,0x7E,0x24};
char playerSprite[] = {0x00,0x18,0x3C, 0xFF, 0xFF, 0xFF};
char laserSprite[] = {0x18,0x18,0x18,0x18,0x18,0x18};

void readPlayerInput(void *args)
{
	while(1)
	{
		//check the joystick for movement
		if(!(LPC_GPIO1->FIOPIN & (1<<23))) //0 means ON, 1 means OFF
		{
			player->dir = -1;
		}
		else if(!(LPC_GPIO1->FIOPIN & (1<<25)))
		{
			player->dir = 1;
		}
		else
			player->dir = 0;
		
		//check the button for laser shooting
		if(!(LPC_GPIO2->FIOPIN & (1<<10)))
		{
			if(lasers[0]->dir == 0)
			{
				//put it in the middle
				lasers[0]->horizontalPosition = player->horizontalPosition + (SPRITE_ROWS-2)*SPRITE_SCALE/2;
				lasers[0]->verticalPosition = player->verticalPosition + (SPRITE_COLS-1)*SPRITE_SCALE;
				lasers[0]->dir = 1;
			}
		}
		osThreadYield();
	}
}

void enemyShoot(actor* enemy)
{
	//put it in the middle
	lasers[1]->horizontalPosition = enemy->horizontalPosition + (SPRITE_ROWS-2)*SPRITE_SCALE/2;
	lasers[1]->verticalPosition = enemy->verticalPosition - (SPRITE_COLS-1)*SPRITE_SCALE;
	lasers[1]->dir = -1;
}

//definitely screw this up with both being task 1 or something
void animate(void *args)
{
	while(1)
	{
		printEnemy(enemy);
		
		//see if there is shooting to do
		if(rand() > ENEMY_SHOT_CHANCE && lasers[1]->dir == 0)
			enemyShoot(enemy);
		
		if(player->dir != 0) //we don't want to print more than we need. The print logic is a bit more complex than for enemies
			printPlayer(player);
		
		if(lasers[0]->dir != 0)
			printLaser(lasers[0]);
		
		if(lasers[1]->dir != 0)
			printLaser(lasers[1]);
	
		osDelay(100U);
	}
}

//thread to check for end-game conditions
void checkEndGame(void* args)
{
	while(1)
	{
		//endgame in three ways: enemy hits player, bullet hits player (both lose), bullet hits enemy (win!)
		if(checkCollision(enemy, player,0))
			osThreadTerminate(animateID);
		else if(checkCollision(enemy,lasers[0],1) && lasers[0]->dir != 0)
			osThreadTerminate(animateID);
		else if(checkCollision(player,lasers[1],1) && lasers[1]->dir != 0)
			osThreadTerminate(animateID);
	
		osThreadYield();
	}
}

void initializeActors()
{
	/*
		Generally this should be handled by a separate function. However, it made sense
		for simplicity of understanding the code that the first thing we do is initialize the 
		actors, so we did this in main instead.
	*/
	enemy = malloc(sizeof(actor));
	enemy->horizontalPosition = 50;
	enemy->verticalPosition = 300;
	enemy->speed = 4;
	enemy->dir = 1;
	enemy->sprite = sprite;
	
	player = malloc(sizeof(actor));
	player->horizontalPosition = 10;
	player->verticalPosition = 20;
	player->speed = 4;
	player->dir = 1;
	player->sprite = playerSprite;
	
	lasers[0] = malloc(sizeof(actor));
	lasers[0]->horizontalPosition = 0;
	lasers[0]->verticalPosition = 0;
	lasers[0]->speed = LASER_SPEED;
	lasers[0]->dir = 0;
	lasers[0]->sprite = laserSprite;
	
	lasers[1] = malloc(sizeof(actor));
	lasers[1]->horizontalPosition = 0;
	lasers[1]->verticalPosition = 0;
	lasers[1]->speed = LASER_SPEED;
	lasers[1]->dir = 0;
	lasers[1]->sprite = laserSprite;
}
