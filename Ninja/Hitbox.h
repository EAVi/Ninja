#ifndef HITBOX_H
#define HITBOX_H

#if defined _WIN32 || _WIN64
#include <SDL.h>
#elif defined __APPLE__ && __MACH__ || defined Macintosh || defined macintosh || defined __linux__
#include <SDL2/SDL.h>
#endif

struct Hitbox
{
	SDL_Rect hitbox;
	int damage;
};

#endif
