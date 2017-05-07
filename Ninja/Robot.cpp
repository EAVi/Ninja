#include "Robot.h"
#include "TextWriter.h"

extern TextWriter gWriter;

using namespace std;

LTexture* Robot::kRobot_Texture = NULL;
vector<SDL_Rect> Robot::kRobot_AnimationClips = vector<SDL_Rect>(0);
//SDL_Rect Robot::kRobot_OffsetCollisionBox = { 0,0,32,32 };
//SDL_Rect Robot::kRobot_OffsetHitbox = { 10,0,12,32 };

Robot::Robot()
{
	Enemy::Enemy();
	mMaxHealth = kRobot_MaxHealth;
	mHealth = mMaxHealth;
}

Robot::~Robot()
{
	//delete this;
	//do nothing
}

Robot::Robot(Uint8 x, Uint8 y, bool right, Level * level)
{
	//mX = x * 16;
	//mY = y * 16;
	Enemy::Enemy(x, y, right, level);
	//mMaxHealth = kRobot_MaxHealth;
	//mHealth = mMaxHealth;
}

void Robot::step(std::vector<SDL_Rect>& colliders)
{
	Enemy::step(colliders);
}

void Robot::step()
{
	Enemy::step();
}

void Robot::attack()
{
	Enemy::attack();
}

void Robot::render(int xD, int yD)
{
	Enemy::render(xD, yD);
	/*
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
	*/
}

void Robot::checkHurt()
{
	Enemy::checkHurt();
}

void Robot::handleAnimation()
{
	Enemy::handleAnimation();
}

void Robot::setTexture(LTexture * texture)
{
	Enemy::setTexture(texture);
	/*
	kRobot_Texture = texture;

	kRobot_AnimationClips =
		spriteClipper(texture->getWidth(),
			texture->getHeight(),
			{ 0, 0, kDefault_ClipW, kDefault_ClipH });
	*/
}

bool Robot::checkLiving()
{
	return true;
}
