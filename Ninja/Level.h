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
#include "LTexture.h"
#include "Algorithms.h"
#include "Hitbox.h"
#include "Enemy.h"
#include "Robot.h"
#include "Demon.h"
#include "Cannon.h"
#include "MagMatrix.h"
#include "LevelID.h"


struct Background
{
	Uint8 TextNum;
	Sint16 initX;
	Sint16 initY;
	Uint8 X_subp;//subpixel movement, a minimum of 1/256th of a pixel per frame
	Uint8 Y_subp;
	Uint8 depth;
	Sint8 XV;
	Uint8 XV_subp;//subpixel movement
	Sint8 YV;
	Uint8 YV_subp;
	bool tileX;
	bool tileY;
};

//if you collide with a rectangle, you go to the specified place
struct Door
{
	SDL_Rect coll;//collision box for the warp
	Uint8 level;//which level in the zone to warp to
	Uint8 x;//position to warp to, x
	Uint8 y;//position to warp to, y
	Uint8 texturenum;
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

	//-Places a block on the map, will overwrite an existing block if necessary
	void setBlock(Uint8 block, Uint8 x, Uint8 y);

	//-Renders every inidividual block
	void renderBlocks();

	//-A color that will be set to be used for color modulation
	//-Note that it doesn't actually set the color modulation 
	void setAmbientLight(SDL_Color color);

	//-Accessor to the ambient light
	SDL_Color getAmbientLight();

	//-Corrects the camera's position so it doesnt go out of bounds
	void boundCamera();

	//-Imports all the background textures, goes to a static vector
	void setBGTextures(std::vector<LTexture>& textures);//useless function, I'll keep it
	void setBGTextures(std::vector<LTexture*>& textures);//-Overload using pointers

	//-Imports all the Door textures, goes to a static vector
	void setDoorTextures(std::vector<LTexture*>& textures);

	//set the block textures
	void setBlockTextures(LTexture* textures);

	//-Adds a background to the vector
	bool createBg(Background bg, Sint16 insert);
	
	//-Overload allowing more stuff
	bool createBg(Uint8 TextNum, Sint16 initX, Sint16 initY, Uint8 depth, Sint8 XV = 0, Uint8 XV_subp = 0, Sint8 YV = 0, Uint8 YV_subp = 0, bool tileX = false, bool tileY = false, Sint16 insert = 0);

	//Does the math and renders the backgrounds
	void renderBg();

	//renders the UpDoors
	void renderDoors();

	//sets the death barrier
	void setDeathBarrier(bool barrier);

	//set mLevelID
	void setLevelID(LevelID l);

	//return mLevelID
	LevelID getLevelID();

	//create a door at the following position
	void createDoor(Door d);

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

	//render function: boundcamera, renderbg, renderblocks, renderDoors
	void renderLevel();

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

	//set mCamera
	void setCamera(SDL_Rect* camera);

	//get mCamera, by value
	SDL_Rect getCamera();

	//modifies the references given
	void getSpawnPoint(Uint8 & x, Uint8 & y);

	//overload using int
	void getSpawnPoint(int & x, int & y);

	//Check Collision with door
	Door* checkDoorCollision(SDL_Rect c, bool up = false);

	Uint8 getSong();

	SDL_Point getPlayerPosition();

	bool checkPlayerDead();
private:
	Matrix<Uint8> mBlocks;
	Uint8 mSong;//the song index that should be playing
	static std::vector<LTexture*> mBGTextures;//the textures for the backgrounds
	static std::vector<LTexture*> mDoorTextures;//the textures for the Doors
	static LTexture* mBlockTextures;//the texture for the blocks
	std::vector<Background> mBackgrounds;//the data representing the backgrounds
	std::vector<SDL_Rect> mRects;//the collision rectangles
	std::vector<Door> mDoors;//all the warpy doors
	std::vector<Door> mUpDoors;//all the warpy doors, need to press Up key to go through these
	static std::vector<SDL_Rect> mBlockClipRects;//the render target rectangles for the block textures
	SDL_Color mAmbient;//unused, the ambient lighting will be implemented someday
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
	LevelID mLevelID;

	void mBGGoodEdge(int & ax, int & bx, int & aw);//modifies ax, finds topmost or leftmost edge
	void mBGTileRender(Uint8& bgnum, SDL_Rect& c, bool & tX, bool & tY);//renders the background for X tiling only
	void drawBlock(Uint16 x, Uint16 y);//draws a single block
	void mClearBlocks();//sets all blocks to 255, which is the null block
	Uint8 mSpawnX;
	Uint8 mSpawnY;
};

#endif
