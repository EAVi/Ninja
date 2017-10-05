#include "Game.h"

using namespace std;

Game::Game()
{
	mRenderer = NULL;
	mWindow = NULL;
	mController = NULL;
	mColorKey = { 255, 0, 255 };
	mQuit = false;
	mCamera = { 0, 0, kScreenWidth, kScreenHeight };
	mDebug = false;
	mTimer.setFrameDelay(kFramePeriod);
	mCurrentMenu = kCutscene;	
	mMenu = vector<Menu>(kTotalMenus);
}

bool Game::init()
{
	
	//Initialize video
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) < 0)
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
#ifdef NDEBUG
	mWindow = SDL_CreateWindow("ninja", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		kScreenWidth, kScreenHeight, SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN_DESKTOP);
#else
	mWindow = SDL_CreateWindow("ninja", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		kScreenWidth, kScreenHeight, SDL_WINDOW_SHOWN);
#endif

	if (mWindow == NULL)
	{
		cout << "Window could not initialize!\n" << SDL_GetError() << endl;
		return false;
	}
	//Initialize renderer
	mRenderer = SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
	if (mRenderer == NULL)
	{
		cout << "Renderer could not initialize!\n" << SDL_GetError() << endl;
		return false;
	}
	SDL_SetRenderDrawColor(mRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

	SDL_RenderSetLogicalSize(mRenderer, kScreenWidth, kScreenHeight);

	//Initialize loading from .png files
	int flags = IMG_INIT_PNG;
	if (!(IMG_Init(flags) & flags))
	{
		cout << "IMG_Init failure\n" << IMG_GetError() << endl;
		return false;
	}

	//Initialize SDL_Mixer
	//flags = MIX_INIT_;
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) == -1)
	{
		cout << "Error opening SDL_Mixer!\nError # " << Mix_GetError();
		return false;
	}

	//Tries allocating 16 channels for mixing, but will accept less (A mute game still functions-ish)
	Uint8 numChannels = (Uint8)(Mix_AllocateChannels((int)LAudio::mChannels));
	if (numChannels < 3)
		cout << "Warning: Could only allocate " << numChannels <<  " channels!" << endl;
	else if (numChannels < LAudio::mChannels)
		cout << "Warning: Less mixer channels than optimal: " << numChannels << endl;
	LAudio::mChannels = numChannels;

	//controller mapping courtesy of https://github.com/gabomdq/SDL_GameControllerDB
	SDL_GameControllerAddMappingsFromFile("gamecontrollerdb.txt");
	mInitControllers();

	return true;
}

