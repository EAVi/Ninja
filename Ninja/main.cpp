//making a random comment to to test github stuff
#ifdef _WIN32
#include <vld.h>//visual leak detector comment this out if you do not have, it's pretty good
#include <SDL.h>
#include <SDL_image.h>
/*
#include <SDL_mixer.h> //These libraries are unused
#include <SDL_ttf.h>
*/
#elif defined __APPLE__ && __MACH__ || defined Macintosh || defined macintosh || defined __linux__
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
/*
#include <SDL2/SDL_mixer.h> //These libraries are unused
#include <SDL2/SDL_ttf.h>
*/
#endif


#include <string>
#include <iostream>
#include <sstream>
#include "LTexture.h"
#include "Player.h"
#include "Block.h"
#include "Algorithms.h"
#include "Level.h"
#include "TextWriter.h"
#include "Enemy.h"
#include "MagMatrix.h"
#include "UIDrawer.h"


using namespace std;

SDL_Renderer* gRenderer = NULL;
SDL_Window* gWindow = NULL;

SDL_Color gColorKey = { 255, 0, 255 };

LTexture gNinjaTexture;
LTexture gBlockTexture;
LTexture gFontTexture;
LTexture gEnemyTexture;
LTexture gTempTexture;//completely unneeded, just here for testing
LTexture gTempTexture1;//completel unneeded, just here for testing

vector <LTexture*> gUITextures;

vector<LTexture*> gBackgroundTextures;

TextWriter gWriter;

UIDrawer gUIDrawer;

const int kScreenWidth = 256;
const int kScreenHeight = 240;

//Initializes window and renderer returns whether init was successful
bool init()
{
	//Initialize video
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		cout << "Failed to initialize!\n" << SDL_GetError() << endl;
		return false;
	}

	//Enable point texture filtering (it's best for this game)
	if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0"))
	{
		cout << "Warning: Point Texture filtering could not initialize\n";
	}

	//Initialize window
	gWindow = SDL_CreateWindow("ninja", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
		kScreenWidth, kScreenHeight, SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN_DESKTOP);
	if (gWindow == NULL)
	{
		cout << "Window could not initialize!\n" << SDL_GetError() << endl;
		return false;
	}
	//Initialize renderer
	gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
	if (gRenderer == NULL)
	{
		cout << "Renderer could not initialize!\n" << SDL_GetError() << endl;
		return false;
	}
	SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

	SDL_RenderSetLogicalSize(gRenderer, kScreenWidth, kScreenHeight);

	//Initialize loading from .png files
	int flags = IMG_INIT_PNG;
	if (!(IMG_Init(flags) & flags))
	{
		cout << "IMG_Init failure\n" << IMG_GetError() << endl;
		return false;
	}

	return true;
}

bool loadAssets()
{
	LTexture::mRenderer = gRenderer;

	//gNinjaTexture.setRenderer(gRenderer);
	if (!gNinjaTexture.loadTextureFile("GFX/AllNinja.png", &gColorKey))
	{
		cout << SDL_GetError() << endl;
		return false;
	}

	//gBlockTexture.setRenderer(gRenderer);
	if (!gBlockTexture.loadTextureFile("GFX/BlockSheet.png", &gColorKey))
	{
		cout << SDL_GetError() << endl;
		return false;
	}

	Block tempBlock;//creates a temporary block, just to set the texture
	tempBlock.setAllBlockTextures(&gBlockTexture);

	//background texture loader
	Uint8 bgTexNum = 2;
	string bgTexS[] = 
	{
		"GFX/bluesky.png",
		"GFX/snowmtn.png",
	};

	for (Uint8 i = 0; i < bgTexNum; ++i)
	{
		LTexture* tempptr = new LTexture;
		//tempptr->setRenderer(gRenderer);
		if (!tempptr->loadTextureFile(bgTexS[i], &gColorKey))
		{
			cout << SDL_GetError() << endl;
			return false;
		}
		gBackgroundTextures.push_back(tempptr);
	}

	//UI texture loader
	Uint8 uiTexNum = 3;
	string uiTexS[] =
	{
		"GFX/UI/healthbar.png",
		"GFX/UI/healthbargradient.png",
		"GFX/UI/ninjalifeicon.png",
	};
	for (Uint8 i = 0; i < uiTexNum; ++i)
	{
		LTexture* tempptr = new LTexture;
		//tempptr->setRenderer(gRenderer);
		if (!tempptr->loadTextureFile(uiTexS[i], &gColorKey))
		{
			cout << SDL_GetError() << endl;
			return false;
		}
		gUITextures.push_back(tempptr);
	}


	//gFontTexture.setRenderer(gRenderer);
	if (!gFontTexture.loadTextureFile("GFX/font.png", &gColorKey))
	{
		cout << SDL_GetError() << endl;
		return false;
	}
	gWriter = TextWriter(&gFontTexture, 6, 8);

	//Set the default enemy texture
	Enemy tempEnemy;
	if (!gEnemyTexture.loadTextureFile("GFX/AllGhost.png", &gColorKey))
	{
		cout << SDL_GetError() << endl;
		return false;
	}
	tempEnemy.setTexture(&gEnemyTexture);



	return true;
}

