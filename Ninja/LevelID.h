#ifndef LEVELID_H
#define LEVELID_H

#if defined _WIN32 || _WIN64
#include <SDL.h>
#elif defined __APPLE__ && __MACH__ || defined Macintosh || defined macintosh || defined __linux__
#include <SDL2/SDL.h>
#endif

/*
this structure will return the current level number and zone number
zone 7, level 1 will be represented as {1,7}
*/
struct LevelID
{
	Uint8 LevelNo;
	Uint8 ZoneNo;//255 will represent debug
};

bool operator== (LevelID& a, LevelID& b);

#endif