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

	//note that there is no option for large text, you'll need 
	//to set the current buffer to the Large buffer
	//using the operator()
}

enum textbuffers : Uint8
{
	Debug = 0,//the debug buffer will always be oriented to the bottom-left corner of the screen
	Small = 1,//the small buffer is just for small text, non-debug purposes
	Large = 2,//the large buffer is for big text, intended for menus and stuff
	TotalBuffers = 3,
};

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
	TextWriter(LTexture* texture, int w, int h, LTexture* largetexture, int wl, int hl);

	//-Renders string at position
	void RenderString(std::string text, int x, int y, SDL_Color* color = NULL, bool large = false);

	//-Renders string at position, overload using textcolors
	void RenderString(std::string text, int x, int y, textColors color, textCommand comm = (textCommand)0);

	//-overload using double
	void RenderString(double val, int x, int y, SDL_Color* color = NULL);

	//-overload using ints
	void RenderString(int val, int x, int y, SDL_Color* color = NULL);

	/*
	*Allows the Writer to act like the "cout <<" object function,
	*Note that this will only throw a bunch of strings onto the buffer, without rendering them
	*To render the strings in the buffer, use ClearBuffer();
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
		mWriteBuffer[mCurrentBuffer] += s.str();
		return *this;
	}

	/*
	set the buffer which strings will be written to
	it may also be followed by a << operator
	-parameter 1: set the buffer
	-parameter 2: set the x position of the buffer (will not affect Debug)
	-parameter 3: set the y position of the buffer (will not affect Debug)
	Example usage:
		TextWriter gout;
		gout(textbuffers::Large, 8, 8) << "I'm Large\n";
		gout << "Still Large\n";
		gout.ClearBuffer();
	*/
	TextWriter & operator() (textbuffers a, int x = 0, int y = 0);

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

	/*
	Get the number of characters printed in the previous frame
	*/
	int getTypePosition();

	/*
	Set the ticks such that a given number of characters are printed
	*/
	void setTypePosition(int a);


private:
	LTexture* mTexture;
	LTexture* mLargeTexture;
	textbuffers mCurrentBuffer; //the textbuffers enum is Uint8
	Uint8 mColorNum;
	Uint8 mCommand;
	std::vector<SDL_Color> mColors;
	std::vector<SDL_Rect> mCharClips;//the character clippings for the small characters
	std::vector<SDL_Rect> mLargeCharClips;//larger characters
	SDL_Point mClipDimensions;//smaller characters
	SDL_Point mLargeClipDimensions;//larger characters
	std::vector<SDL_Point> mBufferPosition;
	std::vector<std::string> mWriteBuffer;
	int mTick;//number of times that clearbuffer has been called
	int mTypeMove;//number of characters that have been rendered past the current character
	Uint8 mTypeSpeed;

	//Get position such that the lowest character of the string touches the bottom of the screen
	SDL_Point mDebugPosition(std::string& s);
};

#endif