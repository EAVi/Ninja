#ifndef LAUDIO_H
#define LAUDIO_H

#ifdef _WIN32
#include <vld.h>//visual leak detector comment this out if you do not have, it's pretty good
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h> //These libraries are unused
/*
#include <SDL_ttf.h>
*/
#elif defined __APPLE__ && __MACH__ || defined Macintosh || defined macintosh || defined __linux__
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h> //These libraries are unused
/*
#include <SDL2/SDL_ttf.h>
*/
#endif

#include <iostream>
#include <string>
#include <vector>

/*
Unlike LTexture, which has a separate object per texture, LAudio
has one object which is the hub for all sounds, which is pretty convenient.
It allows any game object to trigger any of the samples or musics
*/

class LAudio
{
public:
	LAudio();

	//Plays a song of given mMusic index
	//*arg 1 is the index
	//*arg 2 is number of loops
	//	-1 will loop indefinitely
	//	0 will only play once
	void playMusic(Uint8 a, int loop = -1);

	//Plays a sound of given mSound index
	void playSound(Uint8 a, int loop = 0);

	//Loads a given song or sound and throws it onto respective vector
	bool loadSFX(std::string a, bool isSong = false);

	//free all memory regarding music
	void release();

	static Uint8 mChannels;
private:
	std::vector<Mix_Music*> mMusic;
	std::vector<Mix_Chunk*> mSound;
	Uint8 mCurrentSong;
};

#endif
