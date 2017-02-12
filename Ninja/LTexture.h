#ifndef LTEXTURE_H
#define LTEXTURE_H

#ifdef _WIN32 || _WIN64
#include <SDL.h>
#include <SDL_image.h>
#elif defined __APPLE__ && __MACH__ || defined Macintosh || defined macintosh || defined __linux__
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#endif

#include <string>
#include <iostream>

class LTexture
{
public:

	static SDL_Renderer* mRenderer;

	//default constructor
	LTexture();
	
	//Deallocates existing textures
	~LTexture();
	

	//Nullifies, destroys the current existing texture
	void freeTexture();

	//overload for debugging
	void freeTexture(bool freeRenderer);

	//Sets the current renderer
	void setRenderer(SDL_Renderer* renderer);
	
	//Takes in a filepath and populates the mTexture
	bool loadTextureFile(std::string filepath, SDL_Color* colorkey = NULL);

	//Checks if file is loaded
	bool checkLoaded();

	//Draws to renderer
	void renderTexture(int x, int y, SDL_Rect* crop = NULL, double angle = 0.0, SDL_Point* point = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE, int* width = NULL, int* height = NULL);

	//Applies color modulation to texture
	void colorMod(Uint8 red, Uint8 green, Uint8 blue);

	//Overload allowing a color struct rather than individual colors
	void colorMod(SDL_Color& color);

	//Applies alpha value to texture
	void setAlpha(Uint8 alpha);

	//Sets the texture blend mode
	void setBlending(SDL_BlendMode blend);

	//Access texture width
	int getWidth();

	//Access texture height
	int getHeight();

private:

	SDL_Texture* mTexture;
	int mWidth;
	int mHeight;
};


#endif