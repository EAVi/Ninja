#include "Level.h"

using namespace std;

int Level::tilesize = 16;


Level::Level()
{
	mLevelWidth = 0;
	mLevelHeight = 0;
	mCamera = NULL;
	mDeathBarrier = false;
	mBlocks.clear();
	mBGTextures.clear();
	mBackgrounds.clear();
	mRects.clear();
}

Level::Level(int width, int height, SDL_Rect* camera)
{
	mLevelWidth = width;
	mLevelHeight = height;
	mCamera = camera;
	mDeathBarrier = false;
	mBlocks.clear();
	mBGTextures.clear();
	mBackgrounds.clear();
	mRects.clear();
}

SDL_Rect Level::getLevelDimensions()
{
	return{ 0, 0, mLevelWidth, mLevelHeight };
}

/*
vector<Block> Level::getBlocks()
{
	return mBlocks;
}
*/

vector<SDL_Rect>& Level::getRects()
{
	return mRects;
}

void Level::setBlocks(Block block)
{
	mBlocks.get(block.getX() / tilesize, block.getY() / tilesize) = block;
}

void Level::setBlocks(vector<Block> blocks)//note that this converts vector to matrix
{
	for (int i = 0, j = blocks.size(); i < j; ++i)
	{
		this->setBlocks(blocks[i]);
	}
}


