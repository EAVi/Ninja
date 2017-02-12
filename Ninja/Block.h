#ifndef BLOCK_H
#define BLOCK_H

#if defined _WIN32 || _WIN64
#include <SDL.h>
#include <SDL_image.h>
#elif defined __APPLE__ && __MACH__ || defined Macintosh || defined macintosh || defined __linux__
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#endif

#include <vector>
#include "LTexture.h"
#include "Algorithms.h"

class Block
{
public:

	static LTexture* mTexture;
	static std::vector<SDL_Rect> kClipRectangles;

	//Default constructor
	Block();

	//Constructor
	Block(int x, int y, int clip = 0);

	//Access X Value
	int getX();

	//Access Y value
	int getY();

	//sets the static *Ltexture mTexture;
	void setAllBlockTextures(LTexture* texture);

	//When you play against me in smash
	SDL_Rect getRect();

	//calls LTexture render function
	void render();

	//overload allowing camera offset
	void render(int x, int y);

	int getClip();

private:
	SDL_Rect mRectangle;
	bool mSolid;
	int mX;
	int mY;
	int mClip;

};


#endif