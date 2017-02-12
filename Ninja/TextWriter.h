#ifndef TEXTWRITER_H
#define TEXTWRITER_H

#ifdef _WIN32 || _WIN64
#include <SDL.h>
#include <SDL_image.h>
#elif defined __APPLE__ && __MACH__ || defined Macintosh || defined macintosh || defined __linux__
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#endif
#include <string>
#include <vector>
#include <cstdlib>
#include "LTexture.h"
#include "Algorithms.h"

using namespace std;

enum textColors : Uint8;

class TextWriter
{
public:
	//constant array size
	static const int kASize = 64;

	//default constructor
	TextWriter();

	//Deconstructor
	~TextWriter();

	//-Overload with LTexture, and dimensions for clipping
	TextWriter(LTexture* texture, int w, int h);

	//-Renders string at position
	void RenderString(string text, int x, int y, SDL_Color* color = NULL);

	//-Renders string at position, overload using textcolors
	void RenderString(string text, int x, int y, textColors color);

	//-overload using double
	void RenderString(double val, int x, int y, SDL_Color* color = NULL);

	//-overload using ints
	void RenderString(int val, int x, int y, SDL_Color* color = NULL);



	//returns the array address of the integer
	Uint8 cToInt(char a);

private:
	LTexture* mTexture;
	Uint8 mColorNum;
	vector<SDL_Color> mColors;
	string mAllText;
	vector<SDL_Rect> mCharClips;
	SDL_Point mClipDimensions;
};

#endif