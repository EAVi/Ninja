#ifndef CANNON_H
#define CANNON_H

#if defined _WIN32 || _WIN64
#include <SDL.h>
#elif defined __APPLE__ && __MACH__ || defined Macintosh || defined macintosh || defined __linux__
#include <SDL2/SDL.h>
#endif

#include <vector>
#include "Algorithms.h"
#include "LTexture.h"
#include "Enemy.h"
#include "Level.h"

class Cannon public Enemy
{
	Cannon();
	~Cannon();
	Cannon(Uint8 x, Uint8 y, bool right = false, Level* level = NULL);

	/*
	Cannon will not move and will fire balls at the Ninja
	*/

	//static const int kDefault_WalkSpeed = 1;//walk speed for the base class enemy
	//static const int kDefault_Gravity = 1;//gravity for base class enemy
	//static const int kDefault_TerminalVelocity = 6;//TV for base class enemy
	static LTexture* kCannon_Texture;//texture for base class enemy
	static const int kCannon_MaxHealth = 20;//max health for base class enemy
	static std::vector<SDL_Rect> kCannon_AnimationClips;//vector containing the sprite animation rectangles
													   //static SDL_Rect kDefault_OffsetCollisionBox;//collisionBox to be added with mX, mY in order to check optimal position
													   //static SDL_Rect kDefault_OffsetHitbox;
													   //static const int kDefault_ClipW = 32, kDefault_ClipH = 32;
													   //static const Uint8 kDefault_HitStunFrames = 5;
	enum CannonAnimationFrames
	{
		kCannonBody = 0,
		kCannonBall = 1
	};

	void step(std::vector<SDL_Rect>& colliders);//virtual function that will be called for every object for every frame
	void step();
	void attack();//virtual attack function
	void render(int xD, int yD);//render relative to camera
	void checkHurt();//checks for damage taken and hitstun
	void handleAnimation();
	void setTexture(LTexture* texture);//sets texture for all default enemy
	bool checkLiving();//for the level to know when to delete an enemy
};

#endif