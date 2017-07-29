#include "Zone.h"

using namespace std;

Zone::Zone()
{
	//mLevels = NULL;
	mLevels.clear();
	mCurrentLevel = 0;
	mPrefix = "";
	mPlayer = NULL;
	mLevelCount = 0;
	mCamera = NULL;
	mBGTextures = vector<LTexture*>();

}

Zone::Zone(SDL_Rect* camera, Player* player, string prefix, string suffix, std::vector<LTexture*>& textures)
{
	//mLevels = NULL;
	mLevels.clear();
	mCamera = camera;
	mPlayer = player;
	mPrefix = prefix;
	mSuffix = suffix;
	mBGTextures = textures;
	mLevelCount = 0;
	mCurrentLevel = 0;
}

Zone::~Zone()
{
	release();
}

void Zone::init()
{
	countFiles();
	mLevels.resize(mLevelCount);
	if (mPlayer != NULL && mCamera != NULL)
	{
		loadZone();
		setLevel(0);
	}
}

void Zone::setPrefix(std::string s)
{
	mPrefix = s;
}

void Zone::setSuffix(std::string s)
{
	mSuffix = s;
}

void Zone::loadLevel()
{
	if (mCurrentLevel >= mLevelCount)
	{
		cout << "ERROR: Level Number out of bounds\n";
		cout << "Requested: " << mCurrentLevel << " Size: " << (int)mLevelCount << endl;
	}
	stringstream s;

	s.str(string());//empty the stream
	s << mPrefix << '_' << (int)mCurrentLevel << mSuffix;//make the string that will read the map
	mLevels[mCurrentLevel].Loadmap(s.str().c_str());
	
}

void Zone::loadLevel(int i)
{
	if (i >= mLevelCount)
	{
		cout << "ERROR: Level Number out of bounds\n";
		cout << "Requested: " << i << " Size: " << (int)mLevelCount << endl;
	}
	stringstream s;

	s.str(string());//empty the stream
	s << mPrefix << '_' << i << mSuffix;//make the string that will read the map
	mLevels[i].Loadmap(s.str().c_str());
}

void Zone::setLevel(int i)
{
	if (i >= mLevelCount)
	{
		cout << "ERROR: Level Number out of bounds\n";
		cout << "Requested: " << i << " Size: " << (int)mLevelCount << endl;
	}
	else if (mPlayer == NULL)
		cout << "ERROR: Current Zone has NULL Player\n";
	else
	{
		mCurrentLevel = i;
		mPlayer->setLevel(&mLevels[i]);
	}
}

void Zone::stepEnemiesCurrentLevel()
{
	mCheckDoors();
	mLevels[mCurrentLevel].stepEnemies();
}

void Zone::clearHitboxesCurrentLevel()
{
	mLevels[mCurrentLevel].clearHitboxes();
}

void Zone::renderCurrentLevel()
{
	mLevels[mCurrentLevel].renderLevel();
}

void Zone::drawEnemiesCurrentLevel()
{
	mLevels[mCurrentLevel].drawEnemies();
}

void Zone::debugShowHitboxesCurrentLevel(SDL_Renderer& r)
{
	mLevels[mCurrentLevel].debugShowHitboxes(r);
}

int Zone::getLevelCount()
{
	countFiles();
	return mLevelCount;
}

void Zone::setPlayer(Player * a)
{
	mPlayer = a;
}

void Zone::setCamera(SDL_Rect * camera)
{
	mCamera = camera;
}

void Zone::loadZone()
{
	countFiles();
	stringstream ss;
	for (int i = 0; i < mLevelCount; ++i)
	{
		ss.str(string());//clear the stream
		ss << mPrefix << '_' << i << mSuffix;//make the string that will read the map
		mLevels[i].setCamera(mCamera);
		mLevels[i].setBGTextures(mBGTextures);
		mLevels[i].Loadmap(ss.str().c_str());
	}
}

void Zone::setBGTextures(vector<LTexture*>& textures)
{
	mBGTextures = textures;
}

Uint8 Zone::getLevelSize()
{
	return (Uint8)mLevelCount;
}

void Zone::release()
{
	if (mLevels.size() != 0)//if the mLevel has not been init properly, do not need to release
	{
		for (int i = 0, j = (int)mLevelCount; i < j; ++i)
		{
			mLevels[i].~Level();//deallocate memory for all levels
		}
		mLevelCount = 0;
		mLevels.clear();
	}
}

Uint8 Zone::enemyCountCurrentLevel()
{
	return mLevels[mCurrentLevel].enemyCount();
}

void Zone::setSpawn()
{
	if (mLevels.size() != 0)
	{
		Uint8 x, y;
		mLevels[0].getSpawnPoint(x, y);
		mPlayer->getX() = x * 16;
		mPlayer->getY() = y * 16;
	}
}

void Zone::mCheckDoors()
{
	SDL_Rect coll = mPlayer->getCollisionBox();
	coll.x += mPlayer->getX();
	coll.y += mPlayer->getY();

	Door* door = mLevels[mCurrentLevel].checkDoorCollision(coll);
	if (door != NULL)
	{
		//the door will change the level and the position in the level
		if (mCurrentLevel != door->level)
			setLevel(door->level);
		/*
		for the target position, the values 255 mean 
		to leave the position on that axis unchanged 
		*/
		if (door->x != 255)
			mPlayer->getX() = door->x * 16;
		if (door->y != 255)
			mPlayer->getY() = door->y * 16;
	}

}

void Zone::countFiles()
{
	int tcount = 0;
	stringstream a;
	ifstream ifile;
	for (; tcount < 255; ++tcount)
	{
		a.str(string());//clear the stream
		a <<  mPrefix << '_' << tcount << mSuffix; //creates a filename string (eg. "debug_0.txt")

		//open the file
		ifile.open(a.str().c_str());

		//this checks if the file exists
		if (!ifile)
		{
			break;
		}
		ifile.close();
	}
	mLevelCount = tcount;
}
