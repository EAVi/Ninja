#include "Robot.h"
#include "TextWriter.h"

extern TextWriter gWriter;

using namespace std;

LTexture* Robot::kRobot_Texture = NULL;
vector<SDL_Rect> Robot::kRobot_AnimationClips = vector<SDL_Rect>(0);
//SDL_Rect Robot::kRobot_OffsetCollisionBox = { 0,0,32,32 };
//SDL_Rect Robot::kRobot_OffsetHitbox = { 10,0,12,32 };

Robot::Robot()
	:Enemy()
{
	mMaxHealth = kRobot_MaxHealth;
	mHealth = mMaxHealth;
}

Robot::~Robot()
{
	//do nothing
}

Robot::Robot(Uint8 x, Uint8 y, bool right, Level * level)
	: Enemy(x, y, right, level)
{
	mMaxHealth = kRobot_MaxHealth;
	mHealth = mMaxHealth;
}

void Robot::step(vector<SDL_Rect>& colliders)
{
	if (mHitStun <= 0 && mHealth > 0)//cant move if you're stunned
	{
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

void Robot::step()
{
	if (mLevel == NULL) return;//would have issues otherwise
	step(mLevel->getRects());
}

void Robot::attack()
{
	Enemy::attack();
}

void Robot::render(int xD, int yD)
{
	SDL_RendererFlip ftype = SDL_FLIP_NONE;
	if (!mDirectionRight)
		ftype = SDL_FLIP_HORIZONTAL;
	if (kDefault_Texture != NULL)//will only render if the pointer is real
	{
		kRobot_Texture->renderTexture(
			mX - xD,
			mY - yD,
			&kRobot_AnimationClips[mAnimationFrame],
			0.0,
			NULL,
			ftype);
	}
}

void Robot::checkHurt()
{
	Enemy::checkHurt();
}

void Robot::handleAnimation()
{
	if (mHealth <= 0)//dying animation
	{
		if (mAnimationFrame > kDeathEnd || mAnimationFrame < kDeathStart)
			//if not already dying, start dying
			mAnimationFrame = kDeathStart;
		//else, currently in the middle of dying animation, keep it up
		else ++mAnimationFrame;
	}

	else//running animation
	{
		if (mAnimationFrame >= kRunEnd || mAnimationFrame < kRunStart)
			//if not already walking, start walking
			mAnimationFrame = kRunStart;
		//else, currently in the middle of walking animation, keep it up
		else ++mAnimationFrame;
	}
}

void Robot::setTexture(LTexture * texture)
{
	kRobot_Texture = texture;

	kRobot_AnimationClips =
		spriteClipper(texture->getWidth(),
			texture->getHeight(),
			{ 0, 0, kDefault_ClipW, kDefault_ClipH });
}

bool Robot::checkLiving()
{
	return (mAnimationFrame != RobotAnimationFrames::kDeathEnd);
}
