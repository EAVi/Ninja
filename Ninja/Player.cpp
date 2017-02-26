#include "Player.h"

using namespace std;
SDL_Rect Player::kStandardCollisionBox = { 10,0,12,32 };
SDL_Rect Player::kJumpingCollisionBox = { 10,9,12,17 };
float Player::kGravityF = 0.5;

Player::Player() 
{
	this->mTexture = NULL;
	mAnimationFrameRectangles.clear();
	mCollisionBox = {10,0,12,32};
	mX = 0;
	mY = 0;
	mXVelocity = 0;
	mYVelocityF = 0.0;
	mYVelocity = mYVelocityF;
	mFlipType = SDL_FLIP_NONE;
	mAnimationFrame = 0;
	mAttackCoolDown = 0;
	mSwordHitbox = { {6,0,19,25}, 3 };
}

void Player::setTexture(LTexture* texture)
{
	mTexture = texture;

	//after loading the texture, load the frameRectangles
	mAnimationFrameRectangles =
		spriteClipper(mTexture->getWidth(), mTexture->getHeight(), { 0, 0, kClipWidth, kClipHeight });
}

void Player::handleEvent(SDL_Event& e)
{
	if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
	{
		switch (e.key.keysym.sym)
		{
			case SDLK_d: mRightPress(); break;
			case SDLK_a: mLeftPress();  break;
			case SDLK_SPACE: mJumpPress(); break;
			case SDLK_m: mAttackPress(); break;
		}
	}
	else if (e.type == SDL_KEYUP)
	{
		switch (e.key.keysym.sym)
		{
			case SDLK_d: mRightRelease();  break;
			case SDLK_a: mLeftRelease();  break;
		}
	}
}

void Player::step()
{
	//apply gravity if not already touching floor
	if (!(mTouchIndex & kTouchingTop))
	{
		mYVelocityF += kGravityF;
		mYVelocity = mYVelocityF;	
	}

	mCheckClinging();
	move(mLevel->getRects());

	//makes character face the right way
	if (mXVelocity > 0) mFlipType = SDL_FLIP_NONE;
	else if (mXVelocity < 0) mFlipType = SDL_FLIP_HORIZONTAL;

	mBoundPlayer();

	mCollisionSquisher();

	handleAnimation();
	mAttack();
	//Makes a hurtbox and throws it into the level's hurtbox vector
	Hitbox hurtbox = { mCollisionBox , 0 };
	hurtbox.hitbox.x += mX;
	hurtbox.hitbox.y += mY;
	mLevel->addHitbox(hurtbox, true, false, true);

	if (mTouchIndex & kTouchingTop)
	{
		mJump = true;//replenishes doublejump if ground touched
		if (mYVelocity == 0)//if touching floor and not moving vertically
			mRestorePoint = { mX, mY };//sets the restore point in case of out of bounds, will replace when checkpoints are introduced
	}
}

void Player::move(vector<SDL_Rect>& colliders)
{


	//begin actual collision checking
	int offsetX = mCollisionBox.x;//the offset of the collision box
	int offsetY = mCollisionBox.y;//the offset of the collision box
	SDL_Rect player = { mX + offsetX, mY + offsetY, mCollisionBox.w, mCollisionBox.h };


	//checks for collision and moves accordingly
	//also modifies the y velocity if needed
	SDL_Point optimalPoint = optimizedMove(player, colliders, mXVelocity, mYVelocity, mTouchIndex);
	mX = optimalPoint.x - offsetX;
	mY = optimalPoint.y - offsetY;

	if ((mTouchIndex & (kTouchingBottom)) 
		&& mYVelocityF <= -0.8)//if your head bops (or is bopping) the bottom of a tile
	{
	//note that this exponentially reduces the velocity rather than instantly reducing it to 0
	//i think it's better and more fluid
		mYVelocityF = .5 * mYVelocityF; //reduce speed to 0
		mYVelocity = mYVelocityF;
	}
}


void Player::render(int x, int y)
{
	mTexture->renderTexture(this->mX - x, this->mY - y, 
		&mAnimationFrameRectangles[mAnimationFrame], 0.0, NULL, mFlipType);
	
	//Renders the Crescent-shaped slice effect when needed
	//Note that it would be overlapped if rendered during move()
	if (mAttackCoolDown >= kSwordLag - kSwordFrames)
	{
		if (mFlipType == SDL_FLIP_NONE)
			x -= kStandardCollisionBox.x + kStandardCollisionBox.w / 2;
		else 
			x += kStandardCollisionBox.x + kStandardCollisionBox.w / 2;

		mTexture->renderTexture(this->mX - x, this->mY - y,
			&mAnimationFrameRectangles[kEffectSlice], 0.0, NULL, mFlipType);
	}
}

void Player::handleAnimation()
{
	if (mTouchIndex & kTouchingTop //if touching the ground
		|| (mYVelocity <= 2  && mJump) && !mWallClinging && mXVelocity != 0) //or running off an edge (but not falling too fast) (allows running through gaps seamlessly)
	{
		if (mXVelocity != 0
			&& !(mTouchIndex & kTouchingRight)
			&& !(mTouchIndex & kTouchingLeft))
		{
			if (mAttackCoolDown == 0)
			{
				if (mAnimationFrame >= kWalkStart
					&& mAnimationFrame < kWalkEnd)
					++mAnimationFrame;
				else mAnimationFrame = kWalkStart;
			}
			else
			{
				if (mAnimationFrame >= kWalkStart
					&& mAnimationFrame < kWalkEnd)//If the ninja was already running
					mAnimationFrame += kWalkSlashStart - kWalkStart;
				else if (mAnimationFrame >= kWalkSlashStart
					&& mAnimationFrame < kWalkSlashEnd)//If the ninja was already running
					++mAnimationFrame;
				else mAnimationFrame = kWalkSlashStart;
			}
		}
		else if (mAttackCoolDown == 0)
			mAnimationFrame = kStanding;
		else mAnimationFrame = kStandSlash;
	}
	else if (mWallClinging)
		mAnimationFrame = kWallCling;
	else if (mAnimationFrame >= kJumpStart && mAnimationFrame < kJumpEnd)
	{
		++mAnimationFrame;
	}
	else mAnimationFrame = kJumpStart;
}

