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
#include "Demon.h"
#include "Enemy.h"
#include "Robot.h"
#include "MagMatrix.h"
#include "UIDrawer.h"
#include "Timer.h"
#include "Zone.h"
#include "LAudio.h"
#include "Cutscene.h"
#include "Menu.h"

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

	//sets the prefix and loads the zone of given number
	void prepareZone(Uint8 a);

	void introSequence();

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
	const int kFinalZone = 1;

	SDL_Renderer* mRenderer;
	SDL_Window* mWindow;
	SDL_GameController *mController;
	SDL_Color mColorKey;//the magenta color that will represent transparency
	LTexture mNinjaTexture;
	LTexture mBlockTexture;
	LTexture mFontTexture;
	LTexture mLargeFontTexture;
	LTexture mEnemyTexture;
	LTexture mRobotTexture;
	LTexture mDemonTexture;
	UIDrawer mUIDrawer;//draws healthbar, lives, and other UI
	std::vector<LTexture*> mBackgroundTextures;
	std::vector<LTexture*> mDoorTextures;
	Zone mZone;
	Player mPlayer;
	SDL_Rect mCamera;
	int mRefreshRate;//detected refresh rate of the monitor
	SDL_Point mScreen;//screen dimensions, x and y -> w and h
	SDL_Event mEvent;//events to be passed to the even handler
	Timer mTimer;//calculates framerate and helps game run at 60 fps
	LAudio mSoundBox;//loads and plays sound and music
	std::vector<Cutscene> mCutscene;//draw the cutscenes when needed
	Uint8 mCurrentCutScene;//the current index of the cutscene
	Uint8 mCurrentMenu;//the menutype that the game is currently at
	std::vector<Menu> mMenu;//all posible menus

	//gets monitor refresh rate and screen dimensions
	void screenAttrs();

	//polling and event handling
	void handleEvents();

	//handle quit, fullscreen toggle, etc.
	void handleGeneralEvents();

	//the begin step for the zone and the player
	void beginstep();

	//the end step for the zone and the player
	void endstep();

	//draws and renders the screen
	void render();

	//the debug options
	void debugOptions();

	//initializes just one controller if possible
	void mInitControllers();

	//people usually do this after they play vs me in smash bros
	void mDestroyControllers();

	//toggles fullscreendesktop
	void mToggleFullScreen();

	//function that sets the slides of a cutscene
	void mSetCutscene();

	//loop which handles cutscenes
	void mCutSceneLoop();

	//sets up the menus
	void mSetMenu();

	//loop which handles menus
	void mMenuLoop();

	//if a button is pressed in a menu, handles the option
	void mButtonOptionHandler(ButtonOption & a);

	//sets the mCurrentCutScene according to current level if there is any
	void mGetCutSceneCurrentLevel();

};