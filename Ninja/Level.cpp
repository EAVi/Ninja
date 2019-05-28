#include "Level.h"

#include <iostream>

using namespace std;

int Level::tilesize = 16;
vector<SDL_Rect> Level::mBlockClipRects = spriteClipper(256, 256, {0,0,16,16});
std::vector<LTexture*> Level::mBGTextures = vector<LTexture*>();
std::vector<LTexture*> Level::mDoorTextures = vector<LTexture*>();
LTexture* Level::mBlockTextures = NULL;

Level::Level()
	:Level(0, 0, NULL)
{}

Level::Level(int width, int height, SDL_Rect* camera)
{
	mLevelWidth = width;
	mLevelHeight = height;
	mCamera = camera;
	mDeathBarrier = false;
	mBlocks.clear();
	mDoors.clear();
	mUpDoors.clear();
	mBGTextures.clear();
	mBackgrounds.clear();
	mRects.clear();
	mPlayerHitboxes.clear();
	mPlayerHurtboxes.clear();
	mEnemyHitboxes.clear();
	mEnemyHurtboxes.clear();
	initEnemyArray();
	mSpawnX = 0;
	mSpawnY = 0;
	mSong = 255;
}

Level::~Level()
{
	this->mClearEnemies();
}

SDL_Rect Level::getLevelDimensions()
{
	return{ 0, 0, mLevelWidth, mLevelHeight };
}

vector<SDL_Rect>& Level::getRects()
{
	return mRects;
}

void Level::setBlock(Uint8 block, Uint8 x, Uint8 y)
{
	mBlocks.get(x,y) = block;
}

void Level::renderBlocks()
{
	if (mCamera == NULL)
	{
		cout << "Level mCamera NULL\n";
		return;
	}
	int i = mCamera->x / tilesize;
	int j = ((mCamera->x + mCamera->w + tilesize - 1) / tilesize);//basically, just rounds it up

	int k = mCamera->y / tilesize;
	int kReset = k;
	int l = ((mCamera->y + mCamera->h + tilesize - 1) / tilesize);

	for (; i < j; ++i)//x position
		for (k = kReset; k < l; ++k)//y position
			if (mBlocks.get(i, k) != 255)//ensures it is not a default block (will not draw block with #255)
			{
				drawBlock(i,k);
			}
}

void Level::setAmbientLight(SDL_Color color)
{
	mAmbient = color;
}

SDL_Color Level::getAmbientLight()
{
	return mAmbient;
}


void Level::boundCamera()
{
	if (mCamera == NULL)
	{
		cout << "Level mCamera NULL\n";
		return;
	}
	if (mCamera->x + mCamera->w > this->mLevelWidth)
		mCamera->x = this->mLevelWidth - mCamera->w;
	if (mCamera->y + mCamera->h > this->mLevelHeight)
		mCamera->y = this->mLevelHeight - mCamera->h;
	if (mCamera->x < 0)
		mCamera->x = 0;
	if (mCamera->y < 0)
		mCamera->y = 0;
}

void Level::setDeathBarrier(bool barrier)
{
	mDeathBarrier = barrier;
}

void Level::setLevelID(LevelID l)
{
	mLevelID = l;
}

LevelID Level::getLevelID()
{
	return mLevelID;
}

void Level::createDoor(Door d)
{
	if (d.texturenum == 255)
		mDoors.push_back(d);
	else mUpDoors.push_back(d);
}

bool Level::getDeathBarrier()
{
	return mDeathBarrier;
}

void Level::clearHitboxes()
{
	mPlayerHitboxes.clear();
	mPlayerHurtboxes.clear();
	mEnemyHitboxes.clear();
	mEnemyHurtboxes.clear();
}

