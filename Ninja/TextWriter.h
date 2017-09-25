#ifndef TEXTWRITER_H
#define TEXTWRITER_H

#if defined _WIN32 || _WIN64
#include <SDL.h>
#include <SDL_image.h>
#elif defined __APPLE__ && __MACH__ || defined Macintosh || defined macintosh || defined __linux__
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#endif
#include <string>
#include <sstream>
#include <vector>
#include <queue>
#include <cstdlib>
#include "LTexture.h"
#include "Algorithms.h"

extern const int kScreenWidth;//the global screen width and height is needed for some functions
extern const int kScreenHeight;

enum textColors : Uint8;
enum textCommand : Uint8;


namespace txt
{
	/*
	a character enumeration does not work, so using a bunch of separate characters
	another approach would use typecasting, and I don't want to do that
	*/
	static char Red = '\x80';
	static char Orange = '\x81';
	static char Yellow = '\x82';
	static char Green = '\x83';
	static char Blue = '\x84';
	static char Purple = '\x85';
	static char White = '\x86';
	static char Black = '\x87';
	static char Rainbow = '\x88';
	static char OptionsOff = '\x89';
	static char SuperOn = '\x8A';
	static char SuperOff = '\x8B';
	static char TypeOn = '\x8C';
	static char TypeOff = '\x8D';
	static char HLightOn = '\x8E';
	static char HLightOff = '\x8F';
}

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
	void RenderString(std::string text, int x, int y, SDL_Color* color = NULL);

	//-Renders string at position, overload using textcolors
	void RenderString(std::string text, int x, int y, textColors color, textCommand comm = (textCommand)0);

	//-overload using double
	void RenderString(double val, int x, int y, SDL_Color* color = NULL);

	//-overload using ints
	void RenderString(int val, int x, int y, SDL_Color* color = NULL);

	/*
	*Allows the Writer to act like the "cout <<" object function,
	*Note that this will only throw a bunch of strings onto the buffer,
	*it will only render the strings in the buffer when the function ClearBuffer() is called
	Example usage:
		TextWriter gout;
		gout << "Magnar is #" << 1 << " at Melee";
		gout.ClearBuffer();
	*/
	template<typename T>
	TextWriter & operator<< (const T & x)
	{
		std::stringstream s;
		s << x;
		mWriteBuffer += s.str();
		return *this;
	}
	
	/*
	Writes all the strings on the mWriteBuffer queue from top to bottom
	(front-to-back of queue) while clearing the buffer
	Will update the function after the char system is improved
	*/
	void ClearBuffer();

	/*
	Empties the ticks, clearing the screen of text if typewriter is on
	*/
	void ClearTicks();

private:
	LTexture* mTexture;
	Uint8 mColorNum;
	Uint8 mCommand;
	std::vector<SDL_Color> mColors;
	std::vector<SDL_Rect> mCharClips;
	SDL_Point mClipDimensions;
	std::string mWriteBuffer;
	int mTick;//number of times that clearbuffer has been called
	int mTypeMove;//number of characters that have been rendered past the current character
	Uint8 mTypeSpeed;
};

#endif