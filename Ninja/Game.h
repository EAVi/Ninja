#ifdef _WIN32
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h> 
/*
#include <SDL_ttf.h>
*/
#elif defined __APPLE__ && __MACH__ || defined Macintosh || defined macintosh || defined __linux__
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
/*
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
#include "LAudio.h"

class Game
{
public:
	Game();
	
	//Initializes renderer and window
	bool init();

	//Loads textures and other assets
	bool loadAssets();

	//initializes the mZone
	void prepare();

	//Init and load assets and other preparations
	bool fullInit();

	//deallocate assets, window and renderer
	void destroyAssets();

	//advance one frame of the game loop
	void gameLoop();

	//modifies the refresh rate
	void setRefreshRate(int rr);

	//returns if a quit event has happened
	bool getQuit();

	const int kScreenWidth = 256;
	const int kScreenHeight = 240;
private:
	bool mSub60;//if monitor refresh rate is below 60
	bool mSuper60;//if monitor refresh rate is above 60
	bool mQuit;
	bool mDebug;
	bool mOddFrame;//keeps of odd frames for 30-ish Hz monitors.
	const int kFramePeriod = 1000 / 60; // 16.667 ms between  each frame

	SDL_Renderer* mRenderer;
	SDL_Window* mWindow;
	SDL_Color mColorKey;
	LTexture mNinjaTexture;
	LTexture mBlockTexture;
	LTexture mFontTexture;
	LTexture mEnemyTexture;
	LTexture mRobotTexture;
	UIDrawer mUIDrawer;
	std::vector<LTexture*> mBackgroundTextures;
	Zone mZone;
	Player mPlayer;
	SDL_Rect mCamera;
	int mRefreshRate;
	SDL_Point mScreen;
	SDL_Event mEvent;
	Timer mTimer;
	LAudio mSoundBox;

	//gets monitor refresh rate and screen dimensions
	void screenAttrs();

	//polling and event handling
	void handleEvents();

	//the begin step for the zone and the player
	void beginstep();

	//the end step for the zone and the player
	void endstep();

	//draws and renders the screen
	void render();

	//the debug options
	void debugOptions();

};