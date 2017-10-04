#ifndef CUTSCENE_H
#define CUTSCENE_H

#include "TextWriter.h"

/*
A Cutscene will be made up of slides
A slide will be one picture and text
*/


struct Slide
{
	Uint8 texture; //the texture number for the image
	std::string text; //the text, the speech, the words
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
	void addSlide(Uint8 texture, std::string text);

	//renders the texture and writes the text
	void renderCurrentSlide();

	//Event handler allowing skipping
	void handleEvent(SDL_Event & e);

	//Returns whether the last slide has been skipped
	bool checkComplete();

	//Empty the slides
	void clearCutscene();
private:
	std::vector<Slide> mSlides;
	Uint8 mCurrentSlide;
	void mSkipPress();
};

#endif
