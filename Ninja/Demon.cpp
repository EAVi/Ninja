#include "Demon.h"

using namespace std;

//initiate static variables
LTexture* Demon::kDemon_Texture = NULL;
vector<SDL_Rect> Demon::kDemon_AnimationClips = vector<SDL_Rect>();
Hitbox Demon::kDemon_ProjectileCollision = { { 9, 13, 12, 7 }, 10, 14 };


Demon::Demon()
{
	mMaxHealth = kDemon_MaxHealth;
	mHealth = mMaxHealth;
	mProjectiles = vector<SDL_Rect>();
	mFireballCooldown = 0;
	mStyle = kRun;
	mFlyWall = false;
}

Demon::~Demon()
{
}

Demon::Demon(Uint8 x, Uint8 y, bool right, Level * level)
	: Enemy(x, y, right, level)
{
	mMaxHealth = kDemon_MaxHealth;
	mHealth = mMaxHealth;
	mProjectiles = vector<SDL_Rect>();
	mFireballCooldown = 0;
	mStyle = kRun;
	mFlyWall = false;
}

void Demon::step(std::vector<SDL_Rect>& colliders)
{
	if (!checkActive()) return; //if you want the enemy to only start "existing" when it collides with the camera, have this line of code
	mPlayerDeathGimmick();
	if (mHitStun <= 0 && mHealth > 0)//cant move if you're stunned
	{
		mCheckStyle();
		mXV = mDirectionRight ? kDefault_WalkSpeed : -kDefault_WalkSpeed;
		attack();
		checkHurt();//shouldn't get attacked in hitstun
		if (mFlyWall && mStyle == kFly)
			flyMove(colliders, kDefault_OffsetCollisionBox);
		else
		{
			move(colliders, kDefault_OffsetCollisionBox);
			//increment mYV
			if (mTouchIndex & kTouchingTop)
				mYV = 0;
			else if (mYV < kDefault_TerminalVelocity)
				mYV++;
		}

		//when a wall is touched
		if (mTouchIndex & (kTouchingLeft | kTouchingRight))
			if (mStyle == kRun) mDirectionRight = !mDirectionRight;//if running, turn around
			else mFlyWall = true;//if flying, ride the wall
	}
	else
	{

		if (mHitStun > 0)
			--mHitStun;

		if (mHealth > 0)
			attack();

		if (((mHitStun == 1) || (mHitStun == 9) || (mHitStun == 17)) && mHealth)//condition for a triple shot
		{
			mProjectiles.push_back({ mX, mY, -kDemon_ProjectileSpeed, 0 });
			mProjectiles.push_back({ mX, mY, -kDemon_ProjectileSpeed, -1 });
			mProjectiles.push_back({ mX, mY, -kDemon_ProjectileSpeed, -2 });
			mProjectiles.push_back({ mX, mY, kDemon_ProjectileSpeed, 0 });
			mProjectiles.push_back({ mX, mY, kDemon_ProjectileSpeed, -1 });
			mProjectiles.push_back({ mX, mY, kDemon_ProjectileSpeed, -2 });
		}

		mXV = 0;
		mYV++;
		move(colliders, kDefault_OffsetCollisionBox);
	}

	if (mHealth > 0 || mHitStun <= 0)
		handleAnimation();

	//on death, create a door to the next level
	if (mHealth <= 0 && mAnimationFrame == kDeathEnd - 1)
	{
		mLevel->createDoor({ {mX,mY, 16,32},255,255,255,1});
	}
}

void Demon::step()
{
	if (mLevel == NULL) return;//would have issues otherwise
	step(mLevel->getRects());
}

