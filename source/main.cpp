#include "AIE.h"
#include <iostream>

int main( int argc, char* argv[] )
{	
	//constant vars
	static int SCREEN_MAX_X = 1200, SCREEN_MAX_Y = 700;

	Initialise(SCREEN_MAX_X, SCREEN_MAX_Y, false, "My Awesome Game");

    //Game Loop
    do
	{

        ClearScreen();

    } while(!FrameworkUpdate());

    Shutdown();

    return 0;
}
