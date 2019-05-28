#include "Samurai.h"

using namespace std;

float Samurai::kSamurai_Gravity = 0.4f;
vector<SDL_Rect> Samurai::kSamurai_AnimationClips = vector<SDL_Rect>();
LTexture* Samurai::kSamurai_Texture = NULL;
SDL_Rect Samurai::kSamurai_CollisionBox = { 10,0,12,32 };
Hitbox Samurai::kSamurai_KickHitbox = { { 0, 16, 24, 4 }, 20, 40 };
Hitbox Samurai::kSamurai_SliceHitbox = { { 0, 5, 32, 10}, 15, 50 };

Samurai::Samurai()
{
}

Samurai::Samurai(Uint8 x, Uint8 y, bool right, Level * level)
	: Enemy(x,y,right,level)
{
	mMaxHealth = kSamurai_MaxHealth;
	mHealth = mMaxHealth;
	mState = kRunning;
	mAnimationFrame = kAnimRunningStart;
	mStateCooldown = 0;
}

Samurai::~Samurai()
{
}

void Samurai::step(std::vector<SDL_Rect>& colliders)
{
	mPlayerDeathGimmick();
	if (mHitStun <= 0 && mHealth > 0)//not in hitstun and still has health
	{
		attack();
		checkHurt();
		statemove(colliders);
		//when a wall is touched
		if (mTouchIndex & (kTouchingLeft | kTouchingRight) && mXV != 0)
			mDirectionRight = !mDirectionRight;//if running, turn around
	}
	else
	{
		if (mHitStun > 0)
			--mHitStun;
		else if (mHealth > 0)
			attack();
		mXV = 0;
		mYV++;
		move(colliders, kDefault_OffsetCollisionBox);
	}

	if(mState == kDying && mAnimationFrame == kAnimDeathEnd) mState = kDead;//check for death

	if (mHitStun <= 0)
		handleAnimation();

}

void Samurai::step()
{
	if (mLevel == NULL) return;//would have issues otherwise
	step(mLevel->getRects());
}

void Samurai::attack()
{
	if (mLevel == NULL) return; //Enemies aren't always bound to a level

	//this is the hurtbox
	Hitbox hurtbox = { kDefault_OffsetCollisionBox , 0, 0 };
	hurtbox.hitbox.x += mX;
	hurtbox.hitbox.y += mY;
	mHurtbox = hurtbox.hitbox;

	//this is a passive hitbox
	Hitbox hitbox;
	hitbox.hitbox = kSamurai_CollisionBox;
	hitbox.hitbox.x += mX;
	hitbox.hitbox.y += mY;
	hitbox.damage = kSamurai_PassiveDamage;
	hitbox.hitstun = kSamurai_PassiveHitstun;

	mLevel->addHitbox(hurtbox, false, false, true);
	mLevel->addHitbox(hitbox, false, true, false);


	switch (mState)
	{
	case kKicking:
		//Sword handling
		if (mStateCooldown >= kSamurai_KickCooldown - kSamurai_KickLifespan)
		{
			if (mDirectionRight)
			{
				Hitbox temp = kSamurai_KickHitbox;
				temp.hitbox.x += mX + kSamurai_CollisionBox.x + kSamurai_CollisionBox.w / 2;// centered and to the right, for a right-facing ninja
				temp.hitbox.y += mY;
				mLevel->addHitbox(temp, false, true, false);
			}
			else
			{
				Hitbox temp = kSamurai_KickHitbox;
				temp.hitbox.x += mX + kSamurai_CollisionBox.x + kSamurai_CollisionBox.w / 2;//centered to the right for now, but to be changed
				temp.hitbox.x -= kSamurai_KickHitbox.hitbox.x * 2 + kSamurai_KickHitbox.hitbox.w;//moved to the left now
				temp.hitbox.y += mY;
				mLevel->addHitbox(temp, false, true, false);

			}
		}
		break;
	case kLungeSlashing:
		if (mStateCooldown >= kSamurai_LungeCooldown - kSamurai_SliceLifespan)
		{
			Hitbox temp = kSamurai_SliceHitbox;
			temp.hitbox.x += mX;
			temp.hitbox.y += mY;
			mLevel->addHitbox(temp, false, true, false);
		}
		break;
	}
}

void Samurai::render(int xD, int yD)
{
	SDL_RendererFlip ftype = SDL_FLIP_NONE;
	if (!mDirectionRight)
		ftype = SDL_FLIP_HORIZONTAL;
	if (kDefault_Texture != NULL)//will only render if the pointer is real
	{
		kSamurai_Texture->renderTexture(
			mX - xD,
			mY - yD,
			&kSamurai_AnimationClips[mAnimationFrame],
			0.0,
			NULL,
			ftype);
	}


	//render the crescent slice if applicable
	if ((mState == kLungeSlashing) && (mStateCooldown >= kSamurai_LungeCooldown - kSamurai_SliceLifespan))
	{
		int tx = xD;
		SDL_RendererFlip fliptype = (mDirectionRight) ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
		/*if (mFlipType == SDL_FLIP_NONE)
			tx -= kStandardCollisionBox.x + kStandardCollisionBox.w / 2;
		else
			tx += kStandardCollisionBox.x + kStandardCollisionBox.w;*/

		kSamurai_Texture->renderTexture(this->mX - xD, this->mY - yD,
			&kSamurai_AnimationClips[kAnimLungeSliceEffect], 0.0, NULL, fliptype);
	}

}

