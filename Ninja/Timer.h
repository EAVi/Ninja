#ifndef TIMER_H
#define TIMER_H

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

class Timer
{
public:
	Timer();

	//adds the ticks between the end of the previous frame and the current time
	//This allows the ability to calculate how much your framerate would be without vsync
	void vtick();

	//increments frames and also sets a timeframe for vtick()
	void tick();

	//does the math and returns the framerate
	int getFramerate();

	//returns the vsync framerate, which is calculated in getFrameRate()
	int getVSyncFramerate();

	//gets the frame count
	int getFrameCount();
private:
	int mFrameCount;
	int mDiffFrame;
	int mDiffTick;
	int mLastTick;
	int mVSyncTicks;
	int mFrameRate;
	int mVSyncFrameRate;
};

#endif
