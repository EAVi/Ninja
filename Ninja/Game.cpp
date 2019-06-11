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
	mRecording = false;
	mTimer.setFrameDelay(kFramePeriod);
	mCurrentMenu = kMainMenu;	
	mBackgroundTextures.clear();
	mDoorTextures.clear();
	mMenu = vector<Menu>(kTotalMenus);
	mCutscene = vector<Cutscene>();
	mCurrentCutScene = 255;
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

#define BOOL_ASSET_LOAD(loadcmd)\
{\
	if (!loadcmd)\
	{\
		cout << SDL_GetError() << endl;\
		return false;\
	}\
}

bool Game::loadAssets()
{
	LTexture::mRenderer = mRenderer;

	//gNinjaTexture.setRenderer(gRenderer);
	BOOL_ASSET_LOAD(mNinjaTexture.loadTextureFile("GFX/AllNinja.png", &mColorKey));
	mPlayer.setTexture(&mNinjaTexture);

	//gBlockTexture.setRenderer(gRenderer);
	BOOL_ASSET_LOAD(mBlockTexture.loadTextureFile("GFX/BlockSheet.png", &mColorKey));

	Level tempLevel;//creates a temporary level, just to set the static texture
	tempLevel.setBlockTextures(&mBlockTexture);

	//background texture loader
	Uint8 bgTexNum = 22;
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
		"GFX/BG/castlesky.png",
		"GFX/BG/beanstalk.png",
		"GFX/BG/houses.png",
		"GFX/BG/templeinterior.png",
		"GFX/BG/wavetex.png",
		"GFX/BG/traininterior.png",
		"GFX/BG/tunnel.png",
		"GFX/BG/tree.png"
	};

	for (Uint8 i = 0; i < bgTexNum; ++i)
	{
		LTexture* tempptr = new LTexture;
		//tempptr->setRenderer(gRenderer);
		BOOL_ASSET_LOAD(tempptr->loadTextureFile(bgTexS[i], &mColorKey));
		mBackgroundTextures.push_back(tempptr);
	}

	//door texture loader
	Uint8 doorTexNum = 2;
	string doorTexS[] =
	{
		"GFX/DOOR/caveenter.png",
		"GFX/DOOR/caveexit.png",
	};

	for (Uint8 i = 0; i < doorTexNum; ++i)
	{
		LTexture* tempptr = new LTexture;
		//tempptr->setRenderer(gRenderer);
		BOOL_ASSET_LOAD(tempptr->loadTextureFile(doorTexS[i], &mColorKey));
		mDoorTextures.push_back(tempptr);
	}

	//menu texture loader
	Uint8 menuTexNum = 3;
	string menuTexS[] =
	{
		"GFX/MENU/rect.png",
		"GFX/MENU/dirt.png",
		"GFX/MENU/ninjapractice.png",
	};

	for (Uint8 i = 0; i < menuTexNum; ++i)
	{
		LTexture* tempptr = new LTexture;
		//tempptr->setRenderer(gRenderer);
		BOOL_ASSET_LOAD(tempptr->loadTextureFile(menuTexS[i], &mColorKey));
		Menu::mMenuTextures.push_back(tempptr);
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
		BOOL_ASSET_LOAD(tempptr->loadTextureFile(uiTexS[i], &mColorKey));
		UIDrawer::gUITextures.push_back(tempptr);
	}

	//cutscene texture loader
	Uint8 cutTexNum = 17;
	string cutTexS[] =
	{
		"GFX/CUT/ninjaclose.png",
		"GFX/CUT/ninjaforward.png",
		"GFX/CUT/karl.png",
		"GFX/CUT/goldfish.png",
		"GFX/CUT/ninjadark.png",
		"GFX/CUT/demonoutline.png",
		"GFX/CUT/paper.png",
		"GFX/CUT/macho.png",
		"GFX/CUT/ninjadying.png",
		"GFX/CUT/caveexit.png",
		"GFX/CUT/goldfishjail.png",
		"GFX/CUT/machoback.png",
		"GFX/CUT/machosun.png",
		"GFX/CUT/danu.png",
		"GFX/CUT/packet.png",
		"GFX/CUT/sartre.png",
		"GFX/CUT/vinointro.png",
	};
	for (Uint8 i = 0; i < cutTexNum; ++i)
	{
		LTexture* tempptr = new LTexture;
		//tempptr->setRenderer(gRenderer);
		BOOL_ASSET_LOAD(tempptr->loadTextureFile(cutTexS[i], &mColorKey));
		Cutscene::mSlideTextures.push_back(tempptr);
	}


	//gFontTexture.setRenderer(gRenderer);
	BOOL_ASSET_LOAD(mFontTexture.loadTextureFile("GFX/font.png", &mColorKey));
	BOOL_ASSET_LOAD(mLargeFontTexture.loadTextureFile("GFX/largefont.png", &mColorKey));
	
	gWriter = TextWriter(&mFontTexture, 6, 8, &mLargeFontTexture, 16, 24);

	//Set the static enemy textures
	Enemy tempEnemy;
	BOOL_ASSET_LOAD(mEnemyTexture.loadTextureFile("GFX/AllGhost.png", &mColorKey));
	tempEnemy.setTexture(&mEnemyTexture);

	Robot tempRobot;
	BOOL_ASSET_LOAD(mRobotTexture.loadTextureFile("GFX/AllRoboPirate.png", &mColorKey));
	tempRobot.setTexture(&mRobotTexture);

	Demon tempDemon;
	BOOL_ASSET_LOAD(mDemonTexture.loadTextureFile("GFX/AllDemon.png", &mColorKey));
	tempDemon.setTexture(&mDemonTexture);

	Cannon tempCannon;
	BOOL_ASSET_LOAD(mCannonTexture.loadTextureFile("GFX/AllCannon.png", &mColorKey));
	tempCannon.setTexture(&mCannonTexture);

	Samurai tempSamurai;
	BOOL_ASSET_LOAD(mSamuraiTexture.loadTextureFile("GFX/AllSamurai.png", &mColorKey));
	tempSamurai.setTexture(&mSamuraiTexture);

	//Load all the sounds and music
	//Music
	const int defvolume = 30;
	mSoundBox.loadSFX("SFX/MUS/00.wav", true);
	mSoundBox.loadSFX("SFX/MUS/01.wav", true);
	mSoundBox.loadSFX("SFX/MUS/02.wav", true);
	mSoundBox.loadSFX("SFX/MUS/03.wav", true);
	mSoundBox.loadSFX("SFX/MUS/04.wav", true);
	mSoundBox.loadSFX("SFX/MUS/05.wav", true);
	mSoundBox.loadSFX("SFX/MUS/06.wav", true);
	mSoundBox.loadSFX("SFX/MUS/07.wav", true);
	mSoundBox.loadSFX("SFX/MUS/08.wav", true);
	Mix_VolumeMusic(defvolume);
	//Sounds
	mSoundBox.loadSFX("SFX/00.wav");
	mSoundBox.loadSFX("SFX/01.wav");
	mSoundBox.loadSFX("SFX/02.wav");
	mSoundBox.loadSFX("SFX/03.wav");
	mSoundBox.loadSFX("SFX/04.wav");
	mSoundBox.loadSFX("SFX/05.wav");
	mSoundBox.loadSFX("SFX/06.wav");
	Mix_Volume(-1, defvolume);

	return true;
}