void Level::addHitbox(Hitbox&  a, bool good, bool hitbox, bool hurtbox)
{
	if (good)
	{
		if (hitbox)
			mPlayerHitboxes.push_back(a);
		if (hurtbox)
			mPlayerHurtboxes.push_back(a);//can be both a hitbox and a hurtbox
	}

	if (!good)
	{
		if (hitbox)
			mEnemyHitboxes.push_back(a);
		if (hurtbox)
			mEnemyHurtboxes.push_back(a);
	}

}

void Level::debugShowHitboxes(SDL_Renderer& renderer)
{
	SDL_Color tc;
	SDL_GetRenderDrawColor(&renderer, &tc.r, &tc.g, &tc.b, &tc.a);

	SDL_SetRenderDrawColor(&renderer, 255, 0, 255, 255);//pink
	SDL_Rect temp;
	for (int i = 0, j = mPlayerHitboxes.size(); i < j; ++i)//playerhitboxes
	{
		temp = mPlayerHitboxes[i].hitbox;
		temp.x -= mCamera->x;
		temp.y -= mCamera->y;
		SDL_RenderDrawRect(&renderer, &temp);
	}

	SDL_SetRenderDrawColor(&renderer, 0, 255, 0, 255);//green
	for (int i = 0, j = mPlayerHurtboxes.size(); i < j; ++i)//playerhurttboxes
	{
		temp = mPlayerHurtboxes[i].hitbox;
		temp.x -= mCamera->x;
		temp.y -= mCamera->y;
		SDL_RenderDrawRect(&renderer, &temp);
	}

	SDL_SetRenderDrawColor(&renderer, 255, 0, 0, 255);//Black
	for (int i = 0, j = mEnemyHitboxes.size(); i < j; ++i)//enemyhitboxes
	{
		temp = mEnemyHitboxes[i].hitbox;
		temp.x -= mCamera->x;
		temp.y -= mCamera->y;
		SDL_RenderDrawRect(&renderer, &temp);
	}

	SDL_SetRenderDrawColor(&renderer, 255, 255, 0, 255);//Black
	for (int i = 0, j = mEnemyHurtboxes.size(); i < j; ++i)//enemyhurtboxes
	{
		temp = mEnemyHurtboxes[i].hitbox;
		temp.x -= mCamera->x;
		temp.y -= mCamera->y;
		SDL_RenderDrawRect(&renderer, &temp);
	}

	SDL_SetRenderDrawColor(&renderer, tc.r, tc.g, tc.b, tc.a);//back to black or whatever the color was

}

vector<Hitbox>& Level::getEnemyHitboxes()
{
	return mEnemyHitboxes;
}

std::vector<Hitbox>& Level::getPlayerHitboxes()
{
	return mPlayerHitboxes;
}

void Level::renderLevel()
{
	boundCamera();
	renderBg();
	renderBlocks();
	renderDoors();
}

void Level::initEnemyArray()
{
	for (int i = 0; i < kMaxEnemies; ++i)
	{
		mEnemies[i] = NULL;
	}
}

void Level::addEnemy(Uint8 eType, Uint8 x, Uint8 y)
{
	if (eType == 255) return;//255 is the terminator for the level builder
	Uint8 slot = 0;
	for (; slot < kMaxEnemies; ++slot)//find an empty slot to put an enemy
	{
		if (mEnemies[slot] == NULL)
			break;

	}

	//no more than 255 enemies allowed in a level, my dumb rule
	if (slot == kMaxEnemies) return;

	switch (eType)
	{
	case 1:
		mEnemies[slot] = new Robot(x, y, false, this);	break;
	case 2:
		mEnemies[slot] = new Demon(x, y, false, this);	break;
	case 3:
		mEnemies[slot] = new Cannon(x, y, false, this);	break;
	case 4:
		mEnemies[slot] = new Samurai(x, y, false, this);	break;
	default://0 
		mEnemies[slot] = new Enemy(x, y, false, this);	break;
	}

}

void Level::stepEnemies()
{
	for (int i = 0; i < kMaxEnemies; ++i)
	{
		if (mEnemies[i] != NULL)
		{
			mEnemies[i]->step();
			if (!mEnemies[i]->checkLiving())
				mDeleteEnemy(i);
		}
	}

}

