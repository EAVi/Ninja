#include "Timer.h"
#include "TextWriter.h"

extern TextWriter gWriter;

Timer::Timer()
{
	mFrameCount = 0;
	mDiffFrame = 0;
	mLastTick = 0;
	mVSyncTicks = 0;
	mFrameRate = 0;
	mVSyncFrameRate = 0;
}

void Timer::vtick()
{
	mVSyncTicks += (int)SDL_GetTicks() - mLastTick;
}

void Timer::tick()
{
	mLastTick = SDL_GetTicks(); //a point in time for the vtick to reference

	//increment the framecounts
	++mFrameCount;
	++mDiffFrame;

}

int Timer::getFramerate()
{
	//first frame initializes the values
	if (mDiffFrame == 0)
		mDiffTick = SDL_GetTicks();

	//last frame des the math
	//this means the framerate updates every second
	if (mDiffFrame >= 59)
	{
		mFrameRate = (int)(((mDiffFrame + 1) * 1000)/((int)SDL_GetTicks() - mDiffTick));
		if (mVSyncTicks != 0)
			mVSyncFrameRate = (int)(((mDiffFrame + 1) * 1000) / (mVSyncTicks));
		
		mDiffTick = SDL_GetTicks();
		mVSyncTicks = 0;
		mDiffFrame = 0;
	}

	return mFrameRate;
}

int Timer::getVSyncFramerate()
{
	return mVSyncFrameRate;
}

int Timer::getFrameCount()
{
	return mFrameCount;
}

void Timer::setFrameDelay(int a)
{
	mFrameDelay = a;
}

void Timer::delayRender()
{
	int delay = SDL_GetTicks() - mLastTick;
	if (delay < mFrameDelay)
		SDL_Delay(mFrameDelay - delay);
}
