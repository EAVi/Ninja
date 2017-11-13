#ifndef ZONE_H
#define ZONE_H

#if defined _WIN32 || _WIN64
#include <SDL.h>
#elif defined __APPLE__ && __MACH__ || defined Macintosh || defined macintosh || defined __linux__
#include <SDL2/SDL.h>
#endif
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include "LTexture.h"
#include "Level.h"
#include "Algorithms.h"
#include "Hitbox.h"
#include "Enemy.h"
#include "Robot.h"
#include "Player.h"
#include "MagMatrix.h"

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

class Zone
{
public:
	/* Zone constructor*/
	Zone();

	/*Zone overload setting the camera*/
	Zone(SDL_Rect* camera, Player* player, std::string prefix, std::string suffix, std::vector<LTexture*>& textures);

	/* This deconstructor will make sure the levels are destroyed before the zone is*/
	~Zone();

	/*Initializes the vector of levels, make sure to call this after setting the necessary values*/
	void init();

	/* Set the string prefix which the levels will be loaded by
	* For example "zone01" would be a good prefix, and you'll have to have the files
	* "zone01-0"... and ongoing until out of levels*/
	void setPrefix(std::string s);

	/* Same as prefix, but the file extension and other crap ".txt"*/
	void setSuffix(std::string s);

	/* If the level isn't already loaded, load the currentlevel
	* This function is useless so far, but I'm keeping it just in case*/
	void loadLevel();

	/* Overload allowing you to load a specific level
	* Also useless, but I'm keeping it*/
	void loadLevel(int i);

	/* When the Ninja needs to switch levels, this function will need to be called*/
	void setLevel(int i);

	/* Returns the current level number*/
	Uint8 getCurrentLevel();

	/* Call stepEnemies for the curent level
	returns true if the player has switched level*/
	bool stepEnemiesCurrentLevel();

	/* Call clearHitboxes for current level*/
	void clearHitboxesCurrentLevel();

	/* Will render what the current level is*/
	void renderCurrentLevel();

	/* Draw enemies in the current level*/
	void drawEnemiesCurrentLevel();

	/* Render the hitboxed of the current level*/
	void debugShowHitboxesCurrentLevel(SDL_Renderer& r);

	/* Return LevelCount*/
	int getLevelCount();

	/* Set mPlayer*/
	void setPlayer(Player* a);

	/* Set mCameraof current level*/
	void setCamera(SDL_Rect* camera);

	/* Loads every level in the zone*/
	void loadZone();

	/* Sets the BGTextures that will be used by the levels*/
	void setBGTextures(std::vector<LTexture*>& textures);

	/* Sets the Door textures that will be used by the levels*/
	void setDoorTextures(std::vector<LTexture*>& textures);

	/* Returns the mLevel.size()*/
	Uint8 getLevelSize();

	/* Clears the vector of maps*/
	void release();

	/* Delete allocated memory and reload the maps*/
	void reloadZone();

	/* Returns enemyCount() of current level*/
	Uint8 enemyCountCurrentLevel();

	/* checks for the spawn point and spawn the player there*/
	void setSpawn();

	/* Checks if the player has collided with any door, and if the player has, warp them to another dimension*/
	bool mCheckDoors();

	/* Returns the curren level's song*/
	Uint8 getSongCurrentLevel();

	/* Returns the current zone and level number, e.g. 1,1*/
	LevelID getLevelID();

	/* Sets value for mZoneID*/
	void setZoneID(Uint8 a);

private:
	std::vector<Level> mLevels;
	int mSize;
	bool mInit;
	Uint8 mCurrentLevel;
	Uint8 mZoneID;
	std::string mPrefix;
	std::string mSuffix;
	Player* mPlayer;
	Uint8 mLevelCount;
	SDL_Rect* mCamera;
	std::vector<LTexture*> mBGTextures;
	std::vector<LTexture*> mDoorTextures;



	/* Counts the number of files in your folders with the prefix and suffix*/
	void countFiles();
};

#endif