void Level::renderLevel()
{
	int i = mCamera->x / tilesize;
	int j = ((mCamera->x + mCamera->w + tilesize - 1) / tilesize);//basically, just rounds it up

	int k = mCamera->y / tilesize;
	int kReset = k;
	int l = ((mCamera->y + mCamera->h + tilesize - 1) / tilesize);

	for (; i < j; ++i)//x position
		for (k = kReset; k < l; ++k)//y position
			if (mBlocks.valid(i, k)//checks for validity, I'm confident that this is no longer needed, but keeping it for safe measures
				&& (mBlocks.get(i, k).getClip() != 255))//ensures it is not a default block
			{

				mBlocks.get(i, k).render(mCamera->x, mCamera->y);
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

bool Level::getDeathBarrier()
{
	return mDeathBarrier;
}

void Level::clearHitboxes()
{
	mPlayerHitboxes.clear();
	mPlayerHurtboxes.clear();
	mEnemyHitBoxes.clear();
	mEnemyHurtBoxes.clear();
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
			mEnemyHitBoxes.push_back(a);
		if (hurtbox)
			mPlayerHurtboxes.push_back(a);
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
	for (int i = 0, j = mEnemyHitBoxes.size(); i < j; ++i)//enemyhitboxes
	{
		temp = mEnemyHitBoxes[i].hitbox;
		temp.x -= mCamera->x;
		temp.y -= mCamera->y;
		SDL_RenderDrawRect(&renderer, &temp);
	}

	SDL_SetRenderDrawColor(&renderer, 255, 255, 0, 255);//Black
	for (int i = 0, j = mEnemyHurtBoxes.size(); i < j; ++i)//enemyhurtboxes
	{
		temp = mEnemyHurtBoxes[i].hitbox;
		temp.x -= mCamera->x;
		temp.y -= mCamera->y;
		SDL_RenderDrawRect(&renderer, &temp);
	}

	SDL_SetRenderDrawColor(&renderer, tc.r, tc.g, tc.b, tc.a);//back to black or whatever the color was

}

vector<Hitbox>& Level::getEnemyHitboxes()
{
	return mEnemyHitBoxes;
}

void Level::step()
{
	boundCamera();
	renderBg();
	renderLevel();
}

bool Level::Loadmap(string filename)
{
	//file reading segment
	ifstream ifile;
	ifile.open(filename.c_str());
	if (!ifile.is_open())
	{ 
		cout << "Error opening file: " << filename << endl;
		return false; 
	}	
	char cdata = ifile.get();
	string sdata;
	vector<int> idata;
	while (ifile.good())
	{
		if (cdata != ' ' && cdata != '\n')
		sdata += cdata;
		cdata = ifile.get();
		if ((cdata == ' ' || cdata == '\n') && sdata != "")
		{
			idata.push_back(stoi(sdata, nullptr, 10));
			sdata = "";
		}
	}
	if (sdata != "")//gets the last data entry if it exists
		idata.push_back(stoi(sdata, nullptr, 10));
	ifile.close();

	//the part that actually makes the map
	if (idata.size() == 0)
	{
		cout << filename << "is empty!\n";
	}
	Uint32 loadPoint = 0;

	Uint8 attrs = 0; //amount of 8-bit elements used in a given object

	
	//background loader
	attrs = 7;//(8, 16, 16, 8, 1,< 1) //there's leftover 6 bits
	for (Uint32 i = loadPoint, j = idata.size(); i < (j - attrs + 1); i = i + attrs)
	{
		loadPoint = i;
		//gotta stop making backgrounds eventually, if the texture number is at maximum, go to next part
		if (idata[i] == 255)
		{
			++loadPoint;
			break;
		}
		
		createBg(
			idata[i],
			idata[i + 1] * 256 + idata[i + 2],
			idata[i + 3] * 256 + idata[i + 4],
			idata[i + 5],
			idata[i + 6] & 2,
			idata[i + 6] & 1
			);
	}//end background loader
	

	attrs = 4; //(8, 8, 8, 4,< 4)
	//block loader
	mBlocks.setDimensions((mLevelWidth + (tilesize - 1)) / tilesize, (mLevelHeight + (tilesize - 1)) / tilesize);
	for (Uint16 i = loadPoint, j = idata.size(); i < (j - attrs + 1); i = i + attrs, loadPoint = i)
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
				Block tempBlock
					({( xPosition ) * tilesize,
					(yPosition) * tilesize,
					idata[i] });
				
				if (mBlocks.valid(xPosition, yPosition))
					mBlocks.get(xPosition, yPosition) = tempBlock;
			}

		//create the collision rectangles based on given dimensions
		mRects.push_back({
			idata[i + 1] * tilesize,//x
			idata[i + 2] * tilesize,//y
			(dims % 16 + 1) * tilesize,//w
			(dims / 16 + 1) * tilesize//h
		});
	}//end block loader

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


bool Level::createBg(Background bg, Sint16 insert)
{
	bool exists = false;

	//segmentation faults suck, so these are needed
	if (mBGTextures.size() == 0)
		return false;

	if (insert < 0)//a 
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

bool Level::createBg(Uint8 TextNum, Sint16 initX, Sint16 initY, Uint8 depth, bool tileX, bool tileY, Sint16 insert)
{
	return (createBg({ TextNum, initX, initY, depth, tileX, tileY }, insert));
}

void Level::renderBg()
{
	for (Uint8 i = 0; i < (Uint8)mBackgrounds.size(); ++i)
	{

		int rX, rY;
		//makes it easier to keep track of the values
		SDL_Rect rect
		{
			mBackgrounds[i].initX,
			mBackgrounds[i].initY,
			mCamera->x,
			mCamera->y,
		};

		//the render destination of the background, with cases where flops aren't needed
		switch (mBackgrounds[i].depth)
		{
		case 0:
			rX = rect.x;
			rY = rect.y;
			break;
		case 255:
			rX = rect.x + rect.w;
			rY = rect.y + rect.h;
			break;
		default:
			rX = (int)(rect.x + ((rect.w) * (mBackgrounds[i].depth / 255.f)));
			rY = (int)(rect.y + ((rect.h) * (mBackgrounds[i].depth / 255.f)));
		}

		SDL_Rect coll
		{
			rX,
			rY,
			mBGTextures[mBackgrounds[i].TextNum]->getWidth(),
			mBGTextures[mBackgrounds[i].TextNum]->getHeight()
		};

		//cout << coll.x << " " << coll.w << endl;
		//cout << mCamera->x << " " << mCamera->w << "\n\n";


		//checks whether to create a new background, first needs to check for collision
		if (checkCollision(coll, *mCamera))
		{
			if (mBackgrounds[i].tileX)
			{
				//if the background leaks to the right edge
				if ((mCamera->x + mCamera->w) > (coll.x + coll.w))
				{
					createBg(
						mBackgrounds[i].TextNum,
						mBackgrounds[i].initX + coll.w,
						mBackgrounds[i].initY,
						mBackgrounds[i].depth,
						mBackgrounds[i].tileX,
						mBackgrounds[i].tileY,
						(Uint8)i
						);
				}

				//if the background leaks to the left edge
				if (coll.x > mCamera->x)
				{
					createBg(
						mBackgrounds[i].TextNum,
						mBackgrounds[i].initX - coll.w,
						mBackgrounds[i].initY,
						mBackgrounds[i].depth,
						mBackgrounds[i].tileX,
						mBackgrounds[i].tileY,
						(Uint8)i
						);

				}
			}

			if (mBackgrounds[i].tileY)
			{
				//if the background leaks to the bottom edge
				if ((mCamera->y + mCamera->h) > (coll.y + coll.h))
				{
					createBg(
						mBackgrounds[i].TextNum,
						mBackgrounds[i].initX,
						mBackgrounds[i].initY + coll.h,
						mBackgrounds[i].depth,
						mBackgrounds[i].tileX,
						mBackgrounds[i].tileY,
						(Uint8)i
						);
				}

				//if the background leaks to the bottom edge
				if (mCamera->y < coll.y)
				{
					createBg(
						mBackgrounds[i].TextNum,
						mBackgrounds[i].initX,
						mBackgrounds[i].initY - coll.h,
						mBackgrounds[i].depth,
						mBackgrounds[i].tileX,
						mBackgrounds[i].tileY,
						(Uint8)i
						);

				}
			}
		}

		//checks whether to render the BG object
		if (checkCollision(coll, *mCamera))
			mBGTextures[mBackgrounds[i].TextNum]->renderTexture(rX - mCamera->x, rY - mCamera->y);
		else if (mBackgrounds[i].tileY && mBackgrounds[i].tileX)//can only delete if it tiles
		{
			mBackgrounds.erase(mBackgrounds.begin() + i);
			--i;
		}
	}
}