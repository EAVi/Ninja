#include "Enemy.h"
#include "Level.h"
#include <iostream>

using namespace std;

LTexture* Enemy::kDefault_Texture = NULL;
vector<SDL_Rect> Enemy::kDefault_AnimationClips = vector<SDL_Rect>(0);
SDL_Rect Enemy::kDefault_OffsetCollisionBox = { 0,0,32,32 };
SDL_Rect Enemy::kDefault_OffsetHitbox = { 10,0,12,32 };



Enemy::Enemy()
{
	mMaxHealth = kDefault_MaxHealth;
	mHealth = mMaxHealth;
	mAnimationFrame = 0;
	mXV = 0;
	mYV = 0;
	mX = 0;
	mY = 0;
	mActive = false;
	mDirectionRight = false;
	mRewindAnimation = false;
	//mOddFrame = false;
	mLevel = NULL;
	mHitStun = 0;
}

Enemy::~Enemy()
{
/*
According to dumb rules of c++, you can't destroy an inherited
object without a virtual destructor, so here it is, 
a whole lot of nothing happening in this destructor
*/
}

Enemy::Enemy(Uint8 x, Uint8 y, bool right, Level* level)
{
	mMaxHealth = kDefault_MaxHealth;
	mHealth = mMaxHealth;
	mAnimationFrame = 0;
	mXV = 0;
	mYV = 0;
	mX = x * 16;
	mY = y * 16;
	mActive = false;
	mDirectionRight = right;
	mRewindAnimation = false;
	//mOddFrame = false;
	mLevel = level;
	mHitStun = 0;
	mCollisionBox = kDefault_OffsetCollisionBox;//set the collision box;
	mCollisionBox.x += mX;
	mCollisionBox.y += mY;
}

void Enemy::step(vector<SDL_Rect>& colliders)
{	
	if (!checkActive()) return; //if you want the enemy to only start "existing" when it collides with the camera, have this line of code

	if (mHitStun <= 0 && mHealth > 0)//cant move if you're stunned
	{
		//mYV += kDefault_Gravity;
		mXV = mDirectionRight ? kDefault_WalkSpeed : -kDefault_WalkSpeed;
		attack();//shouldn't attack in hitstun
		checkHurt();//shouldn't get attacked in hitstun
		moveEdge(colliders, kDefault_OffsetCollisionBox);
		//Change direction when a wall is touched
		if (mTouchIndex & (kTouchingLeft | kTouchingRight)) mDirectionRight = !mDirectionRight;
	}
	else --mHitStun;
	
	handleAnimation();

}

void Enemy::step()
{
	if (mLevel == NULL) return;//would have issues otherwise
	step(mLevel->getRects());
}

void Enemy::move(vector<SDL_Rect>& colliders, SDL_Rect& offBox)
{
	SDL_Rect enemy = { mX + offBox.x, mY + offBox.y, offBox.w, offBox.h };

	//checks for collision and moves accordingly
	//also modifies the y velocity if needed
	SDL_Point optimalPoint = optimizedMove(enemy, colliders, mXV, mYV, mTouchIndex);
	mX = optimalPoint.x - offBox.x;
	mY = optimalPoint.y - offBox.y;

	if (mYV >= kDefault_TerminalVelocity)
		mYV = kDefault_TerminalVelocity;

}

void Enemy::moveRocket()
{
	mX += mXV;
	mY += mYV;
}

void Enemy::moveEdge(std::vector<SDL_Rect>& colliders, SDL_Rect & offBox)
{
	mTouchIndex = 0;

	mX += mXV;
	mY += mYV;

	SDL_Rect enemy = { mX + offBox.x, mY + offBox.y, offBox.w, offBox.h };

	if (checkEdges(enemy, colliders))
	{
		mTouchIndex = 255;
	}
}

bool Enemy::checkActive()
{
	if (mActive) return true;
	else
	{
		mActive = checkCollision(mCollisionBox,mLevel->getCamera());
		return mActive;
	}
}

void Enemy::setLevel(Level * level)
{
	mLevel = level;
}

void Enemy::attack()
{
	if (mLevel == NULL) return; //Enemies aren't always bound to a level
	//does nothing so far
	Hitbox hurtbox = { kDefault_OffsetCollisionBox , 0};
	hurtbox.hitbox.x += mX;
	hurtbox.hitbox.y += mY;
	mHurtbox = hurtbox.hitbox;

	Hitbox hitbox = { kDefault_OffsetHitbox, 4 };
	hitbox.hitbox.x += mX;
	hitbox.hitbox.y += mY;

	mLevel->addHitbox(hurtbox, false, false, true);
	mLevel->addHitbox(hitbox, false, true, false);

}



void Enemy::render(int xD, int yD)
{
	SDL_RendererFlip ftype = SDL_FLIP_NONE;
	if (!mDirectionRight)
		ftype = SDL_FLIP_HORIZONTAL;
	if (kDefault_Texture != NULL)//will only render if the pointer is real
	{
		kDefault_Texture->renderTexture(
			mX - xD,
			mY - yD, 
			&kDefault_AnimationClips[mAnimationFrame],
			0.0,
			NULL,
			ftype);
	}
}

void Enemy::checkHurt()
{
	vector<Hitbox>& temp = mLevel->getPlayerHitboxes();
	for (int i = 0, j = temp.size(); i < j; ++i)
	{
		if (checkCollision(mHurtbox, temp[i].hitbox))
		{
			mHealth -= temp[i].damage;
			mHitStun = kDefault_HitStunFrames;
		}
	}

	//some checks to prevent under/overflow
	if (mHealth < 0) mHealth = 0;
	if (mHealth > mMaxHealth) mHealth = mMaxHealth;
}

void Enemy::handleAnimation()
{
	//if (mOddFrame)//this makes it so that the ghosts are only 30FPS
	if (mHealth <= 0)//dying animation
	{
		if (mAnimationFrame > kDeathEnd || mAnimationFrame < kDeathStart)
		{
			mRewindAnimation = false;
			mAnimationFrame = kDeathStart;
		}
	}

	else if (mAnimationFrame > kRunEnd || mAnimationFrame < kRunStart)
	{
		mRewindAnimation = false;
		mAnimationFrame = kRunStart;
	}
	else // if already running
	{
		if (mAnimationFrame == kRunEnd) //goes backwards if it reached the end
			mRewindAnimation = true;

		if (mAnimationFrame == kRunStart)//goes forward if it reaches the beginning again
			mRewindAnimation = false;
	}

	mAnimationFrame = (mRewindAnimation) ?
		mAnimationFrame - 1 : mAnimationFrame + 1;
	//mOddFrame = !mOddFrame;
}

void Enemy::setTexture(LTexture* texture)
{
	kDefault_Texture = texture;
	
	kDefault_AnimationClips = 
		spriteClipper(texture->getWidth(), 
		texture->getHeight(), 
		{ 0, 0, kDefault_ClipW, kDefault_ClipH });
}

bool Enemy::checkLiving()
{
	if (mAnimationFrame == kDeathEnd)
		return false;
	return true;
}


