#include "Game.h"

using namespace std;

Game::Game()
{
	mRenderer = NULL;
	mWindow = NULL;
	mColorKey = { 255, 0, 255 };
	mQuit = false;
	mCamera = { 0, 0, kScreenWidth, kScreenHeight };
	mDebug = false;
	mTimer.setFrameDelay(kFramePeriod);
}

bool Game::init()
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
	mWindow = SDL_CreateWindow("ninja", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
#ifndef NDEBUG//(if not-not debug) Also known as (if debug)
		kScreenWidth, kScreenHeight, SDL_WINDOW_SHOWN /*| SDL_WINDOW_FULLSCREEN_DESKTOP*/);
#else
		kScreenWidth, kScreenHeight, SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN_DESKTOP);
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
	Uint8 bgTexNum = 6;
	string bgTexS[] =
	{
		"GFX/BG/bluesky.png",
		"GFX/BG/snowmtn.png",
		"GFX/BG/grasshills.png",
		"GFX/BG/nightynight.png",
		"GFX/BG/skyline.png",
		"GFX/BG/redsky.png",
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


	//gFontTexture.setRenderer(gRenderer);
	if (!mFontTexture.loadTextureFile("GFX/font.png", &mColorKey))
	{
		cout << SDL_GetError() << endl;
		return false;
	}
	gWriter = TextWriter(&mFontTexture, 6, 8);

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
	mSoundBox.loadSFX("SFX/MUS/01.wav",true);
	//mSoundBox.playMusic(1);
	//Sounds

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
	mZone.clearHitboxesCurrentLevel();
	handleEvents();
	beginstep();
	endstep();
	render();
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
		if (mEvent.type == SDL_QUIT)//alt-f4 or clicking x on window
			mQuit = true;

		if (mEvent.key.keysym.sym == SDLK_BACKQUOTE && mEvent.type == SDL_KEYDOWN && mEvent.key.repeat == 0)//press tilde key
			mDebug = !mDebug;//toggle debug

		if (mEvent.key.keysym.sym == SDLK_ESCAPE && mEvent.type == SDL_KEYDOWN && mEvent.key.repeat == 0)//on keypress ESC
		{
			mZone.setLevel(1); //switch level
		}
	
		mPlayer.handleEvent(mEvent);
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
			//GAME OVER
		}
	}
	mPlayer.step();
	mZone.stepEnemiesCurrentLevel();
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
		gWriter << '\x86' + (string)"Ninja pos: (" << mPlayer.getX() << ",\x82 " << mPlayer.getY() << '\x86' << ")\n";
		gWriter << '\x8A' << '\x88' + (string)"Enemy Count: " << (int)mZone.enemyCountCurrentLevel() << '\x89' << '\n';
		gWriter << '\x83' << mTimer.getFramerate() << " FPS\n";
		gWriter << '\x86' << mTimer.getVSyncFramerate() << " FPS - VS\n";
		//if (rr != 0)
		//{
		gWriter << '\x84' << mRefreshRate << "Hz Monitor with dimensions " << mScreen.x << 'x' << mScreen.y << '\n';
		//}
		//gWriter << '\x88' << "Frame #"<< mClock.getFrameCount() << '\n';

		mZone.debugShowHitboxesCurrentLevel(*mRenderer);
	}
}
