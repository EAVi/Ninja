#include "Player.h"

using namespace std;
SDL_Rect Player::kStandardCollisionBox = { 10,0,12,32 };
SDL_Rect Player::kJumpingCollisionBox = { 10,9,12,17 };
float Player::kGravityF = 0.5;

Player::Player() 
{
	this->mTexture = NULL;
	mAnimationFrameRectangles.clear();
	mCollisionBox = kStandardCollisionBox;
	mX = 0;
	mY = 0;
	mXVelocity = 0;
	mYVelocityF = 0.0;
	mLeftKeyDown = false;
	mRightKeyDown = false;
	mYVelocity = mYVelocityF;
	mFlipType = SDL_FLIP_NONE;
	mAnimationFrame = 0;
	mAttackCoolDown = 0;
	mStarCooldown = 0;
	mSwordHitbox = { { 6,-1,24,27 }, 7, 13 };
	mStarHitbox = { { 13,14,6,4 }, 3, 7 };
	mMaxHealth = kMaxHealth;
	mHealth = mMaxHealth;
	resetLives();
	mInvincibilityFrames = 0;
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
	if (e.type == SDL_KEYDOWN && e.key.repeat == 0)//keyboard press
	{
		switch (e.key.keysym.sym)
		{
		case SDLK_d: mRightPress(); break;
		case SDLK_a: mLeftPress();  break;
		case SDLK_SPACE: mJumpPress(); break;
		case SDLK_m: mAttackPress(); break;
		case SDLK_n: mProjectilePress(); break;
		}
	}
	else if (e.type == SDL_KEYUP)//keyboard release
	{
		switch (e.key.keysym.sym)
		{
		case SDLK_d: mRightRelease();  break;
		case SDLK_a: mLeftRelease();  break;
		}
	}
	else if (e.type == SDL_CONTROLLERBUTTONDOWN)//controller press
	{
		switch (e.cbutton.button)
		{
		case SDL_CONTROLLER_BUTTON_A: mJumpPress(); break;
		case SDL_CONTROLLER_BUTTON_X: mAttackPress(); break;
		case SDL_CONTROLLER_BUTTON_B: mProjectilePress(); break;
		case SDL_CONTROLLER_BUTTON_DPAD_LEFT: mLeftPress(); break;
		case SDL_CONTROLLER_BUTTON_DPAD_RIGHT: mRightPress(); break;
		}

	}
	else if (e.type == SDL_CONTROLLERBUTTONUP)//controller release
	{
		switch (e.cbutton.button)
		{
		case SDL_CONTROLLER_BUTTON_DPAD_LEFT: mLeftRelease(); break;
		case SDL_CONTROLLER_BUTTON_DPAD_RIGHT: mRightRelease(); break;
		}
	}
	else if (e.type == SDL_CONTROLLERAXISMOTION )//controller analog (can include triggers)
	{
		if (e.caxis.axis == SDL_CONTROLLER_AXIS_LEFTX)
		{
			if (e.caxis.value < -8000)//8000 is the deadzone, will have a variable for it soon
				mAxisLeft();
			else if (e.caxis.value > 8000)
				mAxisRight();
			else mAxisNone();
		}
	}
}

void Player::step()
{

	if (mHitStun <= 0)
	{
		//apply gravity if not already touching floor
		if (!(mTouchIndex & kTouchingTop))
		{
			mYVelocityF += kGravityF;
			mYVelocity = mYVelocityF;	
		}

		mCheckClinging();
		move(mLevel->getRects());

		if (mHealth > 0)//you shouldn't turn around if you're dying
		{
			//makes character face the right way
			if (mXVelocity > 0) mFlipType = SDL_FLIP_NONE;
			else if (mXVelocity < 0) mFlipType = SDL_FLIP_HORIZONTAL;
		}

		mBoundPlayer();

		mCollisionSquisher();

		handleAnimation();//handles animation
		mAttack();//NOTE that this is not the attack event handler, that would be mAttackPress()
		mAddHurtbox();//pushes a hurtbox into the mLevel->mPlayerHurtbox vector.
	}
	else --mHitStun;

	if (mTouchIndex & kTouchingTop)
	{
		mJump = true;//replenishes doublejump if ground touched
	}
}

void Player::endstep()
{
	mInvincibilityFrames = (mInvincibilityFrames != 0) ? mInvincibilityFrames - 1 : mInvincibilityFrames;
	if (mHitStun == 0)
		mCheckHurt();
	else //stun animation overrides all
	{
		if (mAttackCoolDown <= 0 && mTouchIndex && kTouchingTop)
			mAnimationFrame = kStandStun;
		else if (!(mTouchIndex && kTouchingTop))
			mAnimationFrame = kAirStun;
		else mAnimationFrame = kStandStunSword;
	}
	if (mHitStun == 1)//last frame of hitstun, apply invincibility frames
		mInvincibilityFrames = kInvincibilityFrames + 1;

}

