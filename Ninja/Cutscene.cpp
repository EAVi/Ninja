#include "Cutscene.h"

using namespace std;

extern TextWriter gWriter;

//initialize statiic variable
std::vector<LTexture*> Cutscene::mSlideTextures = vector<LTexture*>();

Cutscene::Cutscene()
{
	mSlides.clear();
	mCurrentSlide = 0;
}

void Cutscene::addSlide(Slide a)
{
	mSlides.push_back(a);
}

void Cutscene::addSlide(Uint8 texture, std::string text, Uint8 songnum)
{
	addSlide({ texture,text,songnum });
}

void Cutscene::renderCurrentSlide()
{
	if (mCurrentSlide >= mSlides.size()) //prevent segfault
		return;
	
	mSlideTextures[mSlides[mCurrentSlide].texture]->renderTexture(0, 32);

	gWriter(textbuffers::Small, 6, 168) << txt::White << txt::OptionsOff << txt::TypeOn;//set position of buffer and the default color and options
	gWriter << mSlides[mCurrentSlide].text;//write the text
	LAudio::playMusic(mSlides[mCurrentSlide].songnum);//play the music
	gWriter.ClearBuffer();
}

void Cutscene::handleEvent(SDL_Event & e)
{
	if (e.type == SDL_KEYDOWN && e.key.repeat == 0)//keyboard press
	{
		switch (e.key.keysym.sym)
		{
		case SDLK_SPACE: mSkipPress(); break;
		case SDLK_m: mSkipPress(); break;
		}
	}
	else if (e.type == SDL_CONTROLLERBUTTONDOWN)//controller press
	{
		switch (e.cbutton.button)
		{
		case SDL_CONTROLLER_BUTTON_A: mSkipPress(); break;
		case SDL_CONTROLLER_BUTTON_X: mSkipPress(); break;
		case SDL_CONTROLLER_BUTTON_B: mSkipPress(); break;
		}

	}
}

void Cutscene::setTrigger(LevelID a)
{
	mTrigger = a;
}

bool Cutscene::checkTrigger(LevelID & a)
{
	return (a == mTrigger);
}

bool Cutscene::checkComplete()
{
	return (mCurrentSlide >= mSlides.size()); //if you're beyond the amount of slides
}

void Cutscene::clearCutscene()
{
	mSlides.clear();
}

void Cutscene::mSkipPress()
{
	if (gWriter.getTypePosition() >= mSlides[mCurrentSlide].text.size())//if all the characters have been printed onscreen
	{
		mCurrentSlide++;
		gWriter.ClearTicks();
	}
	else //if the typewriter effect wasn't complete, show all the text
		gWriter.setTypePosition(mSlides[mCurrentSlide].text.size() + 1);
}