void Demon::attack()
{
	if (mLevel == NULL) return; //Enemies aren't always bound to a level

	//Handle projectiles
	if (mFireballCooldown == 0)
	{
		if (mDirectionRight)
			mProjectiles.push_back({ mX, mY, -kDemon_ProjectileSpeed, 0 });//shoot a fireball to the left if facing to the right
		else
			mProjectiles.push_back({ mX, mY, kDemon_ProjectileSpeed, 0 });//shoot a fireball to the right if facing to the left

		//cooldown will be a function of health, the lower health will increase firing rate by a maximum of 5
		mFireballCooldown = kDemon_ProjectileCooldown - (8 * kDemon_ProjectileCooldown * (kDemon_MaxHealth - mHealth) / kDemon_MaxHealth / 10);
	}

	for (int i = 0; i < (int)mProjectiles.size(); ++i)
	{
		Hitbox hitbox = kDemon_ProjectileCollision;
		mProjectiles[i].x += mProjectiles[i].w;
		mProjectiles[i].y += mProjectiles[i].h;
		hitbox.hitbox.x += mProjectiles[i].x;
		hitbox.hitbox.y += mProjectiles[i].y;

		//whether to continue processing the current star or not
		bool finish = true;

		//checking destruction conditions
		if (checkCollision(hitbox.hitbox, mLevel->getRects())
			|| !checkCollision(hitbox.hitbox, mLevel->getCamera()))
		{
			//destroy the fireball if touching a collision box, or outside the map

			mProjectiles.erase(mProjectiles.begin() + i);
			--i;//decrement since the vector was reduced by 1
			continue;
		}


		//check reflection conditions
		for (int j = 0, k = mLevel->getPlayerHitboxes().size(); j < k; ++j)
		{
			if (checkCollision(hitbox.hitbox, mLevel->getPlayerHitboxes()[j].hitbox)//if the projectile collides with a player hitbox
				&& absValue(mProjectiles[i].w) == kDemon_ProjectileSpeed) //if the projectile is not already at a deflected velocity
			{
				mProjectiles[i].w *= -2; //make the projectile go in opposite direction at double speed

				if (mLevel->getPlayerHitboxes()[j].hitstun < 8)//if the deflecting hitbox has a low hitstun (weak attack), delete the projectile
				{
					mProjectiles.erase(mProjectiles.begin() + i);
					--i;//decrement since the vector was reduced by 1
					finish = false;
					break; //end this loop
				}
			}
		}

		if (finish
			&& absValue(mProjectiles[i].w) == kDemon_ProjectileSpeed) //add the hurtbox if the projectile isn't deflected
			mLevel->addHitbox(hitbox, false, true, false);

	}

	Hitbox hitbox = kDefault_OffsetHitbox;
	hitbox.hitbox.x += mX;
	hitbox.hitbox.y += mY;

	mHurtbox = hitbox.hitbox;
	//passive hitbox, (also a hurtbox, technically)
	mLevel->addHitbox(hitbox, false, true, true);

	//reduce fireball cooldown
	mFireballCooldown = (mFireballCooldown <= 0) ? mFireballCooldown = 0 : mFireballCooldown - 1;
}

void Demon::render(int xD, int yD)
{
	if (!mActive) return; // do not render an inactive Enemy

	//handle projectiles
	for (int i = 0, j = mProjectiles.size(); i < j; ++i)
	{
		SDL_RendererFlip ftype = SDL_FLIP_NONE;
		if (mProjectiles[i].w < 0)
			ftype = SDL_FLIP_HORIZONTAL;

		kDemon_Texture->renderTexture(
			mProjectiles[i].x - xD,
			mProjectiles[i].y - yD,
			&kDemon_AnimationClips[kFireballStart + mFireballCooldown % (kFireballEnd - kFireballStart)],
			0.0,
			NULL,
			ftype
		);
	}

	SDL_RendererFlip ftype = SDL_FLIP_NONE;
	if (!mDirectionRight)
		ftype = SDL_FLIP_HORIZONTAL;
	if (kDefault_Texture != NULL)//will only render if the pointer is real
	{
		kDemon_Texture->renderTexture(
			mX - xD,
			mY - yD,
			&kDemon_AnimationClips[mAnimationFrame],
			0.0,
			NULL,
			ftype);
	}
}

