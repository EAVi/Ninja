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

void Cutscene::addSlide(Uint8 texture, std::string text)
{
	addSlide({ texture,text });
}

void Cutscene::renderCurrentSlide()
{
	if (mCurrentSlide >= mSlides.size()) //prevent segfault
		return;
	
	mSlideTextures[mSlides[mCurrentSlide].texture]->renderTexture(0, 32);
	gWriter.RenderString(mSlides[mCurrentSlide].text, 0, 160);
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
	mCurrentSlide++;
	gWriter.ClearTicks();
}
