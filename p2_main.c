#include <lpc17xx.h>
#include <stdlib.h>
#include <stdio.h>
#include "GLCD.h"
#include "spece.h"
#include <cmsis_os2.h>
#include <os_tick.h>
#include "gameLogic.h"


//Yes! Globals!
actor* enemy;
actor* player;
actor* lasers[2]; //an array of actor pointers!

/*
	we need to be able to stop the animation thread once one of the actors dies, so 
	we are keeping its ID global so that other threads checking on it can stop it
*/
osThreadId_t animateID;

int main()
{
	initializeActors();

	SystemInit();
	
	/*
		Because of a quirk of Keil's multithreading, we need to use printf outside of a thread
		before we ever use it inside of one. On the assumption that you want to print
		debugging information this test string gets printed. You may now instrument
		the threads with printf statements if you wish
	*/
	printf("Test String");

	/*
		Initialize the LCD screen, clear it so that it is mostly black, then
		set the "text" color to green. In reality the "text" color should be called "pixel" or "foreground"
		color, and it used to be. In a recent update from Keil the name was changed. Long story short,
		by setting the text color to green, every pixel we print is also going to be green
	*/
	GLCD_Init();
	GLCD_Clear(Black);
	GLCD_SetTextColor(Green); //actually sets the color of any pixels we directly write to the screen, not just text

	/*
		For efficiency reasons, the player character only gets re-drawn if it has moved, so
		we need to print it before the game starts or we won't see it
	*/
	printPlayer(player);

	//initialize the kernel so that we can create threads
	osKernelInitialize();
	
	//create a new thread for animate and store its ID so that we can shut it down at the endgame
	animateID = osThreadNew(animate,NULL,NULL);
	
	//create a new thread for reading player input
	osThreadNew(readPlayerInput,NULL, NULL);

	//create a new thread that checks for endgame
	osThreadNew(checkEndGame,NULL,NULL);

	//launch the kernel, which simultaneously starts all threads we have created
	osKernelStart();
	while(1){};
	

}