void Player::move(vector<SDL_Rect>& colliders)
{


	//begin actual collision checking
	int offsetX = mCollisionBox.x;//the offset of the collision box
	int offsetY = mCollisionBox.y;//the offset of the collision box
	SDL_Rect player = { mX + offsetX, mY + offsetY, mCollisionBox.w, mCollisionBox.h };


	//checks for collision and moves accordingly
	//also modifies the y velocity if needed
	SDL_Point optimalPoint;
	if (mHealth > 0)//if living, can move freely
		optimalPoint = optimizedMove(player, colliders, mXVelocity, mYVelocity, mTouchIndex);
	else//if dead, use zero for xVelocity
	{
		int zero = 0;
		optimalPoint = optimizedMove(player, colliders, zero, mYVelocity, mTouchIndex);
	}

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
	//Do not render on the blinking frames
	if (((mInvincibilityFrames + 1) % kBlinkingFrequency == 0)
		&& mHealth != 0 //don't want to blink on death frames
		)
		return;

	//Render the Player
	mTexture->renderTexture(this->mX - x, this->mY - y, 
		&mAnimationFrameRectangles[mAnimationFrame], 0.0, NULL, mFlipType);
	
	//Renders the Crescent-shaped slice effect when needed
	//Note that it would be overlapped if rendered during move()
	if (mAttackCoolDown >= kSwordLag - kSwordFrames)
	{
		int tx = x;
		if (mFlipType == SDL_FLIP_NONE)
			tx -= kStandardCollisionBox.x + kStandardCollisionBox.w / 2;
		else 
			tx += kStandardCollisionBox.x + kStandardCollisionBox.w / 2;

		mTexture->renderTexture(this->mX - tx, this->mY - y,
			&mAnimationFrameRectangles[kEffectSlice], 0.0, NULL, mFlipType);
	}

	//Renders Ninja stars
	for (int i = 0, j = mStarPositions.size(); i < j; ++i)
	{
		mTexture->renderTexture(mStarPositions[i].x - x, mStarPositions[i].y - y, &mAnimationFrameRectangles[kStarEffectStart + absValue(mStarCooldown % 4)]);
	}
}