void Level::drawEnemies()
{
	for (int i = 0; i < kMaxEnemies; ++i)
	{
		if (mEnemies[i] != NULL)
			mEnemies[i]->render(mCamera->x,mCamera->y);
	}
}

Uint8 Level::enemyCount()
{
	Uint8 c = 0;
	for (int i = 0; i < kMaxEnemies; ++i)
	{
		if (mEnemies[i] != NULL)//if the slot isn't empty, it's an enemy
			++c;
	}
	return c;
}

void Level::setCamera(SDL_Rect * camera)
{
	if (camera == NULL)
		cout << "NULL camera being passed to level\n";
	else
		mCamera = camera;
}

SDL_Rect Level::getCamera()
{
	return *mCamera;
}

void Level::getSpawnPoint(Uint8 & x, Uint8 & y)
{
	x = mSpawnX;
	y = mSpawnY;
}

void Level::getSpawnPoint(int & x, int & y)
{
	x = (int)mSpawnX * 16;
	y = (int)mSpawnY * 16;
}

Door * Level::checkDoorCollision(SDL_Rect c, bool up)
{
	for (int i = 0, j = mDoors.size(); i < j; ++i)
	{
		if (checkCollision(mDoors[i].coll, c))
			return &mDoors[i];
	}
	if (up)
		for (int i = 0, j = mUpDoors.size(); i < j; ++i)
		{
			if (checkCollision(mUpDoors[i].coll, c))
				return &mUpDoors[i];
		}
	return NULL;
}

Uint8 Level::getSong()
{
	return mSong;
}

SDL_Point Level::getPlayerPosition()
{
	//need to use the first element to find the center of the Ninja
	if (mPlayerHurtboxes.size() == 0) return { 0,0 };

	SDL_Point a;
	a.x = mPlayerHurtboxes[0].hitbox.x + mPlayerHurtboxes[0].hitbox.w / 2;
	a.y = mPlayerHurtboxes[0].hitbox.y + mPlayerHurtboxes[0].hitbox.h / 2;
	return a;
}

bool Level::checkPlayerDead()
{
	if (mPlayerHurtboxes.size() == 0) return false;
	return ((mPlayerHurtboxes[0].damage == 0)
		&& (mPlayerHurtboxes[0].hitstun == 1));
}

void Level::mDeleteEnemy(Uint8 slot)
{
	if (mEnemies[slot] != NULL)
	{
		delete mEnemies[slot];
		mEnemies[slot] = NULL;
	}
}

void Level::mClearEnemies()
{
	for (int i = 0; i < kMaxEnemies; ++i)
	{
		mDeleteEnemy(i);
	}
}

void Level::mBGGoodEdge(int & ax, int & bx, int & aw)
{
	//NAN error prevention measure
	if (aw == 0) return;

	int diff;
	int div;
	if (ax < bx)
	{
		diff = bx - ax;//want to get a positive number, want to move the bg up
	}
	else if (ax == bx)
		return;
	else //ax >=bx
	{
		//diff = -(ax - bx + aw - 1);//should cieling up, we want the best corner
		//should be negative because 
		//we're moving ax down to get a good corner
		diff = bx - ax - aw + 1;//
	}
	div = diff / aw;
	ax += div * aw;
}

