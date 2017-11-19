#include "LTexture.h"

using namespace std;

SDL_Renderer* LTexture::mRenderer = NULL;

LTexture::LTexture()
{
	this->mTexture = NULL;
	this->mWidth = 0;
	this->mHeight = 0;
}

LTexture::~LTexture()
{
	this->freeTexture();
}

void LTexture::freeTexture()
{
	if (this->mTexture == NULL)
		return;
	else
	{
		SDL_DestroyTexture(this->mTexture);
		this->mTexture = NULL;
		this->mWidth = 0;
		this->mHeight = 0;
		//mRenderer = NULL;
	}
}
void LTexture::freeTexture(bool freeRenderer)
{
	if (this->mTexture == NULL)
		return;
	else
	{
		SDL_DestroyTexture(this->mTexture);
		this->mTexture = NULL;
		this->mWidth = 0;
		this->mHeight = 0;
		if (freeRenderer)
			mRenderer = NULL;
	}
}

void LTexture::setRenderer(SDL_Renderer* renderer)
{
	//if (renderer == NULL) return;
	//else
	this->mRenderer = renderer;
}

bool LTexture::loadTextureFile(string filepath, SDL_Color* colorkey)
{
	this->freeTexture(false);
	SDL_Texture* loadedTexture = NULL;
	SDL_Surface* tempSurface = IMG_Load(filepath.c_str());//intermediate surface needed to load texture
	if (tempSurface == NULL)//tempsurface failure
	{
		cout << filepath << "\nSurface loading failure\n" << IMG_GetError << endl;
		return false;
	}
	
	if (colorkey != NULL) //only applies a color key if it was passed
	{
		Uint8 red, green, blue;
		red = colorkey->r;
		green = colorkey->g;
		blue = colorkey->b;
		SDL_SetColorKey(tempSurface, SDL_TRUE, SDL_MapRGB(tempSurface->format, red, green, blue));
	}//null colorkey

	this->mWidth = tempSurface->w;
	this->mHeight = tempSurface->h;
	loadedTexture = SDL_CreateTextureFromSurface(this->mRenderer,tempSurface);//convert to texture
	SDL_FreeSurface(tempSurface);//intermediate surface gone, bye bye
	
	if (loadedTexture == NULL)//conversion failure
	{
		cout << filepath << "\nFailed to convert to Texture\n" << IMG_GetError << endl;
		return false;
	}//conversion failure
	
	this->mTexture = loadedTexture;
	return (this->mTexture != NULL);
}

bool LTexture::checkLoaded()
{
	return (this->mTexture != NULL);
}

void LTexture::renderTexture(int x, int y, SDL_Rect* crop, double angle, SDL_Point* point, SDL_RendererFlip flip, int* width, int* height)
{
	//The target area that the texture will be rendered. 
	//If you make the area bigger, the texture will cover more of the screen.
	SDL_Rect area = { x, y, this->mWidth, this->mHeight};


	//if stretching parameters are given, then stretch the image
	if (width != NULL && height != NULL)
	{
		area.w = *width;
		area.h = *height;
	}
	
	//if a crop was passed, you don't want stretching
	if (crop != NULL)
	{
		area.w = crop->w;
		area.h = crop->h;
	}

	//then render it all
	SDL_RenderCopyEx(this->mRenderer, this->mTexture, crop, &area, angle, point, flip);
}

void LTexture::colorMod(Uint8 red, Uint8 green, Uint8 blue)
{
	SDL_SetTextureColorMod(this->mTexture, red, green, blue);
}

void LTexture::colorMod(SDL_Color& color)
{
	this->colorMod(color.r, color.g, color.b);
}

void LTexture::setAlpha(Uint8 alpha)
{
	SDL_SetTextureAlphaMod(this->mTexture, alpha);
}

void LTexture::setBlending(SDL_BlendMode blend)
{
	SDL_SetTextureBlendMode(this->mTexture, blend);
}

int LTexture::getWidth()
{
	return this->mWidth;
}

int LTexture::getHeight()
{
	return this->mHeight;
}