bool Game::loadAssets()
{
	LTexture::mRenderer = mRenderer;

	//gNinjaTexture.setRenderer(gRenderer);
	if (!mNinjaTexture.loadTextureFile("GFX/AllNinja.png", &mColorKey))
	{
		cout << SDL_GetError() << endl;
		return false;
	}
	mPlayer.setTexture(&mNinjaTexture);

	//gBlockTexture.setRenderer(gRenderer);
	if (!mBlockTexture.loadTextureFile("GFX/BlockSheet.png", &mColorKey))
	{
		cout << SDL_GetError() << endl;
		return false;
	}

	Level tempLevel;//creates a temporary level, just to set the static texture
	tempLevel.setBlockTextures(&mBlockTexture);

	//background texture loader
	Uint8 bgTexNum = 14;
	string bgTexS[] =
	{
		"GFX/BG/bluesky.png",
		"GFX/BG/snowmtn.png",
		"GFX/BG/grasshills.png",
		"GFX/BG/nightynight.png",
		"GFX/BG/skyline.png",
		"GFX/BG/redsky.png",
		"GFX/BG/snow.png",
		"GFX/BG/castlehill.png",
		"GFX/BG/castlewall.png",
		"GFX/BG/stormclouds.png",
		"GFX/BG/moonskull.png",
		"GFX/BG/castleinterior.png",
		"GFX/BG/beachhill.png",
		"GFX/BG/seasky.png",

	};

	for (Uint8 i = 0; i < bgTexNum; ++i)
	{
		LTexture* tempptr = new LTexture;
		//tempptr->setRenderer(gRenderer);
		if (!tempptr->loadTextureFile(bgTexS[i], &mColorKey))
		{
			cout << SDL_GetError() << endl;
			return false;
		}
		mBackgroundTextures.push_back(tempptr);
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
		if (!tempptr->loadTextureFile(uiTexS[i], &mColorKey))
		{
			cout << SDL_GetError() << endl;
			return false;
		}
		UIDrawer::gUITextures.push_back(tempptr);
	}

	//UI texture loader
	Uint8 cutTexNum = 4;
	string cutTexS[] =
	{
		"GFX/CUT/ninjaclose.png",
		"GFX/CUT/ninjaforward.png",
		"GFX/CUT/karl.png",
		"GFX/CUT/goldfish.png",
	};
	for (Uint8 i = 0; i < cutTexNum; ++i)
	{
		LTexture* tempptr = new LTexture;
		//tempptr->setRenderer(gRenderer);
		if (!tempptr->loadTextureFile(cutTexS[i], &mColorKey))
		{
			cout << SDL_GetError() << endl;
			return false;
		}
		Cutscene::mSlideTextures.push_back(tempptr);
	}


	//gFontTexture.setRenderer(gRenderer);
	if (!mFontTexture.loadTextureFile("GFX/font.png", &mColorKey))
	{
		cout << SDL_GetError() << endl;
		return false;
	}
	if (!mLargeFontTexture.loadTextureFile("GFX/largefont.png", &mColorKey))
	{
		cout << SDL_GetError() << endl;
		return false;
	}
	gWriter = TextWriter(&mFontTexture, 6, 8, &mLargeFontTexture, 16, 24);

	//Set the default enemy texture
	Enemy tempEnemy;
	if (!mEnemyTexture.loadTextureFile("GFX/AllGhost.png", &mColorKey))
	{
		cout << SDL_GetError() << endl;
		return false;
	}
	tempEnemy.setTexture(&mEnemyTexture);

	Robot tempRobot;
	if (!mRobotTexture.loadTextureFile("GFX/AllRoboPirate.png", &mColorKey))
	{
		cout << SDL_GetError() << endl;
		return false;
	}
	tempRobot.setTexture(&mRobotTexture);

	//Load all the sounds and music
	//Music
	mSoundBox.loadSFX("SFX/MUS/00.wav", true);
	mSoundBox.loadSFX("SFX/MUS/01.wav", true);
	mSoundBox.loadSFX("SFX/MUS/02.wav", true);
	mSoundBox.loadSFX("SFX/MUS/03.wav", true);
	Mix_VolumeMusic(MIX_MAX_VOLUME / 4);
	//Sounds
	mSoundBox.loadSFX("SFX/00.wav");
	mSoundBox.loadSFX("SFX/01.wav");
	mSoundBox.loadSFX("SFX/02.wav");
	Mix_Volume(-1, MIX_MAX_VOLUME / 4);

	return true;
}

void Game::prepare()
{
	//load zone
	mZone.setPrefix("data/debug");
	mZone.setSuffix(".txt");
	mZone.setCamera(&mCamera);
	mZone.setPlayer(&mPlayer);
	mZone.setBGTextures(mBackgroundTextures);
	//(&mCamera, &mPlayer, "data/debug", ".txt", mBackgroundTextures);
	mZone.init();
	mZone.setSpawn();
	screenAttrs();
	mSetCutscene();
	mSetMenu();
}

bool Game::fullInit()
{
	if (init() && loadAssets())
	{
		prepare();
		mQuit = false;
		return true;
	}
	else
	{
		mQuit = true;
		return false;
	}
}