void Level::mBGTileRender(Uint8 & bgnum, SDL_Rect & c, bool & tX, bool & tY)
{
	//NAN error prevention measure
	if (c.w == 0 || c.h == 0)
		return;

	if (!(tX || tY) && (checkCollision(c, *mCamera)))//if there's no tiling, just do a simple collisioncheck
		{
			mBGTextures[mBackgrounds[bgnum].TextNum]->renderTexture(c.x - mCamera->x, c.y - mCamera->y);
			return;
		}
	else
	{
		if (tX)//finds the good edge for x if xtile is true
			mBGGoodEdge(c.x, mCamera->x, c.w);
		if (tY)//finds the good edge for y if ytile is true
			mBGGoodEdge(c.y, mCamera->y, c.h);
		//if both these conditions were met, then it'll return a good corner

		/*
		here's a big arbitrary formula, it gets the width between the left side of c
		and the right side of mCamera, divides it by the width of c, and cielings it up
		it's just a number to see how many times the loop should happen

		Also, the number will ALWAYS be greater than 1 if the bg is onscreen.
		*/
		Uint8 xtilenum = (mCamera->x + mCamera->w + c.w - 1 - c.x) / c.w;
		Uint8 ytilenum = (mCamera->y + mCamera->h + c.h - 1 - c.y) / c.h;

		if (!tX) xtilenum = 1;
		if (!tY) ytilenum = 1;

		int origy = c.y;//a number for the y value to revert to at the beginning of the nested loop

		for (int i = 0; i < xtilenum; ++i)
		{
			c.y = origy;//reverting back to the y value
			for (int j = 0; j < ytilenum; ++j)
			{
				mBGTextures[mBackgrounds[bgnum].TextNum]->renderTexture(c.x - mCamera->x, c.y - mCamera->y);
				c.y += c.h;
			}
			c.x += c.w;
		}

		//if the top-rightmost tile doesn't collide, then no need to render
		if (!(checkCollision(c, *mCamera))) return;
	}		
}

void Level::drawBlock(Uint16 x, Uint16 y)
{
	mBlockTextures->renderTexture((x << 4) - mCamera->x, (y << 4) - mCamera->y, &mBlockClipRects[mBlocks.get(x, y)]);
}

void Level::mClearBlocks()
{
	int j = mBlocks.size();
	for (int i = 0; i < j; ++i)
		mBlocks[i] = 255;
}


