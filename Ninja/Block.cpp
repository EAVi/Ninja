#include "Block.h"

using namespace std;

LTexture* Block::mTexture = NULL;
vector<SDL_Rect> Block::kClipRectangles = spriteClipper(256, 256, {0,0,16,16});


Block::Block()
{
	mX = 0;
	mY = 0;
	mRectangle = { mX, mY, 16, 16 };
	mClip = 255;
}

Block::Block(int x, int y, int clip)
{
	mX = x;
	mY = y;
	mRectangle = { x, y, 16, 16 };
	mClip = clip;
}

int Block::getX()
{
	return mX;
}

int Block::getY()
{
	return mY;
}

SDL_Rect Block::getRect()
{
	return mRectangle;
}

void Block::render()
{
	mTexture->renderTexture(mX, mY);
}

void Block::render(int x, int y)
{
	mTexture->renderTexture(mX - x, mY - y, &kClipRectangles[mClip]);
}

int Block::getClip()
{
	return mClip;
}

void Block::setAllBlockTextures(LTexture* texture)
{
	mTexture = texture;
}