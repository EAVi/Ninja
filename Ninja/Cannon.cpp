#include "Cannon.h"

using namespace std;

LTexture* Cannon::kCannon_Texture = NULL;
vector<SDL_Rect> Cannon::kCannon_AnimationClips = vector<SDL_Rect>(0);
Hitbox Cannon::kCannon_ProjectileCollision = { {12,12,8,8},10,20 };


Cannon::Cannon()
	: Enemy()
{
	mMaxHealth = 0;//the cannon shall not take damage
	mHealth = mMaxHealth;
	mProjectileCooldown = kProjectileCooldown;
	mProjectile = vector<SDL_Rect>();
}

Cannon::Cannon(Uint8 x, Uint8 y, bool right, Level* level)
	: Enemy(x, y, right, level)
{
	mMaxHealth = 0;//the cannon shall not take damage
	mHealth = mMaxHealth;
	mProjectileCooldown = kProjectileCooldown;
}

Cannon::~Cannon()
{}

void Cannon::step(std::vector<SDL_Rect>& colliders)
{
	if (!checkActive()) return;
	attack();
	moveEdge(colliders, kDefault_OffsetCollisionBox);
	//Change direction when a wall is touched
	//if (mTouchIndex & (kTouchingLeft | kTouchingRight)) mDirectionRight = !mDirectionRight;
	handleAnimation();
}

void Cannon::step()
{
	if (mLevel == NULL) return;//would have issues otherwise
	step(mLevel->getRects());
}

void Cannon::attack()
{
	--mProjectileCooldown;
	if (mProjectileCooldown <= 0)
	{

		if (mDirectionRight)
			mProjectile.push_back({ mX, mY, kCannon_ProjectileSpeed, 0 });//shoot a fireball to the right if facing to the right
		else
			mProjectile.push_back({ mX, mY, -kCannon_ProjectileSpeed, 0 });//shoot a fireball to the left if facing to the left
		mProjectileCooldown = kProjectileCooldown;
	}

	for (int i = 0; i < (int)mProjectile.size(); ++i)
	{
		Hitbox hitbox = kCannon_ProjectileCollision;
		mProjectile[i].x += mProjectile[i].w;
		mProjectile[i].y += mProjectile[i].h;
		hitbox.hitbox.x += mProjectile[i].x;
		hitbox.hitbox.y += mProjectile[i].y;

		//whether to continue processing the current star or not
		bool finish = true;

		//checking destruction conditions
		if (checkCollision(hitbox.hitbox, mLevel->getRects())
			|| !checkCollision(hitbox.hitbox, mLevel->getCamera()))

		{
			//destroy the fireball if touching a collision box, or outside the map

			mProjectile.erase(mProjectile.begin() + i);
			--i;//decrement since the vector was reduced by 1
			continue;
		}


		//check reflection conditions
		for (int j = 0, k = mLevel->getPlayerHitboxes().size(); j < k; ++j)
		{
			if (checkCollision(hitbox.hitbox, mLevel->getPlayerHitboxes()[j].hitbox)//if the projectile collides with a player hitbox
				&& absValue(mProjectile[i].w) == kCannon_ProjectileSpeed) //if the projectile is not already at a deflected velocity
			{
				mProjectile[i].w *= -2; //make the projectile go in opposite direction at double speed

				if (mLevel->getPlayerHitboxes()[j].hitstun < 8)//if the deflecting hitbox has a low hitstun (weak attack), delete the projectile
				{
					mProjectile.erase(mProjectile.begin() + i);
					--i;//decrement since the vector was reduced by 1
					finish = false;
					break; //end this loop
				}
			}
		}

		if (finish
			&& absValue(mProjectile[i].w) == kCannon_ProjectileSpeed) //add the hurtbox if the projectile isn't deflected
			mLevel->addHitbox(hitbox, false, true, false);

	}
}

void Cannon::render(int xD, int yD)
{
	//do not render on inactive frames
	if (!mActive) return;

	//handle projectiles
	for (int i = 0, j = mProjectile.size(); i < j; ++i)
	{
		SDL_RendererFlip ftype = SDL_FLIP_NONE;
		if (mProjectile[i].w < 0)
			ftype = SDL_FLIP_HORIZONTAL;

		kCannon_Texture->renderTexture(
			mProjectile[i].x - xD,
			mProjectile[i].y - yD,
			&kCannon_AnimationClips[kCannonBall]);
	}

	//render the cannon
	SDL_RendererFlip ftype = SDL_FLIP_NONE;
	if (!mDirectionRight)
		ftype = SDL_FLIP_HORIZONTAL;
	if (kDefault_Texture != NULL)//will only render if the pointer is real
	{
		kCannon_Texture->renderTexture(
			mX - xD,
			mY - yD,
			&kCannon_AnimationClips[mAnimationFrame],
			0.0,
			NULL,
			ftype);
	}
}

void Cannon::checkHurt()
{
	//the cannon cannot get hurt
}

void Cannon::handleAnimation()
{
	if (mProjectileCooldown == kProjectileCooldown)
	{
		mAnimationFrame = kCannonFireStart;
		mRewindAnimation = false;
	}

	if (mAnimationFrame >= kCannonFireEnd) //goes backwards if it reached the end
		mRewindAnimation = true;

	//if (mAnimationFrame == kCannonFireStart)//goes forward if it reaches the beginning again

	if (mAnimationFrame >= kCannonFireStart && mAnimationFrame <= kCannonFireEnd)
	{
		mAnimationFrame = (mRewindAnimation) ?
			mAnimationFrame - 1 : mAnimationFrame + 1;
	}

}

void Cannon::setTexture(LTexture * texture)
{
	kCannon_Texture = texture;

	kCannon_AnimationClips =
		spriteClipper(texture->getWidth(),
			texture->getHeight(),
			{ 0, 0, kDefault_ClipW, kDefault_ClipH });
}

bool Cannon::checkLiving()
{
	return true;
}
