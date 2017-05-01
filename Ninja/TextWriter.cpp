#include "TextWriter.h"

using namespace std;

enum textColors : Uint8
{
	kRed, kOrange, kYellow, kGreen, kBlue, kPurple, kWhite, kBlack, kRainbow
};


TextWriter::TextWriter()
{
	mTexture = NULL;
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
	if (color == NULL)//this initializes the colornum to 0, because NULL means RAINBOW TIME BAYBEEE
		mColorNum = 0;

	if (color != NULL)//if non-NULL, then it will do the color you put
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

		Uint8 clipnum = (Uint8)(text[i]);
		
		if (clipnum == (int)'\n')//NEWLINE symbol
		{
			pointy.x = x;
			pointy.y += mClipDimensions.y;
		}
		/*
		To assign a color is a weird process, the best way (in my opinion) is to use a hex value escape character
		These hex values begin at x80
		For example:
			<< (char)0x80 << "RED";
			<< '\x80'+(string)"RED";
		You can't have it all in one string unless you put a space after the escape character
		*/
		else if (clipnum > 127)// for color assignment
		{
			mColorNum = (clipnum - 128) % (mColors.size() + 1);
			if (mColorNum == kRainbow)
			{
				color = NULL;//nullifies the color, allowing rainbow mode
				mColorNum = kRed;
			}
			else
				color = &mColors[0];//just gives color a value to prevent rainbowing
			this->mTexture->colorMod(mColors[mColorNum]);
		}
		else
		{
			mTexture->renderTexture(pointy.x, pointy.y, &mCharClips[clipnum]);
			pointy.x += mClipDimensions.x;
		}

		
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


void TextWriter::ClearBuffer()
{
	string s;
	int y = kScreenHeight - mClipDimensions.y;
	while (!mWriteBuffer.empty())
	{
		s += mWriteBuffer.front();
		mWriteBuffer.pop();
	}
	for (int i = 0; i < s.size(); ++i)
	{
		if (s[i] == '\n')
			y -= mClipDimensions.y;
	}

	RenderString(s, 0, y);


	/*
	//outdated version of the function
	RenderString(*wb, 0, y);
	string * wb;
	int y = kScreenHeight - 8 * mWriteBuffer.size();
	//int i = mWriteBuffer.size();
	while (!mWriteBuffer.empty())
	{
		wb = &mWriteBuffer.front();
		RenderString(*wb, 0, y);
		mWriteBuffer.pop();
		y += 8;
		//i = mWriteBuffer.size();
	}
	*/
}