bool Level::Loadmap(string filename)
{
	//file reading segment
	ifstream ifile;
	ifile.open(filename.c_str(), std::ifstream::in | std::ifstream::binary);
	if (!ifile.is_open())
	{ 
		cout << "Error opening file: " << filename << endl;
		return false; 
	}	
	Uint8 cdata = (Uint8)ifile.get();
	vector<int> idata;
	while (ifile.good())//read the binary file
	{
		idata.push_back(cdata);
		cdata = (Uint8)ifile.get();
	}
	ifile.close();

	//the part that actually makes the map
	if (idata.size() == 0)
	{
		cout << filename << "is empty!\n";
	}
	Uint32 loadPoint = 0;

	Uint8 attrs = 8; //amount of 8-bit elements used in a given object

	//Level dimensions, ambient light, spawn point
	mLevelWidth = idata[0] * tilesize;
	mLevelHeight = idata[1] * tilesize;
	setAmbientLight({ (Uint8)idata[2], (Uint8)idata[3], (Uint8)idata[4] });
	mSpawnX = idata[5];
	mSpawnY = idata[6];
	mSong = idata[7];
	loadPoint += attrs; 

	
	//background loader
	attrs = 11;//(8, 16, 16, 8, 1,< 1, 8, 8) //there's leftover 6 bits
	for (Uint32 i = loadPoint, j = idata.size(); i < (j - attrs + 1); loadPoint = i = i + attrs)
	{
		//gotta stop making backgrounds eventually, if the texture number is at maximum, go to next part
		if (idata[i] == 255)
		{
			++loadPoint;
			break;
		}
		
		createBg(
			idata[i],//textnum
			idata[i + 1] * 256 + idata[i + 2],//initx
			idata[i + 3] * 256 + idata[i + 4],//inity
			idata[i + 5],//depth
			idata[i + 6],//XV
			(Sint8)idata[i + 7],//XV_subp
			idata[i + 8],//YV
			(Sint8)idata[i + 9],//YV_subp
			idata[i + 10] & 2,//tilex
			idata[i + 10] & 1//tiley
			);
	}//end background loader
	

	attrs = 4; //(8, 8, 8, 4,< 4)
	//block loader
	mBlocks.setDimensions((mLevelWidth + (tilesize - 1)) / tilesize, (mLevelHeight + (tilesize - 1)) / tilesize);
	mClearBlocks();
	for (Uint16 i = loadPoint, j = idata.size(); i < (j - attrs + 1); loadPoint = i = i + attrs)
	{
		if (idata[i] == 255)
		{
			++loadPoint;
			break;
		}
		Uint8 dims = idata[i + 3];
		//Nested loop copies and pastes the same block along the dimension specified
		for (Uint8 k = 0; k < (dims % 16) + 1; ++k)//width first
			for (Uint8 l = 0; l < (dims / 16) + 1; ++l)//height second
			{
				Uint8 xPosition = idata[i + 1] + k;
				Uint8 yPosition = idata[i + 2] + l;
				//idata[i] is the block texture
				
				if (mBlocks.valid(xPosition, yPosition))
					mBlocks.get(xPosition, yPosition) = idata[i];
			}

		//create the collision rectangles based on given dimensions
		mRects.push_back({
			idata[i + 1] * tilesize,//x
			idata[i + 2] * tilesize,//y
			(dims % 16 + 1) * tilesize,//w
			(dims / 16 + 1) * tilesize//h
		});
	}//end block loader

	mRects = rectangleMerge(mRects);

	 //Enemy Loader
	attrs = 3;//(8, 16, 16, 8, 1,< 1) //there's leftover 6 bits
	for (Uint32 i = loadPoint, j = idata.size(); i < (j - attrs + 1); loadPoint = i = i + attrs)
	{
		loadPoint = i;
		//gotta stop making backgrounds eventually, if the texture number is at maximum, go to next part
		if (idata[i] == 255)
		{
			++loadPoint;
			break;
		}

		addEnemy(
			idata[i], 
			idata[i + 1], 
			idata[i + 2]);
	}//end Enemy loader

	//Door loader
	attrs = 7;//{8, 8, 8, 8}, 8, 8, 8
	for (Uint32 i = loadPoint, j = idata.size(); i < (j - attrs + 1); loadPoint = i = i + attrs)
	{
		loadPoint = i;
		if (idata[i] == 255)
		{
			++loadPoint;
			break;
		}

		int x, y, w, h;
		
		x = ((int)idata[i]) * tilesize;
		y = ((int)idata[i + 1]) * tilesize;
		w = ((int)idata[i + 2]) * tilesize;
		h = ((int)idata[i + 3]) * tilesize;

		mDoors.push_back
			({
			{x, y, w, h},
			(Uint8)idata[i + 4],
			(Uint8)idata[i + 5],
			(Uint8)idata[i + 6],
			0});
	}//Door loader

	//UpDoor loader
	attrs = 8;//{8, 8, 8, 8}, 8, 8, 8, 8
	for (Uint32 i = loadPoint, j = idata.size(); i < (j - attrs + 1); loadPoint = i = i + attrs)
	{
		loadPoint = i;
		if (idata[i] == 255)
		{
			++loadPoint;
			break;
		}

		int x, y, w, h;

		x = ((int)idata[i]) * tilesize;
		y = ((int)idata[i + 1]) * tilesize;
		w = ((int)idata[i + 2]) * tilesize;
		h = ((int)idata[i + 3]) * tilesize;

		mUpDoors.push_back
		({
			{ x, y, w, h },
			(Uint8)idata[i + 4],
			(Uint8)idata[i + 5],
			(Uint8)idata[i + 6],
			(Uint8)idata[i + 7]});
	}//UpDoor loader

	return true;
}/*Loadmap*/

void Level::setBGTextures(vector<LTexture>& textures)
{

	for (Uint8 i = 0, j = textures.size(); i < j ; ++i)
	{
		mBGTextures.push_back(&textures[i]);
	}
}