void Game::destroyAssets()
{
	mZone.release();
	mNinjaTexture.freeTexture();
	mEnemyTexture.freeTexture();
	mRobotTexture.freeTexture();
	mBlockTexture.freeTexture();
	mFontTexture.freeTexture();
	mLargeFontTexture.freeTexture();

	//deallocate the background textures
	for (Uint8 i = 0, j = mBackgroundTextures.size(); i < j; ++i)
	{
		mBackgroundTextures[i]->freeTexture();
		delete mBackgroundTextures[i];//'new' memory was allocated for these textures
	}

	//rinse and repeat for the UI textures
	for (Uint8 i = 0, j = UIDrawer::gUITextures.size(); i < j; ++i)
	{
		UIDrawer::gUITextures[i]->freeTexture();
		delete UIDrawer::gUITextures[i];//'new' memory was allocated for these textures
	}

	//cutscene textures
	for (Uint8 i = 0, j = Cutscene::mSlideTextures.size(); i < j; ++i)
	{
		Cutscene::mSlideTextures[i]->freeTexture();
		delete Cutscene::mSlideTextures[i];//'new' memory was allocated for these textures
	}

	//close the controller
	mDestroyControllers();


	SDL_DestroyWindow(mWindow);
	mWindow = NULL;

	SDL_DestroyRenderer(mRenderer);
	mRenderer = NULL;

	mSoundBox.release();
	SDL_Quit();
	IMG_Quit();
	Mix_CloseAudio();
	Mix_Quit();
}

void Game::gameLoop()
{
	if (mCurrentMenu == kCutscene)
	{
		mCutSceneLoop();
	}
	else if (mCurrentMenu == kInGame)
	{
		mZone.clearHitboxesCurrentLevel();
		handleEvents();
		beginstep();
		endstep();
		render();
	}
	else
	{
		mMenuLoop();//else, it's a menu object we need to take care of
	}
}

void Game::setRefreshRate(int rr)
{
	if (rr == 0)//if undefined, assume 60
		rr = 60;

	mRefreshRate = rr;

	if (rr == 60)
	{
		mSub60 = false;
		mSuper60 = false;
	}
	else if (rr > 60)
	{
		mSub60 = false;
		mSuper60 = true;
	}
	else //below 60
	{
		mSub60 = true;
		mSuper60 = false;
	}
}

bool Game::getQuit()
{
	return mQuit;
}

void Game::screenAttrs()
{
	SDL_DisplayMode monitor;
	SDL_GetDisplayMode(0, 0, &monitor);
	setRefreshRate(monitor.refresh_rate);
	mScreen.x = monitor.w;
	mScreen.y = monitor.h;
}

void Game::handleEvents()
{
	while (SDL_PollEvent(&mEvent))
	{
		handleGeneralEvents();
		mPlayer.handleEvent(mEvent);
	}
}

void Game::handleGeneralEvents()
{
	if (mEvent.type == SDL_QUIT)//alt-f4 or clicking x on window
		mQuit = true;

	if (mEvent.type == SDL_CONTROLLERBUTTONDOWN && mEvent.cbutton.button == SDL_CONTROLLER_BUTTON_BACK //back button on controller
		|| mEvent.key.keysym.sym == SDLK_BACKQUOTE && mEvent.type == SDL_KEYDOWN && mEvent.key.repeat == 0)//or press tilde key
		mDebug = !mDebug;//toggle debug

	if (mEvent.key.keysym.sym == SDLK_ESCAPE && mEvent.type == SDL_KEYDOWN && mEvent.key.repeat == 0)//on keypress ESC
	{
		if (mZone.getCurrentLevel() == (mZone.getLevelSize() - 1))//skip level, and if at max level, set to 0
			mZone.setLevel(0);
		else mZone.setLevel(mZone.getCurrentLevel() + 1); //skip level
	}

	if (mEvent.key.keysym.sym == SDLK_RETURN && mEvent.type == SDL_KEYDOWN && mEvent.key.repeat == 0 && mEvent.key.keysym.mod & (KMOD_RALT | KMOD_LALT))// on alt+enter
		mToggleFullScreen();

	//Controller Disconnecting
	if (mEvent.type == SDL_CONTROLLERDEVICEREMOVED)
	{
		mDestroyControllers();
	}
	//Controller Connecting
	if (mEvent.type == SDL_CONTROLLERDEVICEADDED)
	{
		mInitControllers();
	}
}