void Player::setLevel(Level* level)
{
	mLevel = level;
}


void Player::setposition(int x, int y)
{
	mX = x;
	mY = y;
}

int Player::getX()
{
	return this->mX;
}

int Player::getY()
{
	return this->mY;
}

int Player::getHealth()
{
	return mHealth;
}


void Player::mLeftPress()
{
	mXVelocity -= kMovementSpeed;
}

void Player::mJumpPress()
{
	if (mJump && !(mTouchIndex & kTouchingBottom))//if you can jump, you will jump
	{
		mJump = false;
		mYVelocityF = -kJumpVelocity;
		mYVelocity = mYVelocityF;
	}
	//NO FUN ALLOWED MODE, FINITE JUMPS

	/*
	if (!(mTouchIndex & kTouchingBottom))
	{
		mYVelocityF = -kJumpVelocity;
		mYVelocity = mYVelocityF;
	}
	*/
	//LOTS AND LOTS OF FUN MODE
}

void Player::mRightPress()
{
	mXVelocity += kMovementSpeed;
}

void Player::mAddHurtbox()
{
}

void Player::mCheckClinging()
{
	if ((mTouchIndex & (kTouchingRight | kTouchingLeft))//if touching a wall
		&& !(mTouchIndex & kTouchingTop)// and not touching the floor
		&& mYVelocity >= 0//and not moving upwards
		&& mXVelocity != 0)//and holding the direction towards the wall (holding away would un-cling regardless)
	{
		mWallClinging = true;
		mJump = true;
		mYVelocityF = 1; //friction agaist wall
		mYVelocity = mYVelocityF;
	}
	else mWallClinging = false;
}

void Player::mCollisionSquisher()
{
		//collision box jump squisher
	if ((mTouchIndex == 0 )  && !mAirborne)//if touching nothing i.e. airborne or jumping
	{
		mAirborne = true;
		mCollisionBox = kJumpingCollisionBox;//set current collisionbox to the airborne one
	}
	else if ((mTouchIndex & kTouchingTop) && mAirborne)
	{//will only change the collision box once, rather than constantly applying the same one
		mAirborne = false;
		mCollisionBox = kStandardCollisionBox;
		if (kJumpingCollisionBox.y + kJumpingCollisionBox.h < kStandardCollisionBox.y + kStandardCollisionBox.h)
		{//dont need to have this check, since I'm probably not going to change the collision boxes
			mY += (kJumpingCollisionBox.y +  kJumpingCollisionBox.h) - (kStandardCollisionBox.y + kStandardCollisionBox.h);
			mYVelocityF = 0;
			mYVelocity = 0;
		}
	}//end box squisher
}

void Player::mBoundPlayer()
{
	//make sure the player does not accelerate indefinitely
	if (mYVelocity >= kTerminalVelocity)
	{
		mYVelocityF = kTerminalVelocity;
		mYVelocity = mYVelocityF;
	}

	//level boundaries, so the ninja doesn't go out the map
	if (mLevel != NULL)
	{
		SDL_Rect dimensions = mLevel->getLevelDimensions();
		
		if (mX + mCollisionBox.x < dimensions.x)
			mX = dimensions.x - mCollisionBox.x;
		if (mX + mCollisionBox.x + mCollisionBox.w > dimensions.w)
			mX = dimensions.w - mCollisionBox.x - mCollisionBox.w;
		if (mY > dimensions.h)
		{
			mX = mRestorePoint.x;
			mY = mRestorePoint.y;
			//kill the player
		}
	}
}

void Player::mAttack()
{
	if (mAttackCoolDown >= kSwordLag - kSwordFrames)
	{
		if (mFlipType == SDL_FLIP_NONE)
		{
			Hitbox temp = mSwordHitbox;
			temp.hitbox.x += mX + kStandardCollisionBox.x + kStandardCollisionBox.w / 2;// centered and to the right, for a right-facing ninja
			temp.hitbox.y += mY;
			mLevel->addHitbox(temp, true, true, false);
		}
		else
		{
			Hitbox temp = mSwordHitbox;
			temp.hitbox.x += mX + kStandardCollisionBox.x + kStandardCollisionBox.w / 2;//centered to the right for now, but to be changed
			temp.hitbox.x -= mSwordHitbox.hitbox.x * 2 + mSwordHitbox.hitbox.w;//moved to the left now
			temp.hitbox.y += mY;
			mLevel->addHitbox(temp, true, true, false);

		}
	}
	mAttackCoolDown = (mAttackCoolDown > 0) ? mAttackCoolDown - 1 : mAttackCoolDown; //lowers the mAttackCoolDown if greater than 0
}

void Player::mAttackPress()
{
	if (mAttackCoolDown == 0)
	{
		mAttackCoolDown = kSwordLag;
	}
}


void Player::mLeftRelease()
{
	this->mXVelocity += kMovementSpeed;
}

void Player::mRightRelease()
{
	this->mXVelocity -= kMovementSpeed;
}
