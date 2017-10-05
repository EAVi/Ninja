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
	kTypewriter = 2,
	kHighlight = 4,
	kJiggle = 8
};



TextWriter::TextWriter() 
	: TextWriter(NULL, 0, 0, NULL, 0, 0)
{
}

TextWriter::TextWriter(LTexture* texture, int w, int h, LTexture* largetexture, int wl, int hl)
{
	mCurrentBuffer = Debug;//default to buffer 0

	mTexture = texture;
	mLargeTexture = largetexture;

	mClipDimensions = { w, h };
	mLargeClipDimensions = {wl, hl};

	mColorNum = 0;
	mColors.push_back({ 255, 0, 0 });//red
	mColors.push_back({ 255, 112, 0 });//orange (same shade as ninja)
	mColors.push_back({ 255, 255, 0 });//yellow
	mColors.push_back({ 0, 255, 0 });//green
	mColors.push_back({ 0, 0, 255 });//blue
	mColors.push_back({ 255, 0, 255 });//purple
	mColors.push_back({ 255, 255, 255 });//white
	mColors.push_back({ 0, 0, 0 });//black

	mWriteBuffer = vector<string>(TotalBuffers);
	mBufferPosition = vector<SDL_Point>(TotalBuffers);

	
	//fills the rectangle vector
	SDL_Rect clipsize = { 0, 0, w, h };
	mCharClips.clear();
	if( (w > 0 ) && (h > 0))
		mCharClips = spriteClipper(mTexture->getWidth(),mTexture->getHeight(),clipsize);

	clipsize = { 0, 0, wl, hl };
	mLargeCharClips.clear();
	if ((wl > 0) && (hl > 0))
		mLargeCharClips = spriteClipper(mLargeTexture->getWidth(), mLargeTexture->getHeight(), clipsize);

	mTick = 0;
	mTypeSpeed = 3;
}

TextWriter::~TextWriter()
{
	mTexture = NULL;
}

void TextWriter::RenderString(string text, int x, int y, SDL_Color* color, bool large)
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
			if (!large)
				pointy.y += mClipDimensions.y;
			else 
				pointy.y += mLargeClipDimensions.y;
		}
		/*
		There are a few ways to assign colors, the newest best way is to use the txt namespace
		For example:
			<< txt::Red << "Red";
		The other ways involve using hex escape characters.
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
				mCommand |= kTypewriter;
				break;
			case 4://turn off the typewriter
				mCommand &= ~kTypewriter;
				break;
			case 5:
				mCommand |= kHighlight;
				break;
			case 6://turn off the highlight
				mCommand &= ~kHighlight;
				break;
			case 7:
				mCommand |= kJiggle;
				break;
			case 8://turn off the jiggles
				mCommand &= ~kJiggle;
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
			this->mLargeTexture->colorMod(mColors[mColorNum]);
		}
		else//for rendering the font clip
		{
			if (!(mCommand & kTypewriter) //if typewriter is off
				|| mTypeMove < mTick/mTypeSpeed)			//or typewriter is on and hasn't advanced too much
			{
				//Superimpose condition or highligh condition
				if (mCommand & (kSuperImpose | kHighlight))
				{
					if (mCommand & kSuperImpose)
					{
						mTexture->colorMod(mColors[kBlack]);//set color to black
						mLargeTexture->colorMod(mColors[kBlack]);//set color to black
					}
					else//highlight
					{
						mTexture->colorMod(mColors[kWhite]);//set color to white
						mLargeTexture->colorMod(mColors[kWhite]);//set color to white
					}

					if(!large)
						mTexture->renderTexture(pointy.x + 1, pointy.y + 1, &mCharClips[clipnum]);//render the black or white
					else
						mLargeTexture->renderTexture(pointy.x + 2, pointy.y + 2, &mLargeCharClips[clipnum]);//render the black or white
					mTexture->colorMod(mColors[mColorNum]);//reset back to previous color, will re-render over the black
					mLargeTexture->colorMod(mColors[mColorNum]);//reset back to previous color, will re-render over the black
				}
				//render a single character
				if (!large)
					mTexture->renderTexture(pointy.x, pointy.y, &mCharClips[clipnum]);
				else 
					mLargeTexture->renderTexture(pointy.x, pointy.y, &mLargeCharClips[clipnum]);

				//move position to the right
				if (!large)
					pointy.x += mClipDimensions.x;
				else
					pointy.x += mLargeClipDimensions.x;

				/*
				increments the typemove for every character that is drawn
				in typewriter mode
				*/
				if (mCommand & kTypewriter)
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


TextWriter & TextWriter::operator()(textbuffers a, int x, int y)
{
	//set the current buffer
	mCurrentBuffer = a;

	//set the position for the selected buffer
	mBufferPosition[a] = { x, y };

	//return reference to current object, in order to allow << operator afterwards
	return *this;
}

void TextWriter::ClearBuffer()
{
	//set the position for the debug buffer
	mBufferPosition[Debug] = mDebugPosition(mWriteBuffer[Debug]);

	//render every buffer (string)
	for (int i = 0, j = (int)TotalBuffers; i < j; ++i)
	{
		RenderString(mWriteBuffer[i], mBufferPosition[i].x, mBufferPosition[i].y, NULL, i == Large);//draw it using the renderstring function
	}

	mTick++;//advance the ticks

	for (int i = 0, j = (int)TotalBuffers; i < j; ++i)
		mWriteBuffer[i] = "";
}

void TextWriter::ClearTicks()
{
	mTick = 0;
}

int TextWriter::getTypePosition()
{
	return mTick / mTypeSpeed;
}

void TextWriter::setTypePosition(int a)
{
	mTick = mTypeSpeed * a;
}

SDL_Point TextWriter::mDebugPosition(std::string & s)
{
	//x position doesn't matter, it'll be 0
	int y = kScreenHeight - mClipDimensions.y;//the text will be drawn in the bottom left corner.

	for (int i = 0; i < mWriteBuffer[Debug].size(); ++i)
	{
		//count the number of endlines there are, and move the topmost text upwards to accomodate
		if (mWriteBuffer[Debug].at(i) == '\n')
			y -= mClipDimensions.y;
	}
	
	return { 0,y };
}