void Player::handleAnimation()
{
	//handle death animation
	if (mHealth <= 0)
	{
		if (mLives == 0) //For 0 lives, we have a special death animation
		{
			//if you're dead, keep it up
			if (mAnimationFrame >= kDeathStart && mAnimationFrame < kDeathEnd)
			{
				mAnimationFrame++;
			}
			//if not already dead, start dying
			else if (mAnimationFrame != kDeathEnd)
			{
				mInvincibilityFrames = 60;
				mAnimationFrame = kDeathStart;
			}
		}
		else //do the log death animation
		{
			//if you're dead, keep it up
			if (mAnimationFrame >= kLogDeathStart && mAnimationFrame < kLogDeathEnd)
			{
				mAnimationFrame++;
			}
			//if not already dead, start dying
			else if (mAnimationFrame != kLogDeathEnd)
			{
				mInvincibilityFrames = 60;
				mAnimationFrame = kLogDeathStart;
			}
		}
	}
	else if (mTouchIndex & kTouchingTop //if touching the ground
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

int& Player::getX()
{
	return this->mX;
}

int& Player::getY()
{
	return this->mY;
}

int& Player::getHealth()
{
	return mHealth;
}

int& Player::getMaxHealth()
{
	return mMaxHealth;
}

int & Player::getLives()
{
	return mLives;
}

SDL_Rect & Player::getCollisionBox()
{
	return mCollisionBox;
}

bool Player::checkDead()
{
	return ( ( mAnimationFrame == kDeathEnd || mAnimationFrame == kLogDeathEnd)
		&& mInvincibilityFrames <= 0);
}

void Player::resetLives()
{
	mLives = kStartingLives;
}

void Player::respawn()
{
	mLevel->getSpawnPoint(mX, mY);//passes by reference, modifies x,y position

	//if kStartingLives is zero, you would get a second game over screen, this will prevent that by making the player stand up
	if (mAnimationFrame == kDeathEnd) mAnimationFrame = kStanding;

}


void Player::mHandleDirection()
{
	if (mLeftKeyDown && mRightKeyDown)
		mXVelocity = 0;
	else if (mLeftKeyDown)
		mXVelocity = -kMovementSpeed;
	else if (mRightKeyDown)
		mXVelocity = kMovementSpeed;
	else mXVelocity = 0;
}

void Player::mLeftPress()
{
	mLeftKeyDown = true;
	mHandleDirection();
}

void Player::mJumpPress()
{
	//Can't jump if in hitstun or dead
	if (mHitStun > 0 || mHealth <= 0) return;
	if (mJump && !(mTouchIndex & kTouchingBottom))//if you can jump, you will jump
	{
		mJump = false;
		mYVelocityF = -kJumpVelocity;
		mYVelocity = mYVelocityF;
		LAudio::playSound(2);
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
	mRightKeyDown = true;
	mHandleDirection();
}

void Player::mAddHurtbox()
{
	//Makes a hurtbox and throws it into the level's hurtbox vector
	Hitbox hurtbox = { mCollisionBox , 0, 0 };
	hurtbox.hitbox.x += mX;
	hurtbox.hitbox.y += mY;
	mLevel->addHitbox(hurtbox, true, false, true);
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
			mYVelocity = mYVelocityF;
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
			respawn();
			mY += kStandardCollisionBox.y + kStandardCollisionBox.h - kJumpingCollisionBox.y - kJumpingCollisionBox.h;
			mYVelocityF = 0;
			mYVelocity = mYVelocityF;
			mTouchIndex |= kTouchingTop;
			//kill the player
			mHealth = 0;
		}
	}
}

void Player::mAttack()
{
	//Shuriken handling
	for (int i = 0; i < mStarPositions.size(); ++i)
	{
		Hitbox tempr = mStarHitbox;
		tempr.hitbox.x += mStarPositions[i].x;
		tempr.hitbox.y += mStarPositions[i].y;
		mStarPositions[i].x += mStarPositions[i].w;
		mStarPositions[i].y += mStarPositions[i].h;

		//checking destruction conditions
		if (checkCollision(tempr.hitbox, mLevel->getRects())
			|| !checkCollision(tempr.hitbox, mLevel->getLevelDimensions()))
		{
		
			mStarPositions.erase(mStarPositions.begin() + i);
			--i;//decrement since the vector was reduced by 1
		}
		//destroy the shuriken if outside the map
		mLevel->addHitbox(tempr, true, true, false);

	}

	//Sword handling
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
	mStarCooldown = (mStarCooldown > -3) ? mStarCooldown - 1 : 0; //the star cooldown may be below 0, but will cycle from -3 to 0 for animation purposes
}

void Player::mCheckHurt()
{
	if (mInvincibilityFrames > 0 //can't get hurt while invincible
		|| mHealth <= 0) return; //can't get hurt while already dead

	SDL_Rect hurtbox = mCollisionBox;
	hurtbox.x += mX;
	hurtbox.y += mY;
	
	int hdamage = 0;//highest damage
	int hhitstun = 0;//highest stun

	vector<Hitbox>& temp = mLevel->getEnemyHitboxes();
	for (int i = 0, j = temp.size(); i < j; ++i)
	{
		if (checkCollision(hurtbox, temp[i].hitbox))
		{
			if (temp[i].damage > hdamage)
				hdamage = temp[i].damage;
			if (temp[i].hitstun > hhitstun)
				hhitstun = temp[i].hitstun;
		}
	}

	mHealth -= hdamage;//chooses the highest damage
	mHitStun = hhitstun;//and the highest hitstun

	if (mHealth <= 0)
	{
		//mHitStun = 0;
		mHealth = 0;
	}

	if (hdamage > 0 && hhitstun == 0)//the way invincibility is handled, the player would otherwise be damage-able the next frame
		mInvincibilityFrames = kInvincibilityFrames;


	//some checks to prevent under/overflow
	if (mHealth < 0) mHealth = 0;
	if (mHealth > mMaxHealth) mHealth = mMaxHealth;
}

void Player::mAttackPress()
{
	//Not allowed to use sword while wallclinging or in hitstun
	if (mHitStun > 0) return;
	if (mWallClinging) return;
	if (mHealth <= 0) return;
	if (mAttackCoolDown == 0)
	{
		LAudio::playSound(1);//sword sound effect
		mAttackCoolDown = kSwordLag;
	}
}

void Player::mProjectilePress()
{
	//Not allowed to use shuriken while wallclinging or in hitstun
	if (mHitStun > 0) return;
	if (mWallClinging) return;
	if (mHealth <= 0) return;
	if (mStarCooldown > 0) return; //the star cooldown may be below 0, but will cycle from -3 to 0 for animation purposes

	if (mFlipType == SDL_FLIP_NONE)//if facing right, spawn a ninja star with a rightward trajectory
	{
		mStarPositions.push_back({ mX,mY,mStarSpeed,0 });
		mStarCooldown = kStarLag;
	}
	else if (mFlipType == SDL_FLIP_HORIZONTAL)//if facing left, spawn a ninja star with a leftward trajectory
	{
		mStarPositions.push_back({ mX,mY,-mStarSpeed,0 });
		mStarCooldown = kStarLag;
	}
}


void Player::mLeftRelease()
{
	mLeftKeyDown = false;
	mHandleDirection();
}

void Player::mRightRelease()
{
	mRightKeyDown = false;
	mHandleDirection();
}

void Player::mAxisLeft()
{
	mXVelocity = -kMovementSpeed;
}

void Player::mAxisRight()
{
	mXVelocity = kMovementSpeed;
}

void Player::mAxisNone()
{
	mXVelocity = 0;
}
