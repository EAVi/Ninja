#include "TextWriter.h"

using namespace std;

enum textColors : Uint8
{
	kRed, kOrange, kYellow, kGreen, kBlue, kPurple, kWhite, kBlack, kRainbow
};

enum textCommand : Uint8
{
	kOff = 0,
	kSuperImpose = 1,
	kTypewriterStart = 2,
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
	mColors.push_back({ 255, 255, 255 });//white
	mColors.push_back({ 0, 0, 0 });//black

	//fills the rectangle vector
	SDL_Rect clipsize = { 0, 0, 0, 0 };
	mCharClips.clear();
	mTick = 0;
	mTypeSpeed = 3;
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
	mTick = 0;
	mTypeSpeed = 3;
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

	SDL_Point pointy = { x, y };//point where the text will be rendered
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
		else if(clipnum > 127 + kRainbow + 1)//for command assignment
		{
			switch (clipnum - 127 - kRainbow - 2)
			{
			case 1:
				mCommand |= kSuperImpose;
				break;
			case 2://turn off superimpose
				mCommand &= ~kSuperImpose;
				break;
			case 3:
				mCommand |= kTypewriterStart;
				break;
			case 4://turn off the typewriter
				mCommand &= ~kTypewriterStart;
				break;
			default: //Case 0
				mCommand = kOff;
				break;
			}
		}
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
		else//for rendering the font clip
		{
			if (!(mCommand & kTypewriterStart) //if typewriter is off
				|| mTypeMove < mTick/mTypeSpeed)			//or typewriter is on and hasn't advanced too much
			{
				//Superimpose condition
				if (mCommand & kSuperImpose)
				{
					mTexture->colorMod(mColors[kBlack]);//set color to black
					mTexture->renderTexture(pointy.x + 1, pointy.y + 1, &mCharClips[clipnum]);//render the black
					mTexture->colorMod(mColors[mColorNum]);//reset back to previous color, will re-render over the black
				}
				mTexture->renderTexture(pointy.x, pointy.y, &mCharClips[clipnum]);
				pointy.x += mClipDimensions.x;

				/*
				increments the typemove for every character that is drawn
				in typewriter mode
				*/
				if (mCommand & kTypewriterStart)
					mTypeMove++;
			}
		}

	}
	//deactivate commands, to prevent the commands from applying
	//to a buffer or a later renderstring() call
	mCommand = kOff;
	mTypeMove = 0;
}

void TextWriter::RenderString(string text, int x, int y, textColors color, textCommand comm)
{
	mCommand = comm;
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
	//x position doesn't matter, it'll be 0
	int y = kScreenHeight - mClipDimensions.y;//the text will be drawn in the bottom left corner.

	for (int i = 0; i < mWriteBuffer.size(); ++i)
	{
		//count the number of endlines there are, and move the topmost text upwards to accomodate
		if (mWriteBuffer[i] == '\n')
			y -= mClipDimensions.y;
	}

	RenderString(mWriteBuffer, 0, y);//draw it using the renderstring function
	mTick++;//advance the ticks
	mWriteBuffer = "";
}

void TextWriter::ClearTicks()
{
	mTick = 0;
}
