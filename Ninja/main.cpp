#ifdef _WIN32
#include <vld.h>//visual leak detector comment this out if you do not have, it's pretty good
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h> 
/*
//These libraries are unused
#include <SDL_ttf.h>
*/
#elif defined __APPLE__ && __MACH__ || defined Macintosh || defined macintosh || defined __linux__
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h> 
/*
//These libraries are unused
#include <SDL2/SDL_ttf.h>
*/
#endif

#include <string>
#include <iostream>
#include <sstream>
#include "LTexture.h"
#include "Player.h"
#include "Algorithms.h"
#include "Level.h"
#include "TextWriter.h"
#include "Enemy.h"
#include "Robot.h"
#include "MagMatrix.h"
#include "UIDrawer.h"
#include "Timer.h"
#include "Zone.h"
#include "Game.h"
#include "LAudio.h"

using namespace std;

TextWriter gWriter;
const int kScreenWidth = 256;
const int kScreenHeight = 240;

int main(int argc, char* args[])
{
	Game theGame;

	if (!theGame.fullInit())//failure to initialize
	{
		theGame.destroyAssets();
#ifdef _WIN32
		system("pause");
		//so you can see the error if it fails on windows
#endif
		return 1;
	}

#ifdef NDEBUG
	theGame.introSequence();
#endif

	while (!theGame.getQuit())
	{
		theGame.gameLoop();
	}
	theGame.destroyAssets();
	return 0;
}