void Demon::checkHurt()
{
	//the only thing that can hurt the demon is it's own fireballs reflected back at itself
	for (int i = 0, j = mProjectiles.size(); i < j; ++i)
	{
		if (absValue(mProjectiles[i].w) != kDemon_ProjectileSpeed) //if the fireball is deflected
		{
			SDL_Rect temp = kDemon_ProjectileCollision.hitbox;
			temp.x += mProjectiles[i].x;
			temp.y += mProjectiles[i].y;

			if (checkCollision(mHurtbox, temp))//if the deflected fireball collides with self
			{
				mFlyWall = false;
				//Destroy the fireball and hurt the Demon
				mProjectiles.erase(mProjectiles.begin() + i);
				--i;//decrement since the vector was reduced by 1
				mHealth -= kDemon_ProjectileCollision.damage;
				mHitStun += kDemon_PowerupTime;
				mStyle = kPowerup;
				break; //end this loop
			}
		}
	}

	//some checks to prevent under/overflow
	if (mHealth <= 0)
	{
		mHealth = 0;
		mAnimationFrame = kDeathStart;
	}
	if (mHealth > mMaxHealth) mHealth = mMaxHealth;

}

void Demon::handleAnimation()
{
	if (mHealth <= 0)//dying animation
	{
		if (mAnimationFrame > kDeathEnd || mAnimationFrame < kDeathStart)
			//if not already dying, start dying
			mAnimationFrame = kDeathStart;
		//else, currently in the middle of dying animation, keep it up
		else ++mAnimationFrame;
	}

	else if (mStyle == kRun)//running animation
	{
		if (mAnimationFrame >= kRunEnd || mAnimationFrame < kRunStart)
			//if not already walking, start walking
			mAnimationFrame = kRunStart;
		//else, currently in the middle of walking animation, keep it up
		else ++mAnimationFrame;
	}

	else if (mStyle == kPowerup)//powerup animation
	{
		if (mAnimationFrame >= kPowerupEnd || mAnimationFrame < kPowerupStart)
			mAnimationFrame = kPowerupStart;
		else ++mAnimationFrame;

		//this animation is the beginning of the charge animation, which is a loop
		if (mAnimationFrame >= kPowerupEnd)
			mStyle = kCharge;
	}

	else if (mStyle == kFly)//flying animation
	{
		if (mAnimationFrame >= kFlyEnd || mAnimationFrame < kFlyStart)
			mAnimationFrame = kFlyStart;
		else ++mAnimationFrame;
	}

	else if (mStyle == kCharge)//charge animation
	{
		if (mAnimationFrame >= kChargeEnd || mAnimationFrame < kChargeStart)
			mAnimationFrame = kChargeStart;
		else ++mAnimationFrame;
	}
}

void Demon::setTexture(LTexture * texture)
{
	kDemon_Texture = texture;

	kDemon_AnimationClips =
		spriteClipper(texture->getWidth(),
			texture->getHeight(),
			{ 0, 0, kDefault_ClipW, kDefault_ClipH });
}

bool Demon::checkLiving()
{
	return (mAnimationFrame != kDeathEnd);
}

void Demon::flyMove(std::vector<SDL_Rect>& colliders, SDL_Rect & offBox)
{
	mXV = 0;
	Uint8 ycenter = mY + kDefault_ClipH / 2;
	if (mLevel != NULL)
	{
		if (ycenter < mLevel->getPlayerPosition().y)
			mYV = kDefault_WalkSpeed;
		else if (ycenter > mLevel->getPlayerPosition().y)
			mYV = -kDefault_WalkSpeed;
		else if (ycenter == mLevel->getPlayerPosition().y)
			mYV = 0;
	}
	moveRocket();
}

void Demon::mCheckStyle()
{
	if ((mHealth / 10 % 2) == 0)
		mStyle = kRun;
	else mStyle = kFly;
}

void Demon::mPlayerDeathGimmick()
{

	if (mLevel->checkPlayerDead()
		&& (mLevel->getLevelID().ZoneNo != 8))
	{
		//notice that the door lead to level 255, 
		//when a level above the zone mlevel.size is given, it counts as a zone completed (victory)
		mLevel->createDoor({ {0,0,1023,1023}, 255, 255, 255, 255 });
	}
}
