#include "TextWriter.h"

enum textColors : Uint8
{
	kRed, kOrange, kYellow, kGreen, kBlue, kPurple, kWhite, kBlack, kRainbow
};


TextWriter::TextWriter()
{
	mTexture = NULL;
	mAllText = "";
	mClipDimensions = { 0,0 };

	mColorNum = 0;
	mColors.push_back({ 255, 0, 0 });//red
	mColors.push_back({ 255, 127, 0 });//orange
	mColors.push_back({ 255, 255, 0 });//yellow
	mColors.push_back({ 0, 255, 0 });//green
	mColors.push_back({ 0, 0, 255 });//blue
	mColors.push_back({ 255, 0, 255 });//purple

	//fills the rectangle vector
	SDL_Rect clipsize = { 0, 0, 0, 0 };
	mCharClips.clear();
}

TextWriter::TextWriter(LTexture* texture, int w, int h)
{
	mTexture = texture;
	mAllText = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789. ";
	mClipDimensions = { w, h };

	mColorNum = 0;
	mColors.push_back({ 255, 0, 0 });//red
	mColors.push_back({ 255, 112, 0 });//orange (same shade as ninja)
	mColors.push_back({ 255, 255, 0 });//yellow
	mColors.push_back({ 0, 255, 0 });//green
	mColors.push_back({ 0, 0, 255 });//blue
	mColors.push_back({ 255, 0, 255 });//purple
	mColors.push_back({ 255, 255, 255 });//white
	mColors.push_back({ 0, 0, 0 });//black

	
	//fills the rectangle vector
	SDL_Rect clipsize = { 0, 0, w, h };
	mCharClips.clear();
	mCharClips = spriteClipper(mTexture->getWidth(),mTexture->getHeight(),clipsize);
}

TextWriter::~TextWriter()
{
	mTexture = NULL;
}

void TextWriter::RenderString(string text, int x, int y, SDL_Color* color)
{
	if (color == NULL)
		mColorNum = 0;

	if (color != NULL)
		this->mTexture->colorMod(*color);

	SDL_Point pointy = { x, y };
	for (Uint16 i = 0, j = text.size(); i < j; ++i)
	{
		if (color == NULL)//the rainbow loop
		{
			if (mColorNum > kPurple)
				mColorNum = 0;
			this->mTexture->colorMod(mColors[mColorNum]);
			mColorNum++;
		}
		int clipnum = cToInt(text[i]);
		if (clipnum != 64)
			mTexture->renderTexture(pointy.x, pointy.y, &mCharClips[clipnum]);
		pointy.x += mClipDimensions.x;
	}
}

void TextWriter::RenderString(string text, int x, int y, textColors color)
{
	if (color != kRainbow)
		this->RenderString(text, x, y, &mColors[color]);
	else //if color is rainbow
		this->RenderString(text, x, y);
}

void TextWriter::RenderString(double val, int x, int y, SDL_Color* color)
{
	this->RenderString(to_string(val), x, y, color);
}

void TextWriter::RenderString(int val, int x, int y, SDL_Color* color)
{
	this->RenderString(to_string(val), x, y, color);
}


Uint8 TextWriter::cToInt(char a)
{
	for (Uint8 i = 0, j = mAllText.size(); i < j ; ++i)
	{
		if (mAllText[i] == a)
			return i;
	}
	return 64;
}