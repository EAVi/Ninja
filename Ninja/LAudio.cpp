#include "LAudio.h"

using namespace std;

Uint8 LAudio::mChannels = 16;
std::vector<Mix_Music*> LAudio::mMusic = vector<Mix_Music*>();
std::vector<Mix_Chunk*> LAudio::mSound = vector<Mix_Chunk*>();
Uint8 LAudio::mCurrentSong = 255;

LAudio::LAudio()
{
	mMusic.clear();
	mSound.clear();
	mCurrentSong = 255;
}

void LAudio::playMusic(Uint8 a, int loop)
{
	if (mCurrentSong == a)//if song is already playing
		return;

	//check index to prevent segfault
	if (a >= mMusic.size())
	{
		cout << "Music index out of range\n";
		cout << "mMusic.size(): " << mMusic.size() << "\tIndex: " << (int)a << endl;
		return;
	}
	//the function will return the channel which the music is played
	//if channel is -1, there was an allocation error
	if (Mix_PlayMusic(mMusic[a], loop) == -1)
	{
		cout << "Warning, Music channel failure for mMusic index " << a;
	}
	else
	{
		mCurrentSong = a;
	}
}

void LAudio::playSound(Uint8 a, int loop)
{
	//check index to prevent segfault
	if (a >= mSound.size())
	{
		cout << "Sound index out of range\n";
		cout << "mSound.size(): " << mMusic.size() << "\tIndex: " << (int)a << endl;
		return;
	}
	//the function will return the channel which the sound is played
	//if channel is -1, there was an allocation error
	if (Mix_PlayChannel(-1, mSound[a], loop) == -1)
	{
		cout << "Warning, Music channel failure for mMusic index " << a;
	}
}

bool LAudio::loadSFX(string a, bool isSong)
{
	/*
	SDL2 Mixer uses different types for sounds and music
	they both pretty much do the same thing, except different priorities 
	and all that. Music will only occupy one channel and will have high priority
	while sounds can overlap and will not play if there isn't an available channel
	*/
	if (isSong)
	{
		Mix_Music *temp = NULL;
		temp = Mix_LoadMUS(a.c_str());
		if (temp == NULL)
			cout << "Failed to load Music file " << a << endl;
		mMusic.push_back(temp);
	}
	else
	{
		Mix_Chunk *temp = NULL;
		temp = Mix_LoadWAV(a.c_str());
		if (temp == NULL)
			cout << "Failed to load Sound file" << a << endl;
		mSound.push_back(temp);
	}
	return true;
}

void LAudio::release()
{
	for (int i = 0, j = mMusic.size(); i < j; ++i)
	{
		Mix_FreeMusic(mMusic[i]);
	}
	for (int i = 0, j = mSound.size(); i < j; ++i)
	{
		Mix_FreeChunk(mSound[i]);
	}
}
