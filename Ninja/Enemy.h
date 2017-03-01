#ifndef ENEMY_H
#define ENEMY_H

#if defined _WIN32 || _WIN64
#include <SDL.h>
#elif defined __APPLE__ && __MACH__ || defined Macintosh || defined macintosh || defined __linux__
#include <SDL2/SDL.h>
#endif

#include <vector>
#include "Algorithms.h"
#include "LTexture.h"
class Level;
class Enemy
{
public:
	Enemy();//default constructor, probably won't be used
	Enemy(Uint8 x, Uint8 y, bool right = false, Level* level = NULL);
	static const int kDefault_WalkSpeed = 1;//walk speed for the base class enemy
	static const int kDefault_Gravity = 1;//gravity for base class enemy
	static const int kDefault_TerminalVelocity = 6;//TV for base class enemy
	static LTexture* kDefault_Texture;//texture for base class enemy
	static const int kDefault_MaxHealth = 1;//max health for base class enemy
	static std::vector<SDL_Rect> kDefault_AnimationClips;//vector containing the sprite animation rectangles
	static SDL_Rect kDefault_OffsetCollisionBox;//collisionBox to be added with mX, mY in order to check optimal position
	static SDL_Rect kDefault_OffsetHitbox;
	static const int kDefault_ClipW = 32, kDefault_ClipH = 32;
	static const Uint8 kDefault_HitStunFrames = 5;
	enum DefaultAnimationFrames
	{
		kRunStart = 0,
		kRunEnd = 7,
		kDeathStart = 8,
		kDeathEnd = 23
	};

	virtual void step(std::vector<SDL_Rect>& colliders);//virtual function that will be called for every object for every frame
	virtual void step();
	void move(std::vector<SDL_Rect>& colliders, SDL_Rect& offBox);//movement function, the velocities should be changed beforehand in the step() function
	void setLevel(Level* level);//sets the level for the enemy
	virtual void attack();//virtual attack function
	virtual void render(int xD, int yD);//render relative to camera
	virtual void checkHurt();//checks for damage taken and hitstun
	virtual void handleAnimation();
	virtual void setTexture(LTexture* texture);//sets texture for all default enemy
	virtual bool checkLiving();//for the level to know when to delete an enemy
private:
	bool mBoss;
	int mHealth;
	int mMaxHealth;
	Uint16 mAnimationFrame;
	bool mOddFrame;
	bool mRewindAnimation;
	int mX, mY, mXV, mYV;
	Uint8 mAttackFrame;
	SDL_Rect mCollisionBox;//collision box relative to position
	SDL_Rect mHurtbox;//collision box
	SDL_Rect mDangerZone;//passive hitbox if any
	Uint8 mTouchIndex;//used to make collision checking faster
	bool mDirectionRight;//begins false
	Level* mLevel; //the level that will be referenced by the enemy
	Uint8 mHitStun;
};



#endif // !ENEMY_H

