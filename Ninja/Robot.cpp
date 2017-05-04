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
	Enemy();
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
	Enemy(x, y, right, level);
	mMaxHealth = kRobot_MaxHealth;
	mHealth = mMaxHealth;
	mX = x * 16;
	mY = y * 16;
}

void Robot::step(std::vector<SDL_Rect>& colliders)
{
}

void Robot::step()
{

}

void Robot::attack()
{

}

void Robot::render(int xD, int yD)
{
	if (kRobot_AnimationClips.empty())
	{
		gWriter << "Robot subscript of range!\n";
		return;
	}

	mAnimationFrame = 0;
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

}

void Robot::handleAnimation()
{

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
	return true;
}