void Game::beginstep()
{
	SDL_SetRenderDrawColor(mRenderer, 0, 0, 0, 0xFF);
	SDL_RenderClear(mRenderer);
	if (mPlayer.checkDead())
	{
		if (mPlayer.getLives() != 0)
		{
		mPlayer.getLives()--;
		mPlayer.getHealth() = mPlayer.getMaxHealth();
		}
		else
		{
			mCurrentMenu = kGameOver;
		}
	}
	mPlayer.step();
	mZone.stepEnemiesCurrentLevel();
	
	mSoundBox.playMusic(mZone.getSongCurrentLevel());
	//since mCheckDoors is called in stepEnemies...
}

void Game::endstep()
{
	mPlayer.endstep();
	mCamera.x = mPlayer.getX() - kScreenWidth / 2 + mPlayer.kClipWidth / 2;
	mCamera.y = mPlayer.getY() - kScreenHeight / 2 + mPlayer.kClipHeight / 2;
}


void Game::render()
{
	if (mSub60 && mOddFrame //odd frame of a 30Hz monitor 
		|| !mSub60)//or regular frame of a 60Hz(or more) monitor
	{
		mZone.renderCurrentLevel();
		mPlayer.render(mCamera.x, mCamera.y);
		mZone.drawEnemiesCurrentLevel();
		mUIDrawer.drawHealthbar(mPlayer.getHealth(), mPlayer.getMaxHealth(), mPlayer.getLives());
		debugOptions();
		gWriter.ClearBuffer();
		mTimer.vtick();
		if (mSuper60)
		{
			mTimer.delayRender();
		}

		SDL_RenderPresent(mRenderer);
		mTimer.tick();
	}
}

void Game::debugOptions()
{
	if (mDebug)//the debug stuff, shows some stats, and renders hurtboxes
	{

		gWriter(textbuffers::Debug) << txt::TypeOn << txt::White  << "Ninja pos: (" << mPlayer.getX() << ',' << txt::Yellow << mPlayer.getY() << txt::White << ")\n";
		gWriter << txt::SuperOn << txt::Rainbow  << "Enemy Count: " << (int)mZone.enemyCountCurrentLevel() << txt::SuperOff << '\n';
		gWriter << txt::Green << mTimer.getFramerate() << " FPS\n";
		gWriter << txt::White << mTimer.getVSyncFramerate() << " FPS - VS\n" << txt::TypeOff;
		//if (rr != 0)
		//{
		gWriter << txt::HLightOn << txt::Blue << mRefreshRate << "Hz Monitor with dimensions " << mScreen.x << 'x' << mScreen.y << txt::HLightOff << '\n';
		//}
		//gWriter << '\x88' << "Frame #"<< mClock.getFrameCount() << '\n';

		gWriter(textbuffers::Large, 16 , 32) << txt::TypeOn << txt::Blue << txt::HLightOn << "(" << mPlayer.getX() << ',' << txt::Yellow << mPlayer.getY() << txt::White << ")" << txt::HLightOff;

		mZone.debugShowHitboxesCurrentLevel(*mRenderer);
	}
}

void Game::mInitControllers()
{
	//if a controller is already connected, no need to init
	if (mController != NULL) return;

	for (int i = 0, j = SDL_NumJoysticks(); i < j; ++i)
	//goes through all currently plugged in joysticks
	{
		if (SDL_IsGameController(i))
		//if the joystick is a game controller, then initialize it as the controller
		{
			mController = SDL_GameControllerOpen(i);//sets the controller
			return;//return to prevent many controllers from initializing, since only one will be used
		}
	}
}

void Game::mDestroyControllers()
{
	if (mController != NULL)
		SDL_GameControllerClose(mController);
	mController = NULL;
}

void Game::mToggleFullScreen()
{
	if (SDL_GetWindowFlags(mWindow) & SDL_WINDOW_FULLSCREEN_DESKTOP)
		//if already fullscreen, turn it off
		SDL_SetWindowFullscreen(mWindow, 0);
	else //else turn it on
		SDL_SetWindowFullscreen(mWindow, SDL_WINDOW_FULLSCREEN_DESKTOP);
}

