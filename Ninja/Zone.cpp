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
	mLevelCount = 0;
	mCurrentLevel = 0;
	mZoneID = 0;
	mZoneComplete = false;
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
	mZoneID = 0;
	mZoneComplete = false;
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
	mLevels[mCurrentLevel].setLevelID({ mCurrentLevel, mZoneID });
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
	mLevels[i].setLevelID({(Uint8)i, mZoneID });
}

void Zone::setLevel(int i)
{
	if (i >= mLevelCount)
	{
		//if you give a door level number larger than the level count, it will count as beating the zone
		mZoneComplete = true;
	}
	else if (mPlayer == NULL)
		cout << "ERROR: Current Zone has NULL Player\n";
	else
	{
		mCurrentLevel = i;
		mPlayer->setLevel(&mLevels[i]);
	}
}

Uint8 Zone::getCurrentLevel()
{
	return mCurrentLevel;
}

bool Zone::stepEnemiesCurrentLevel()
{
	bool a = mCheckDoors();//returns true if door touched
	if (mCurrentLevel < mLevels.size())
		mLevels[mCurrentLevel].stepEnemies();
	return a;

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
		mLevels[i].setDoorTextures(mDoorTextures);
		mLevels[i].Loadmap(ss.str().c_str());
		mLevels[i].setLevelID({(Uint8)i, mZoneID});
	}
}

void Zone::setBGTextures(vector<LTexture*>& textures)
{
	mBGTextures = textures;
}

void Zone::setDoorTextures(std::vector<LTexture*>& textures)
{
	mDoorTextures = textures;
}

Uint8 Zone::getLevelSize()
{
	return (Uint8)mLevelCount;
}

void Zone::release()
{
	if (mLevels.size() != 0)//if the mLevel has not been init properly, do not need to release
	{
		mLevelCount = 0;
		mLevels.clear();
	}
}

void Zone::reloadZone()
{
	release();
	init();
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

bool Zone::mCheckDoors()
{
	SDL_Rect coll = mPlayer->getCollisionBox();
	coll.x += mPlayer->getX();
	coll.y += mPlayer->getY();

	Door* door = mLevels[mCurrentLevel].checkDoorCollision(coll, mPlayer->checkUpPressed());
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
		return true;
	}
	return false;
}

bool Zone::checkComplete()
{
	return mZoneComplete;
}

Uint8 Zone::getSongCurrentLevel()
{
	return mLevels[mCurrentLevel].getSong();
}

LevelID Zone::getLevelID()
{
	return {mCurrentLevel, mZoneID};
}

void Zone::setZoneID(Uint8 a)
{
	mZoneID = a;
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
