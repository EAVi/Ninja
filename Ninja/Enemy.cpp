#include "Enemy.h"
#include "Level.h"
#include <iostream>

using namespace std;

LTexture* Enemy::kDefault_Texture = NULL;
vector<SDL_Rect> Enemy::kDefault_AnimationClips = vector<SDL_Rect>(0);
SDL_Rect Enemy::kDefault_OffsetCollisionBox = { 0,0,32,32 };



Enemy::Enemy()
{
	mHealth = kDefault_MaxHealth;
	mAnimationFrame = 0;
	mXV = 0;
	mYV = 0;
	mX = 0;
	mY = 0;
	mDirectionRight = false;
	mRewindAnimation = false;
	mOddFrame = false;
	mLevel = NULL;
}

Enemy::Enemy(Uint8 x, Uint8 y, bool right, Level* level)
{
	mHealth = kDefault_MaxHealth;
	mAnimationFrame = 0;
	mXV = 0;
	mYV = 0;
	mX = x * 16;
	mY = y * 16;
	mDirectionRight = right;
	mRewindAnimation = false;
	mOddFrame = false;
	mLevel = level;

}

void Enemy::step(vector<SDL_Rect>& colliders)
{
	mYV += kDefault_Gravity;
	mXV = mDirectionRight ? kDefault_WalkSpeed : -kDefault_WalkSpeed;
	
	attack();
	checkHurt();
	
	if (mHitStun <= 0 && mHealth > 0)//cant move if you're stunned
	{
		move(colliders, kDefault_OffsetCollisionBox);
		handleAnimation();
	}
	else mHitStun--;
	
	//Change direction when a wall is touched
	if (mTouchIndex & (kTouchingLeft | kTouchingRight)) mDirectionRight = !mDirectionRight;
}

void Enemy::step()
{
	if (mLevel == NULL) return;//would have issues otherwise
	step(mLevel->getRects());
}

void Enemy::move(vector<SDL_Rect>& colliders, SDL_Rect& offBox)
{
	int offsetX = offBox.x;//the offset of the collision box
	int offsetY = offBox.y;//the offset of the collision box
	SDL_Rect enemy = { mX + offsetX, mY + offsetY, offBox.w, offBox.h };


	//checks for collision and moves accordingly
	//also modifies the y velocity if needed
	SDL_Point optimalPoint = optimizedMove(enemy, colliders, mXV, mYV, mTouchIndex);
	mX = optimalPoint.x - offsetX;
	mY = optimalPoint.y - offsetY;

	if (mYV >= kDefault_TerminalVelocity)
		mYV = kDefault_TerminalVelocity;

}

void Enemy::setLevel(Level * level)
{
	mLevel = level;
}

void Enemy::attack()
{
	if (mLevel == NULL) return; //Enemies aren't always bound to a level
	//does nothing so far
	Hitbox mHybridBox = { kDefault_OffsetCollisionBox , 4};
	mHybridBox.hitbox.x += mX;
	mHybridBox.hitbox.y += mY;
	mHurtbox = mHybridBox.hitbox;

	mLevel->addHitbox(mHybridBox, false, true, true);
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
	vector<Hitbox> temp = mLevel->getPlayerHitboxes();
	for (int i = 0, j = temp.size(); i < j; ++i)
	{
		if (checkCollision(mHurtbox, temp[i].hitbox))
		{
			mHealth -= temp[i].damage;
			mHitStun = kDefault_HitStunFrames;
		}
	}
}

void Enemy::handleAnimation()
{
	if (mOddFrame)//this makes it so that the ghosts are only 30FPS 
	{

		if (mAnimationFrame > kRunEnd || mAnimationFrame < kRunStart)
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
	}
	mOddFrame = !mOddFrame;
}

void Enemy::setTexture(LTexture* texture)
{
	kDefault_Texture = texture;
	
	kDefault_AnimationClips = 
		spriteClipper(texture->getWidth(), 
		texture->getHeight(), 
		{ 0, 0, kDefault_ClipW, kDefault_ClipH });
}