void Level::setBGTextures(vector<LTexture*>& textures)
{
	mBGTextures = textures;
}

void Level::setDoorTextures(std::vector<LTexture*>& textures)
{
	mDoorTextures = textures;
}

void Level::setBlockTextures(LTexture * textures)
{
	mBlockTextures = textures;
}


bool Level::createBg(Background bg, Sint16 insert)
{
	bool exists = false;

	//segmentation faults suck, so these are needed
	if (mBGTextures.size() == 0)
		return false;

	if (insert < 0)//cannot insert into a negative index
		return false;

	bg.TextNum = bg.TextNum % mBGTextures.size();//if tnum given is too big, it just wraps around
	
	//checks the vector to see if it already exists
	for (Uint8 i = 0, j = mBackgrounds.size(); i < j && !exists; ++i)
		if (bg == mBackgrounds[i])
			exists = true;

	//pushes back if so
	if (!exists)
	{
			mBackgrounds.insert(mBackgrounds.begin() + insert, bg);
	}
	//else cout << "BG already exists\n BG count: " << mBackgrounds.size() << endl;
	return !exists;
}

bool Level::createBg(Uint8 TextNum, Sint16 initX, Sint16 initY, Uint8 depth, Sint8 XV, Uint8 XV_subp, Sint8 YV, Uint8 YV_subp, bool tileX, bool tileY, Sint16 insert)
{
	return (createBg({ TextNum, initX, initY, 0, 0, depth, XV, XV_subp, YV, YV_subp, tileX, tileY }, insert));
}

void Level::renderBg()
{
	for (Uint8 i = 0; i < (Uint8)mBackgrounds.size(); ++i)
	{
		mBackgrounds[i].initX = //Move on the X axis using subpixel movement
			subPixelMove(mBackgrounds[i].initX,
			mBackgrounds[i].XV,
			mBackgrounds[i].X_subp,
			mBackgrounds[i].XV_subp);
		
		mBackgrounds[i].initY = //Y axis
			subPixelMove(mBackgrounds[i].initY,
			mBackgrounds[i].YV,
			mBackgrounds[i].Y_subp,
			mBackgrounds[i].YV_subp);

		SDL_Rect coll
		{
			0,//render x
			0,//render y
			mBGTextures[mBackgrounds[i].TextNum]->getWidth(),
			mBGTextures[mBackgrounds[i].TextNum]->getHeight()
		};

		//the render destination of the background, with cases where flops aren't needed
		switch (mBackgrounds[i].depth)
		{
		case 0://the background will stay still, like a block tile. only uses addition
			coll.x = mBackgrounds[i].initX;
			coll.y = mBackgrounds[i].initY;
			break;
		case 255://the background will follow the camera, ie it'll stay at the same place on screen. only uses addition
			coll.x = mBackgrounds[i].initX + mCamera->x;
			coll.y = mBackgrounds[i].initY + mCamera->y;
			break;
		default://the background has parallax, it will follow the camera with some ratio. uses flops to determine the amount of movement
			coll.x = (int)(mBackgrounds[i].initX + ((mCamera->x) * (mBackgrounds[i].depth / 255.f)));
			coll.y = (int)(mBackgrounds[i].initY + ((mCamera->y) * (mBackgrounds[i].depth / 255.f)));
		}


		mBGTileRender(i, coll, mBackgrounds[i].tileX, mBackgrounds[i].tileY);//calls the tilerender command to do the rest of the dirtywork
	}
}

void Level::renderDoors()
{
	for (int i = 0, j = mUpDoors.size(); i < j; ++i)
	{		
		//render every door 
		Door* d = &mUpDoors[i]; 

		//no need to multiply by tilesize, d.coll is an SDL_Rect {int,int,int,int}
		//and the door has been initialized with the tilesize already multiplied
		mDoorTextures[d->texturenum]->renderTexture(d->coll.x - mCamera->x, d->coll.y - mCamera->y);
		
	}
}