void Samurai::checkHurt()
{
	vector<Hitbox>& temp = mLevel->getPlayerHitboxes();
	for (int i = 0, j = temp.size(); i < j; ++i)
	{
		if (checkCollision(mHurtbox, temp[i].hitbox))
		{
			mHealth -= temp[i].damage;
			mHitStun = temp[i].hitstun;
			if (mState == kStanding)//if attacked while standing, kick
			{
				mState = kKicking;
				mStateCooldown = kSamurai_KickCooldown;
				mRewindAnimation = false;
			}
		}
	}
	//some checks to prevent under/overflow
	if (mHealth < 0) mState = kDying;
	if (mHealth > mMaxHealth) mHealth = mMaxHealth;
}

void Samurai::handleAnimation()
{
	if (mHitStun > 0) return;
	switch (mState)
	{
	case kRunning:
		if (mAnimationFrame >= kAnimRunningEnd || mAnimationFrame < kAnimRunningStart)
			//if not already walking, start walking
			mAnimationFrame = kAnimRunningStart;
		//else, currently in the middle of walking animation, keep it up
		else ++mAnimationFrame;
		break;

	case kLunging:
		mAnimationFrame = kAnimLunging; break;
	case kLungeSlashing:
		mAnimationFrame = kAnimLungeSlash; break;

	case kDying:
		if (mAnimationFrame > kAnimDeathEnd || mAnimationFrame < kAnimDeathStart)
			//if not already dying, start dying
			mAnimationFrame = kAnimDeathStart;
		//else, currently in the middle of dying animation, keep it up
		else ++mAnimationFrame;
		break;

	case kKicking:
		if (mAnimationFrame > kAnimKickEnd || mAnimationFrame < kAnimKickStart)
			 mAnimationFrame = kAnimKickStart;
		//else, currently in the middle of kick animation, keep it up
		else ++mAnimationFrame;
		break;

	case kStanding:
		mAnimationFrame = kAnimStanding;
		break;
	}
}

void Samurai::setTexture(LTexture * texture)
{
	kSamurai_Texture = texture;

	kSamurai_AnimationClips =
		spriteClipper(texture->getWidth(),
			texture->getHeight(),
			{ 0, 0, kDefault_ClipW, kDefault_ClipH });
}

bool Samurai::checkLiving()
{
	if(mState == kDead)
	{
			mLevel->createDoor({ {mX,mY, 16,32},255,255,255,1 });
			return false;
	}
	return true;
}

void Samurai::statemove(std::vector<SDL_Rect>& colliders)
{
	if (mLevel == NULL) return;
	SDL_Point playerpos = mLevel->getPlayerPosition();
	int xaverage = mX + kSamurai_CollisionBox.x + kSamurai_CollisionBox.w/2;//should total up to mX + 16, but I want to be safe
	if (mHealth <= 0 && mState != kDead) mState = kDying;
	switch (mState)
	{
		case kRunning:
			mDirectionRight = (xaverage < playerpos.x);//face the ninja
			mXV = (mDirectionRight) ? kSamurai_Runspeed : -kSamurai_Runspeed; //chase the Ninja
			if (absValue(xaverage - playerpos.x) < kSamurai_LungeDistance)
			{
				mState = kLunging;//start the lunge if the ninja is close
				mYV = -kSamurai_JumpVelocity;//jump to enter the lunge
			}
			break;
		case kLunging:
			if ((mTouchIndex & kTouchingTop)) //slash when you hit the ground
			{
				mState = kLungeSlashing;
				mStateCooldown = kSamurai_LungeCooldown;//slash and stay vulnerable for a bit
			}
			mXV = mDirectionRight ? kSamurai_Lungespeed : -kSamurai_Lungespeed; //Samurai chases the Ninja
			break;
		case kLungeSlashing:
			mYV = 0;
			mXV = 0;
			if (mStateCooldown <= 0)
			{
				mState = kStanding;
				mStateCooldown = kSamurai_LungeCooldown;//stand still for a while
			}
			
			break;
		case kStanding:
			mYV = 0;
			mXV = 0;
			if (mStateCooldown <= 0)
			{
				mState = kRunning;
			}
			
			break;
		//kick state is entered if hurt while in standing phase, the transition logic is in checkhurt
		case kKicking:
			mYV = 0;
			mXV = 0;
			mDirectionRight = (xaverage < playerpos.x);//face the ninja
			
			if (mAnimationFrame == kAnimKickEnd)
			{ 
				mState = kRunning;
				mStateCooldown = 0;//stand still for a while
			}
			mAttackFrame = kSamurai_KickCooldown;
			break;
		
	}		

	if(mStateCooldown > 0) mStateCooldown--;

	move(colliders, kSamurai_CollisionBox);

	//gravity and ground touching logic
	if (mTouchIndex & kTouchingTop)
		mYV = 0;
	else if (mYV < kDefault_TerminalVelocity)
		mYV++;
}

void Samurai::mPlayerDeathGimmick()
{
	if (mLevel->checkPlayerDead()
		&& (mLevel->getLevelID().ZoneNo != 8))
	{
		//notice that the door lead to level 255, 
		//when a level above the zone mlevel.size is given, it counts as a zone completed (victory)
		mLevel->createDoor({ {0,0,1023,1023}, 255, 255, 255, 255 });
	}
}