void Game::mSetCutscene()
{
	mCutscene.clearCutscene();
	mCutscene.addSlide(2, (string)"\x89\x8C\x86" + " test cutscene\nI am Karl Marx, I've come to bring you my\nCommunist manifesto of doom.");
	mCutscene.addSlide(0, (string)"\x89\x8C\x86" + " I'm the ninja man");
	mCutscene.addSlide(3, (string)"\x89\x8C\x86" + " And I'm the goldfish in charge");
	mCutscene.addSlide(1, (string)"\x89\x8C\x86" + " test cutscene");
	mCutscene.addSlide(0, (string)"\x89\x8C\x86" + " test cutscene");
	gWriter.ClearTicks();
}

void Game::mCutSceneLoop()
{
	SDL_SetRenderDrawColor(mRenderer, 0, 0, 0, 0xFF);
	SDL_RenderClear(mRenderer);
	//event handler
	while (SDL_PollEvent(&mEvent))
	{
		handleGeneralEvents();
		mCutscene.handleEvent(mEvent);

		if (mCutscene.checkComplete())//no need to poll if done
		{
			mCurrentMenu = kInGame;
			return;
		}
	}

	//Draw the slide
	mCutscene.renderCurrentSlide();
	SDL_RenderPresent(mRenderer);
}

void Game::mSetMenu()
{
	
	//Main Menu
	mMenu[kMainMenu] = Menu(kMainMenu, "Ninja (working title)");

	//Main Options
	mMenu[kMainOptions] = Menu(kMainOptions, "Ninja (working title)");

	//Pause Menu
	mMenu[kPauseMenu] = Menu(kPauseMenu, "Ninja (working title)");

	//Pause Options
	mMenu[kPauseOptions] = Menu(kPauseOptions, "Ninja (working title)");

	//Game Over Screen
	mMenu[kGameOver] = Menu(kGameOver, "\x89\x80"+(string)"GAME OVER", {56,16});
	mMenu[kGameOver].addButton("Continue", kRestartZone, { 64, 160 }, (string)"\x82\x8E", (string)"\x80");
	mMenu[kGameOver].addButton("Quit", kSetQuit, {144, 160}, (string)"\x82\x8E", (string)"\x80");
}

void Game::mMenuLoop()
{
	SDL_SetRenderDrawColor(mRenderer, 0, 0, 0, 0xFF);
	SDL_RenderClear(mRenderer);
	if (mMenu.size() <= mCurrentMenu)
	{
		cout << "Game::MenuLoop segfault\n";
		return;
	}
	while (SDL_PollEvent(&mEvent))
	{
		//in case the menu was switched inside mButtonOptionHandler
		if (mCurrentMenu >= kTotalMenus)
			return;

		ButtonOption a = mMenu[mCurrentMenu].handleEvent(mEvent);
		handleGeneralEvents();
		mButtonOptionHandler(a);
	}

	//render the player's corpse
	//mPlayer.render(mCamera.x, mCamera.y);

	mMenu[mCurrentMenu].renderMenu();
	SDL_RenderPresent(mRenderer);
}

void Game::mButtonOptionHandler(ButtonOption & a)
{
	switch (a)
	{
	case kSetZone1: break;
	case kSetZone2: break;
	case kSetZone3: break;
	case kSetZone4: break;
	case kSetZone5: break;
	case kSetZone6: break;
	case kSetZone7:	break;
	case kSetZone8: break;
	case kSetMainMenu: break;
	case kSetMainOptions: break;

	case kSetQuit: mQuit = true; break;

	case kSetPauseMenu: break;
	case kSetPauseOption: break;

	case kRestartZone:
		mPlayer.getHealth() = mPlayer.getMaxHealth();
		mPlayer.resetLives();
		mZone.setLevel(0);
		mPlayer.respawn();
		mCurrentMenu = kInGame;
		break;

	case kUnPause: break;
	case kMusicDecrease: break;
	case kMusicIncrease: break;
	case kSFXDecrease: break;
	case kSFXIncrease: break;

	case kToggleFullscreen: mToggleFullScreen(); break;

	}
}