void Game::prepare()
{
	//load zone
	/*
	mZone.setZoneID(255);
	mZone.setPrefix("data/debug");
	mZone.setSuffix(".txt");
	mZone.setCamera(&mCamera);
	mZone.setPlayer(&mPlayer);
	mZone.setBGTextures(mBackgroundTextures);
	mZone.setDoorTextures(mDoorTextures);
	mZone.init();
	mZone.setSpawn();
	*/
	prepareZone(1);
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

void Game::prepareZone(Uint8 a)
{
	mPlayer.resetLives();
	mZone.release();
	mZone = Zone();
	//load zone
	
	switch (a)
	{
	case 1:
		mZone.setZoneID(1);
		mZone.setPrefix("data/01");
		mZone.setSuffix(".bin");
		break;
	case 2:
		mZone.setZoneID(2);
		mZone.setPrefix("data/02");
		mZone.setSuffix(".bin");
		break;
	case 3:
		mZone.setZoneID(3);
		mZone.setPrefix("data/03");
		mZone.setSuffix(".bin");
		break;
	case 4:
		mZone.setZoneID(4);
		mZone.setPrefix("data/04");
		mZone.setSuffix(".bin");
		break;
	case 5:
		mZone.setZoneID(5);
		mZone.setPrefix("data/05");
		mZone.setSuffix(".bin");
		break;
	case 6:
		mZone.setZoneID(6);
		mZone.setPrefix("data/06");
		mZone.setSuffix(".bin");
		break;
	case 7:
		mZone.setZoneID(7);
		mZone.setPrefix("data/07");
		mZone.setSuffix(".bin");
		break;
	case 8:
		mZone.setZoneID(8);
		mZone.setPrefix("data/08");
		mZone.setSuffix(".bin");
		break;
	default:
		mZone.setZoneID(255);
		mZone.setPrefix("data/debug");
		mZone.setSuffix(".bin");
		break;
	}
	mZone.setCamera(&mCamera);
	mZone.setPlayer(&mPlayer);
	mZone.setBGTextures(mBackgroundTextures);
	mZone.setDoorTextures(mDoorTextures);
	mSetCutscene();
	//(&mCamera, &mPlayer, "data/debug", ".txt", mBackgroundTextures);
	mZone.init();
	if (mZone.getLevelSize() <= 0)
	{
		mQuit = true;
		return;
	}
	mZone.setSpawn();
}

void Game::introSequence()
{
	LTexture tempLogo;//the logo for magnar games
	LTexture tempSpecialThanks;//the special thanks image, Open Source software rules!

	tempLogo.loadTextureFile("GFX/magnarlogo.png", &mColorKey);//load the texture
	tempSpecialThanks.loadTextureFile("GFX/specialthanks.png", &mColorKey);//load the texture


	int logox = 64, logoy = 64;//x and y position of the logo
	int fade = 20;//number of frames it takes for the logo to fade in and out
	int fadein = 191;
	int fadeout = 235;
	int introframes = 577;//the intro tune is 9.62 seconds, or 577 frames
	int thanksframes = 280;
	mSoundBox.playMusic(0, 0);

	string thanks = "Thank you:\n\n";
	thanks += "Old Man Jackson\n\n";
	thanks += "My Professors\n";
	thanks += "My Friends\n\n\n";
	thanks += "   & you\nThank You <3\n\n";

	string s = "MAGNAR";


	for (int i = 0; i < introframes; ++i)
	{
		if (mSub60 && mOddFrame //odd frame of a 30Hz monitor 
			|| !mSub60)//or regular frame of a 60Hz(or more) monitor
		{
			SDL_SetRenderDrawColor(mRenderer, 0, 0, 0, 0xFF);
			SDL_RenderClear(mRenderer);
			while (SDL_PollEvent(&mEvent))//simple event polling
			{
				handleGeneralEvents();

				//on ENTER or START button, skip sequence
				if (mEvent.key.keysym.sym == SDLK_ESCAPE && mEvent.type == SDL_KEYDOWN && mEvent.key.repeat == 0// on ENTER
					|| mEvent.type == SDL_CONTROLLERBUTTONDOWN && mEvent.cbutton.button == SDL_CONTROLLER_BUTTON_START)// or START button
					i = introframes;//change i, exiting the while loop
			}
			switch (i)//modify the text based on the frame number
			{
			case 0: s = txt::Rainbow + (string)"MAGNAR"; break;
			case 16: s = (string)"M" + txt::Rainbow + (string)"AGNAR"; break;//first note at .2712s = 16.2 frames
			case 79: s = (string)"MA" + txt::Rainbow + (string)"GNAR"; break;//second note at 1.328s = 79.6 frames
			case 104: s = (string)"MAG" + txt::Rainbow + (string)"NAR"; break;//third note at 1.7420s = 104.5 frames
			case 132: s = (string)"MAGN" + txt::Rainbow + (string)"AR"; break;//fourth note at 2.211S = 132.7 frames
			case 167: s = (string)"MAGNA" + txt::Rainbow + (string)"R"; break;//fifth note at 2.785s = 167.1 frames
			case 191: s = "MAGNAR"; break;//sixth note at 3.195s = 191.7 frames
			case 230: s = ""; break;//the logo is fully visible at 211, and disapears at 235
			}

			/* fading logic for the magnar logo*/
			if (i > fadein
				&& i < fadein + fade)//if the logo is fading in
			{
				Uint8 A;
				A = (255 * (i - fadein) / (fade));
				tempLogo.setAlpha(A);
			}
			else if (i >= fadein + fade && i <= fadeout)//in the middle where the logo is visible 
				tempLogo.setAlpha(255);
			else if (i > fadeout
				&& i < fadeout + fade)//if the logo is fading out
			{
				Uint8 A;
				A = (255 * (fadeout - i) / (fade));
				tempLogo.setAlpha(A);
			}
			else tempLogo.setAlpha(0);

			if (i >= thanksframes && (i < introframes - 20))
			{
				gWriter(textbuffers::Debug) << txt::White << thanks;
				tempSpecialThanks.renderTexture(128, 0);
			}
				

			tempLogo.renderTexture(logox, logoy);
			gWriter(textbuffers::Large, 78, 82) << txt::White << s;
			gWriter.ClearBuffer();
		
			mTimer.vtick();
			if (mSuper60)
			{
				mTimer.delayRender();
			}

			SDL_RenderPresent(mRenderer);
			mTimer.tick();
		}

		if (mQuit) break;
	}
	
	tempLogo.freeTexture();
	tempSpecialThanks.freeTexture();
}

void Game::destroyAssets()
{
	mZone.release();
	mNinjaTexture.freeTexture();
	mEnemyTexture.freeTexture();
	mRobotTexture.freeTexture();
	mDemonTexture.freeTexture();
	mCannonTexture.freeTexture();
	mBlockTexture.freeTexture();
	mFontTexture.freeTexture();
	mLargeFontTexture.freeTexture();

	//deallocate the background textures
	for (Uint8 i = 0, j = mBackgroundTextures.size(); i < j; ++i)
	{
		mBackgroundTextures[i]->freeTexture();
		delete mBackgroundTextures[i];//'new' memory was allocated for these textures
	}

	//deallocate the door textures
	for (Uint8 i = 0, j = mDoorTextures.size(); i < j; ++i)
	{
		mDoorTextures[i]->freeTexture();
		delete mDoorTextures[i];//'new' memory was allocated for these textures
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

	//Menu textures
	for (Uint8 i = 0, j = Menu::mMenuTextures.size(); i < j; ++i)
	{
		Menu::mMenuTextures[i]->freeTexture();
		delete Menu::mMenuTextures[i];//'new' memory was allocated for these textures
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
		if (mCurrentMenu == kInGame)//menu may change at the beginstep or handleEvents, this will prevent a 1 frame flicker
		{
			endstep();
			render();
		}
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

void Game::takeScreenShot()
{
	stringstream filename;
	time_t timestamp = time(0);
	struct tm now;
#if defined _WIN32 || _WIN64
	localtime_s(&now, &timestamp);
#elif defined __APPLE__ && __MACH__ || defined Macintosh || defined macintosh || defined __linux__
	localtime_r(&timestamp, &now);
#endif


	filename << "./Screenshots/NINJA_" //the path and prefix
		<< now.tm_year + 1900 << '_'
		<< now.tm_mon + 1 << '_'
		<< now.tm_mday << '-'
		<< now.tm_min << '_' << now.tm_sec << '-'
		<< mTimer.getFrameCount()
		<< ".bmp";
	string a = "lame";
	screenShot(mWindow, mRenderer,filename.str());
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

	if (mEvent.key.keysym.sym == SDLK_r && mEvent.type == SDL_KEYDOWN && mEvent.key.repeat == 0)//R key press toggles recording
		mRecording = !mRecording;//toggle recording

	if (mCurrentMenu == kInGame)
	{
		if (mEvent.key.keysym.sym == SDLK_ESCAPE && mEvent.type == SDL_KEYDOWN && mEvent.key.repeat == 0//on keypress ESC
			|| mEvent.type == SDL_CONTROLLERBUTTONDOWN && mEvent.cbutton.button == SDL_CONTROLLER_BUTTON_START //start button on controller
			)
		{
			mCurrentMenu = kPauseMenu;
		}
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
		mPlayer.removeLife();
		mPlayer.refillHealth();
		}
		else
		{
			mCurrentMenu = kGameOver;
		}
	}
	mPlayer.step();
	if (mZone.stepEnemiesCurrentLevel())//stepenemies returns true if player touched a door
	{
		mGetCutSceneCurrentLevel();
	}
	
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

		if (mRecording)
		{
			takeScreenShot();
			gWriter(textbuffers::Debug) << txt::Black << txt::HLightOn << "Currently Recording, press R to stop";
			gWriter.ClearBuffer();
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

		//gWriter(textbuffers::Large, 16 , 32) << txt::TypeOn << txt::Blue << txt::HLightOn << "(" << mPlayer.getX() << ',' << txt::Yellow << mPlayer.getY() << txt::White << ")" << txt::HLightOff;
		SDL_Delay(60);
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
	mCutscene.clear();
	Cutscene temp;
	temp.clearCutscene();
	//first cutscene at the first door
	temp.setTrigger({1,255});
	temp.addSlide(0, " Oh no, the developer of this game \nmade a portal to another world, you can\nfind it if you jump down that hole to\nthe right!");
	mCutscene.push_back(temp);

	//second cutscene before the boss
	temp.clearCutscene();
	temp.setTrigger({ 6,255 });
	temp.addSlide(5, "BEWARE!!!, there be a baddy\nup ahead\n\n!!!\n!!!");
	mCutscene.push_back(temp);

	temp.clearCutscene();
	temp.setTrigger({ 0,1 });
	temp.addSlide(1, "Hello, I'm Ninja, and this\nis my story.");
	temp.addSlide(1, "I was born under the \nOrange Ninja lineage where I'm\ncurrently being trained\nto be a Ninja");
	temp.addSlide(3, "I was ruled by king goldfish, \nwith whom our village was sworn\nto protect");
	temp.addSlide(6, "It was a regular day, more regular\nthan usual.");
	temp.addSlide(1, "I was at my favorite diner \nhaving brinner");
	temp.addSlide(1, "It was pretty late, so\nI decided to head back\nto the village");
	mCutscene.push_back(temp);

	//The secret area
	temp.clearCutscene();
	temp.setTrigger({ 3,1 });
	temp.addSlide(1, "This is the secret armory!\nI shouldn't be here,\nit's getting late");
	mCutscene.push_back(temp);

	//seige realization
	temp.clearCutscene();
	temp.setTrigger({ 4,1 });
	temp.addSlide(0, "Oh no! The village is \nunder attack!");
	mCutscene.push_back(temp);

	//small talk with volcanus
	temp.clearCutscene();
	temp.setTrigger({ 6,1 });
	temp.addSlide(15, "Hello, my name is Sartre;\nI'm here to talk to your king...");
	temp.addSlide(10, "HELP!");
	temp.addSlide(15, "Pardon, did I say \"Talk\"?\nI meant kidnap.");
	temp.addSlide(0, "The King!\n\nI'll Save you!");
	mCutscene.push_back(temp);

	//dying
	temp.clearCutscene();
	temp.setTrigger({ 7,1 });
	temp.addSlide(8, "Damn...");
	temp.addSlide(4, "Looks like that nerd\ndefeated me...");
	temp.addSlide(8, "The king...\nI couldn't save him");
	temp.addSlide(4, "I know!\nI'll turn into a ghost \nand haunt that bad guy!");
	temp.addSlide(9, "Dont go towards the light?\nKnock Knock!\nHere I come!");
	mCutscene.push_back(temp);

	//first glance at heaven
	temp.clearCutscene();
	temp.setTrigger({ 1,2 });
	temp.addSlide(1, "Nice place they've got here");
	mCutscene.push_back(temp);

	//talk with god himself
	temp.clearCutscene();
	temp.setTrigger({ 3,2 });
	temp.addSlide(0, "HEY!\nWho's in charge here?");
	temp.addSlide(2, "I am");
	temp.addSlide(0, "Karl Marx, what are \nyou doing here?");
	temp.addSlide(2, "Didn't you hear?\n\nI'm in charge in these parts");
	temp.addSlide(0, "Neat");
	temp.addSlide(1, "Anyways, I died and \nI'd like a refund");
	temp.addSlide(2, "Sure, I can get you\nback down there;\nand while you're there\ncan you run some errands?");
	mCutscene.push_back(temp);
}

void Game::mCutSceneLoop()
{
	if (mCurrentCutScene == 255)
		if (!mZone.checkComplete())//if the zone has not been completed
			mCurrentMenu = kInGame;
		else if (mZone.getLevelID().ZoneNo < kFinalZone)//zone complete
		{
			mLevelIntroSequence(mZone.getLevelID().ZoneNo + 1);
		}
		else if (mZone.getLevelID().ZoneNo == 255)//debug zone
		{
			mLevelIntroSequence(1);
		}
		else if (mZone.getLevelID().ZoneNo == kFinalZone)//zone complete, final zone
		{
			mCurrentMenu = kInGame;
			//if you beat kfinalzone, then the game is complete!
			//call credits or whatever here
		}
	
	SDL_SetRenderDrawColor(mRenderer, 0, 0, 0, 0xFF);
	SDL_RenderClear(mRenderer);
	//event handler
	while (SDL_PollEvent(&mEvent) && !mCutscene[mCurrentCutScene].checkComplete())
	{
		handleGeneralEvents();
		mPlayer.handleEvent(mEvent, true);//because the player could release the movement key/button and it would otherwise not register.
		mCutscene[mCurrentCutScene].handleEvent(mEvent);

		if (mCutscene[mCurrentCutScene].checkComplete())//no need to poll if done
		{
			if (!mZone.checkComplete())//zone not completed
			{
				mCurrentMenu = kInGame;
				return;
			}
			else if (mZone.getLevelID().ZoneNo == 255)//debug zone
			{
				mLevelIntroSequence(1);
				return;
			}
			else if (mZone.getLevelID().ZoneNo < kFinalZone)//completed zone
			{
				mLevelIntroSequence(mZone.getLevelID().ZoneNo + 1);
				return;
			}
			else if (mZone.getLevelID().ZoneNo == kFinalZone)//completed zone, final zone
			{
				mCurrentMenu = kInGame;
				//if you beat kfinalzone, then the game is complete!
				//call credits or whatever here
			}
		}
	}

	//Draw the slide
	mCutscene[mCurrentCutScene].renderCurrentSlide();
	SDL_RenderPresent(mRenderer);
}

void Game::mSetMenu()
{
	
	//Main Menu
	mMenu[kMainMenu] = Menu(kMainMenu, "\0");
	mMenu[kMainMenu].addButton("New Game", kSetZone1, { 8, 152 }, (string)"\x82\x8E", (string)"\x86");
	mMenu[kMainMenu].addButton("Continue", kSetStageSelect, { 16, 160 }, (string)"\x82\x8E", (string)"\x86");
	mMenu[kMainMenu].addButton("Options", kSetMainOptions, { 24, 168 }, (string)"\x82\x8E", (string)"\x86");
	mMenu[kMainMenu].setMusic(5);
	mMenu[kMainMenu].setTextureNum(2);
	mMenu[kMainMenu].addButton("Quit", kSetQuit, { 32, 176 }, (string)"\x82\x8E", (string)"\x86");

	//Main Options
	mMenu[kMainOptions] = Menu(kMainOptions, "\x87" + (string)"\x8E" + (string)"Options", {64,32});
	mMenu[kMainOptions].addButton("+", kMusicIncrease, { 160 , 128 }, (string)"\x82\x8E", (string)"\x86");
	mMenu[kMainOptions].addButton("-", kMusicDecrease, { 198 , 128 }, (string)"\x82\x8E", (string)"\x86");
	mMenu[kMainOptions].addButton("+", kSFXIncrease, { 160 , 136 }, (string)"\x82\x8E", (string)"\x86");
	mMenu[kMainOptions].addButton("-", kSFXDecrease, { 198 , 136 }, (string)"\x82\x8E", (string)"\x86");
	mMenu[kMainOptions].addButton("Back", kSetMainMenu, { 200 , 200 }, (string)"\x82\x8E", (string)"\x86");
	mMenu[kMainOptions].setTextureNum(1);
	mMenu[kMainOptions].setBackButtonOption(kSetMainMenu);


	//Pause Menu
	mMenu[kPauseMenu] = Menu(kPauseMenu, "Paused");
	mMenu[kPauseMenu].addButton("Continue", kUnPause, { 8, 152 }, (string)"\x82\x8E", (string)"\x86");
	mMenu[kPauseMenu].addButton("Options", kSetPauseOption, { 16, 160 }, (string)"\x82\x8E", (string)"\x86");
	mMenu[kPauseMenu].addButton("Quit", kSetMainMenu, {24, 168}, (string)"\x82\x8E", (string)"\x86");
	mMenu[kPauseMenu].setBackButtonOption(kUnPause);

	//Pause Options
	mMenu[kPauseOptions] = Menu(kPauseOptions, "\x87" + (string)"\x8E" + (string)"Options", { 64,32 });
	mMenu[kPauseOptions].addButton("+", kMusicIncrease, { 160 , 128 }, (string)"\x82\x8E", (string)"\x86");
	mMenu[kPauseOptions].addButton("-", kMusicDecrease, { 198 , 128 }, (string)"\x82\x8E", (string)"\x86");
	mMenu[kPauseOptions].addButton("+", kSFXIncrease, { 160 , 136 }, (string)"\x82\x8E", (string)"\x86");
	mMenu[kPauseOptions].addButton("-", kSFXDecrease, { 198 , 136 }, (string)"\x82\x8E", (string)"\x86");
	mMenu[kPauseOptions].addButton("Back", kSetPauseMenu, { 200 , 200 }, (string)"\x82\x8E", (string)"\x86");
	mMenu[kPauseOptions].setTextureNum(1);
	mMenu[kPauseOptions].setBackButtonOption(kSetPauseMenu);

	//Game Over Screen
	mMenu[kGameOver] = Menu(kGameOver, "\x89\x80"+(string)"GAME OVER", {56,16});
	mMenu[kGameOver].addButton("Continue", kRestartZone, { 64, 160 }, (string)"\x82\x8E", (string)"\x80");
	mMenu[kGameOver].addButton("Quit", kSetMainMenu, { 144, 160 }, (string)"\x82\x8E", (string)"\x80");
	mMenu[kGameOver].setTextureNum(255);

	//Stage Select
	mMenu[kStageSelect] = Menu(kStageSelect, "Stage Select");
	mMenu[kStageSelect].addButton("Debug", kSetZoneDebug, { 8 , 56 }, (string)"\x82\x8E", (string)"\x86");
	mMenu[kStageSelect].addButton("Zone 1", kSetZone1, { 8 , 64 }, (string)"\x82\x8E", (string)"\x86");
	mMenu[kStageSelect].addButton("Zone 2", kSetZone2, { 8 , 72 }, (string)"\x82\x8E", (string)"\x86");
	mMenu[kStageSelect].addButton("Zone 3", kSetZone3, { 8 , 80 }, (string)"\x82\x8E", (string)"\x86");
	mMenu[kStageSelect].addButton("Zone 4", kSetZone4, { 8 , 88 }, (string)"\x82\x8E", (string)"\x86");
	mMenu[kStageSelect].addButton("Zone 5", kSetZone5, { 8 , 96 }, (string)"\x82\x8E", (string)"\x86");
	mMenu[kStageSelect].addButton("Zone 6", kSetZone6, { 8 , 104 }, (string)"\x82\x8E", (string)"\x86");
	mMenu[kStageSelect].addButton("Zone 7", kSetZone7, { 8 , 112 }, (string)"\x82\x8E", (string)"\x86");
	mMenu[kStageSelect].addButton("Zone 8", kSetZone8, { 8 , 120 }, (string)"\x82\x8E", (string)"\x86");
	mMenu[kStageSelect].addButton("Back", kSetMainMenu, { 200 , 200 }, (string)"\x82\x8E", (string)"\x86");
	mMenu[kStageSelect].setBackButtonOption(kSetMainMenu);
	mMenu[kStageSelect].setTextureNum(255);
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
		mPlayer.handleEvent(mEvent,true);//handles only the player movement

	}

	//render the player's corpse
	//mPlayer.render(mCamera.x, mCamera.y);

	if (mCurrentMenu < kTotalMenus)//the event handler may change the menu type to a non-menu suck as kInGame
	{
		mMenu[mCurrentMenu].renderMenu();
		LAudio::playMusic(mMenu[mCurrentMenu].getMusic());
	}

	//shows the volume on the options menus
	if (mCurrentMenu == kMainOptions || mCurrentMenu == kPauseOptions)
	{
		gWriter(textbuffers::Small, 96, 128) << txt::White << "MUS Volume   " << txt::Blue << Mix_VolumeMusic(-1);
		gWriter << txt::White << "\nSFX Volume   " << txt::Blue << Mix_Volume(-1, -1);
		gWriter.ClearBuffer();
	}

	SDL_RenderPresent(mRenderer);
}

void Game::mButtonOptionHandler(ButtonOption & a)
{
	int mVolume = Mix_VolumeMusic(-1);//get music volume
	int sVolume = Mix_Volume(-1, -1);//get sound volume

	switch (a)
	{
	case kSetZoneDebug: mLevelIntroSequence(255); break;
	case kSetZone1: mLevelIntroSequence(1); break;
	case kSetZone2: mLevelIntroSequence(2); break;
	case kSetZone3: mLevelIntroSequence(3); break;
	case kSetZone4: mLevelIntroSequence(4); break;
	case kSetZone5: mLevelIntroSequence(5); break;
	case kSetZone6: mLevelIntroSequence(6); break;
	case kSetZone7:	mLevelIntroSequence(7); break;
	case kSetZone8: mLevelIntroSequence(8); break;
	case kSetMainMenu: 
		mCurrentMenu = kMainMenu; 
		LAudio::playSound(4);//play the "doo do doo" sound
		break;
	case kSetMainOptions: mCurrentMenu = kMainOptions; break;
	case kSetQuit: mQuit = true; break;
	case kSetPauseMenu: 
		mCurrentMenu = kPauseMenu; 
		LAudio::playSound(4);//play the "doo do doo" sound
		break;
	case kSetPauseOption: mCurrentMenu = kPauseOptions; break;
	case kSetStageSelect: mCurrentMenu = kStageSelect; break;
	case kRestartZone:
		mPlayer.resetLives();
		mZone.reloadZone();
		mSetCutscene();//reload the cutscenes every restart
		mZone.setLevel(0);
		mPlayer.respawn();
		mCurrentMenu = kInGame;
		break;

	case kUnPause: mCurrentMenu = kInGame; break;
	case kMusicDecrease:
		mVolume -= 10;
		if (mVolume < 0) mVolume = 0;
		Mix_VolumeMusic(mVolume);
		LAudio::playSound(5);//play the "ding" sound
		break;
	case kMusicIncrease:
		mVolume += 10;
		if (mVolume > 100) mVolume = 100;
		Mix_VolumeMusic(mVolume);
		LAudio::playSound(5);//play the "ding" sound
		break;
	case kSFXDecrease:
		sVolume -= 10;
		if (sVolume < 0) sVolume = 0;
		Mix_Volume(-1, sVolume);
		LAudio::playSound(0);
		break;
	case kSFXIncrease: 
		sVolume += 10;
		if (sVolume > 100) sVolume = 100;
		Mix_Volume(-1, sVolume);
		LAudio::playSound(0);
		break;
	case kToggleFullscreen: mToggleFullScreen(); break;
	}
}

void Game::mGetCutSceneCurrentLevel()
{
	LevelID id;
	if (!mZone.checkComplete())
		id = mZone.getLevelID();
	else//zone complete
	{
		Uint8 ZoneID = mZone.getLevelID().ZoneNo;
		id = { (Uint8)mZone.getLevelSize(), ZoneID };
	}

	for (int i = 0, j = mCutscene.size(); i < j; ++i)
		if (mCutscene[i].checkTrigger(id))
		{
			if (mCutscene[i].checkComplete()) break; //would result in black flicker when switching to a completed cutscene
			mCurrentCutScene = i;
			mCurrentMenu = kCutscene;
			gWriter.ClearTicks();
			return;
		}


	//returning a cutscene will skip this logic
	//putting this condition after the loop prevents the music from the next level from playing over these cutscenes
	if (mZone.checkComplete())
	{
		Uint8 ZoneID = mZone.getLevelID().ZoneNo;

		if (ZoneID == 255)
			mLevelIntroSequence(1);
		else if (ZoneID < kFinalZone)
			mLevelIntroSequence(ZoneID + 1);
		else
		{
			//you just beat the final zone
			//call a credits function here
			//if there is nothing here, you'll just end up stuck on the last zone
		}
	}
}

void Game::mLevelIntroSequence(Uint8 a)
{
	string s = "";

	switch (a)
	{
	case 1:
		s = "Epilogue";
		break;
	case 2:
		s = txt::Red + (string)"To the Light";
		break;
	case 3:
		s = txt::Blue + (string)"Ninja Vacation";
		break;
	case 4:
		break;
	case 5:
		break;
	case 6:
		break;
	case 7:
		break;
	case 8:
		break;
	default:
		break;
	}

	LAudio::playMusic(254);

	for (int i = 0, noframes = 240; i < noframes; ++i)
	{
		//print the string for a number of frames
		//
		if (mSub60 && mOddFrame //odd frame of a 30Hz monitor 
			|| !mSub60)//or regular frame of a 60Hz(or more) monitor
		{
			SDL_RenderClear(mRenderer);
			while (SDL_PollEvent(&mEvent))//simple event polling
			{
				handleGeneralEvents();
			}
			gWriter(textbuffers::Large) << txt::White << "Zone " << (int)a << '\n' << s;
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
	while (SDL_PollEvent(&mEvent))
	{
		//do nothing
	}

	//all the preparations for the level
	prepareZone(a);
	mSoundBox.playMusic(mZone.getSongCurrentLevel());
	mCurrentMenu = kInGame;
	mGetCutSceneCurrentLevel();
}
