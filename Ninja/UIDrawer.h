#ifndef UIDRAWER_H
#define UIDRAWER_H

#if defined _WIN32 || _WIN64
#include <SDL.h>
#include <SDL_image.h>
#elif defined __APPLE__ && __MACH__ || defined Macintosh || defined macintosh || defined __linux__
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#endif
#include <vector>
#include <string>
#include "LTexture.h"
#include "TextWriter.h"

extern std::vector<LTexture*> gUITextures; //global textures defined in main.cpp
extern TextWriter gWriter;//global writer defined in main.cpp

enum UITextureNums : Uint8;

namespace Healthbar{};

class UIDrawer
{
public:
	UIDrawer();
	void drawHealthbar(int& health, int& maxhealth, int& lives)const;

private:

};




#endif