void exit()
{
	gNinjaTexture.freeTexture();
	gBlockTexture.freeTexture();
	gFontTexture.freeTexture();
	gTempTexture.freeTexture();
	gTempTexture1.freeTexture();

	//deallocate the background textures
	for (Uint8 i = 0, j = gBackgroundTextures.size(); i < j; ++i)
	{
		gBackgroundTextures[i]->freeTexture();
		delete gBackgroundTextures[i];//'new' memory was allocated for these textures
	}

	//rinse and repeat for the UI textures
	for (Uint8 i = 0, j = gUITextures.size(); i < j; ++i)
	{
		gUITextures[i]->freeTexture();
		delete gUITextures[i];//'new' memory was allocated for these textures
	}

	SDL_DestroyWindow(gWindow);
	gWindow = NULL;

	SDL_DestroyRenderer(gRenderer);
	gRenderer = NULL;
	
	SDL_Quit();
	IMG_Quit();
}

int main(int argc, char* args[])
{

	if (!(init() && loadAssets()))
	{
		exit();
		return 0;
	}
	bool quit = false;
	bool ghostalive = true;//just a temporary variable for testing, will probably stick around for a while
	Player ninja;
	ninja.setTexture(&gNinjaTexture);
	gFontTexture.colorMod(0xFF, 0, 0);
	SDL_Rect camera = { 0, 0, kScreenWidth, kScreenHeight };
	SDL_Color red = { 0xFF, 0, 0, 0xFF };

	//toggle-able debug options
	bool debugtoggle = false;

	//build the debug level
	Level debugLevel(1366,768,&camera);
	debugLevel.setBGTextures(gBackgroundTextures);
	debugLevel.Loadmap("data/debug.txt");

	
	SDL_Event mainevent;
	int health = 5;
	int maxhealth = 20;

	ninja.setLevel(&debugLevel);

	while (!quit)
	{
		debugLevel.clearHitboxes();//need to put before the event handler because the events make hitboxes
		while (SDL_PollEvent(&mainevent))
		{
			if (mainevent.type == SDL_QUIT)//alt-f4 or clicking x on window
				quit = true;
			if (mainevent.key.keysym.sym == SDLK_BACKQUOTE && mainevent.type == SDL_KEYDOWN && mainevent.key.repeat == 0)//press tilde key
				debugtoggle = !debugtoggle;
			if (mainevent.key.keysym.sym == SDLK_ESCAPE && mainevent.type == SDL_KEYDOWN && debugtoggle)
				debugLevel.addEnemy(0, 55, 6);

			ninja.handleEvent(mainevent);

		}
		SDL_SetRenderDrawColor(gRenderer, 0,0,0,0xFF);
		SDL_RenderClear(gRenderer);

		//ninja.move(debugLevel.getRects());
		ninja.step();
		debugLevel.stepEnemies();
		ninja.endstep();

		camera.x = ninja.getX() - kScreenWidth/2 + ninja.kClipWidth/2;
		camera.y = ninja.getY() - kScreenHeight/2 + ninja.kClipHeight/2;
		debugLevel.step();//bounds camera and renders

		ninja.render(camera.x, camera.y);
		debugLevel.drawEnemies();
		gUIDrawer.drawHealthbar(ninja.getHealth(), ninja.getMaxHealth(), health);

		if (debugtoggle)//the debug stuff, shows some stats, and renders hurtboxes
		{
			gWriter.RenderString(ninja.getX(), 8, 32);
			gWriter.RenderString(ninja.getY(), 40, 32);
			gWriter.RenderString(debugLevel.enemyCount(), 8, 40);

			debugLevel.debugShowHitboxes(*gRenderer);
		}

		SDL_RenderPresent(gRenderer);
	}
	exit();
	return 0;
}