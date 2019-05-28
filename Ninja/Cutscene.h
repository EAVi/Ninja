#ifndef CUTSCENE_H
#define CUTSCENE_H

#include "TextWriter.h"
#include "Zone.h"//for LevelID
#include "Laudio.h"

/*
A Cutscene will be made up of slides
A slide will be one picture and text
*/

struct Slide
{
	Uint8 texture; //the texture number for the image
	std::string text; //the text, the speech, the words
	Uint8 songnum;//optionally trigger a song
};


class Cutscene
{
public:
	static std::vector<LTexture*> mSlideTextures;

	//Constructor
	Cutscene();

	//Add a slide to the mSlide vector
	void addSlide(Slide a);

	//Overload using member variables
	void addSlide(Uint8 texture, std::string text, Uint8 songnum = 254);

	//renders the texture and writes the text
	void renderCurrentSlide();

	//Event handler allowing skipping
	void handleEvent(SDL_Event & e);

	//sets the trigger for the cutscene, eg "the cutscene will play at zone 1 level 3"
	void setTrigger(LevelID a);

	bool checkTrigger(LevelID& a);

	//Returns whether the last slide has been skipped
	bool checkComplete();

	//Empty the slides
	void clearCutscene();
private:
	std::vector<Slide> mSlides;
	Uint8 mCurrentSlide;
	void mSkipPress();
	LevelID mTrigger;
};

#endif

