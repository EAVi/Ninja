#ifndef LEVEL_H
#define LEVEL_H

#if defined _WIN32 || _WIN64
#include <SDL.h>
#elif defined __APPLE__ && __MACH__ || defined Macintosh || defined macintosh || defined __linux__
#include <SDL2/SDL.h>
#endif
#include <vector>
#include <string>
#include <fstream>
#include "Block.h"
#include "LTexture.h"
#include "Algorithms.h"
#include "Hitbox.h"
#include "Enemy.h"
#include "MagMatrix.h"


struct Background
{
	Uint8 TextNum;
	Sint16 initX;
	Sint16 initY;
	Uint8 depth;
	bool tileX;
	bool tileY;
};

inline bool operator==(const Background& a, const Background& b)
{
	return (a.TextNum == b.TextNum &&
		a.initX == b.initX &&
		a.initY == b.initY &&
		a.depth == b.depth &&
		a.tileX == b.tileX &&
		a.tileY == b.tileY);
}

class Level
{
public:
	static int tilesize;

	//-Default constructor
	Level();

	//-Constructor with dimensions
	Level(int width, int height, SDL_Rect* camera);

	//-Deconstructor, destroys all enemies
	~Level();

	//-Returns a rectangle of the level's dimensions
	SDL_Rect getLevelDimensions();

	//-Loads map from file, will optimize later
	bool Loadmap(std::string filename);


	//std::vector<Block> getBlocks();//pointless/outdated function

	//returns all collision boxes
	std::vector<SDL_Rect>& getRects();

	//-Adds more blocks to the map
	void setBlocks(Block block);

	//-Overload with vectors
	//-this function is useless outside of debug, by the way
	void setBlocks(std::vector<Block> blocks);

	//-Renders every inidividual block
	void renderLevel();

	//-A color that will be set to be used for color modulation
	//-Note that it doesn't actually set the color modulation 
	void setAmbientLight(SDL_Color color);

	//-Accessor to the ambient light
	SDL_Color getAmbientLight();

	//-Corrects the camera's position so it doesnt go out of bounds
	void boundCamera();

	//-Imports all the background textures, goes to a static function
	void setBGTextures(std::vector<LTexture>& textures);
	void setBGTextures(std::vector<LTexture*>& textures);//-Overload using pointers

	//-Adds a background to the vector
	bool createBg(Background bg, Sint16 insert);
	
	//-Overload allowing more stuff
	bool createBg(Uint8 TextNum, Sint16 initX, Sint16 initY, Uint8 depth, bool tileX = false, bool tileY = false, Sint16 insert = 0);

	//Does the math and renders the backgrounds
	void renderBg();

	//sets the death barrier
	void setDeathBarrier(bool barrier);

	//checks for death barrier
	bool getDeathBarrier();

	//clears the hitbox vector
	void clearHitboxes();

	//adds a hitbox to the vector
	void addHitbox(Hitbox& a, bool good , bool hitbox, bool hurtbox);

	//shows hitboxes
	void debugShowHitboxes(SDL_Renderer& renderer);

	//returns the Enemy hitbox vector
	std::vector<Hitbox>& getEnemyHitboxes();

	//returns the Player hitbox vector
	std::vector<Hitbox>& getPlayerHitboxes();

	void step();

	//nullifies all enemy pointers
	void initEnemyArray();

	//allocates some enemy memory and puts it into the array
	//eType is the enemy type you want, any number from 0-254
	//x and y are the coordinates, multiplied by the tilesize 16
	void addEnemy(Uint8 eType, Uint8 x, Uint8 y);

	//does a step for every enemy
	void stepEnemies();

	//does a render for every enemy
	void drawEnemies();

	//returns number of enemies
	Uint8 enemyCount();

private:
	Matrix<Block> mBlocks;
	std::vector<LTexture*> mBGTextures;
	std::vector<Background> mBackgrounds;
	std::vector<SDL_Rect> mRects;
	SDL_Color mAmbient;
	SDL_Rect* mCamera;
	int mLevelWidth;
	int mLevelHeight;
	bool mDeathBarrier;
	std::vector<Hitbox> mPlayerHitboxes;
	std::vector<Hitbox> mPlayerHurtboxes;
	std::vector<Hitbox> mEnemyHitboxes;
	std::vector<Hitbox> mEnemyHurtboxes;
	static const Uint8 kMaxEnemies = 255;
	Enemy* mEnemies[kMaxEnemies];//pointer for polymorphism
	void mDeleteEnemy(Uint8 slot);//deletes an enemy if the slot given has an enemy
	void mClearEnemies();//deletes all enemies in the level
};

#endif