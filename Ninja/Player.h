#ifndef PLAYER_H
#define PLAYER_H

#if defined _WIN32 || _WIN64
#include <SDL.h>
#include <SDL_image.h>
#elif defined __APPLE__ && __MACH__ || defined Macintosh || defined macintosh || defined __linux__
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#endif

#include <vector>
#include <iostream>
#include "LTexture.h"
#include "Algorithms.h"
#include "Block.h"
#include "Level.h"
#include "Hitbox.h"

class Player
{
public:
	Player();
	static const int kClipWidth = 32;
	static const int kClipHeight = 32;

	//Set the texture to be rendered
	void setTexture(LTexture* texture);

	//Reads keyboard input
	void handleEvent(SDL_Event& e);

	//handles everything that should be done within a frame
	void step();

	//moves the object accordingly
	void move(std::vector<SDL_Rect>& colliders);

	//overload using camera's x and ys
	void render(int x = 0, int y = 0);

	//checks for conditions for animation and animates
	void handleAnimation();

	//allows the player to reference the current level
	void setLevel(Level* level);

	//set x,y position
	void setposition(int x, int y);

	int getX();

	int getY();
private:
	LTexture* mTexture;
	SDL_Point mRestorePoint;
	std::vector<SDL_Rect> mAnimationFrameRectangles;
	SDL_Rect mCollisionBox;
	SDL_RendererFlip mFlipType;
	Level* mLevel;
	int mX;
	int mY;
	int mXVelocity;
	int mYVelocity;//reminder that negatives go upwards on screen
	int mHealth;
	int mMaxHealth;
	int lives;
	Uint8 mTouchIndex;
	Uint16 mAnimationFrame;
	Uint8 mAttackCoolDown;
	Hitbox mSwordHitbox;
	bool mWallClinging;
	bool mJump;
	bool mAirborne;

	void mLeftPress();//the function for when the left key is pressed
	void mJumpPress();//the function for when the jump key is pressed
	void mRightPress();//the function for when the right key is pressed
	void mAttackPress();//the function for when the attack key is pressed
	void mLeftRelease();//the function for when the left key is released
	void mRightRelease();//the function for when the right key is released

	//buncha things used to check collision
	void mCheckClinging();//checks if clinging to a wall and changes mWallClinging
	void mCollisionSquisher();//modifies collision boxes if needed
	void mBoundPlayer();//Bounds player velocity and character within the level
	void mAddHurtbox();
	void mAttack();//Attacks if currently attacking

	static const int kTerminalVelocity = 7;//player terminal velocity
	static const int kGravity = 1;//player gravity
	static const int kJumpVelocity = 15;//velocity gained during jump
	static const int kMovementSpeed = 5;
	static const Uint8 kSwordLag = 24;
	static const Uint8 kSwordFrames = 3;
	static SDL_Rect kStandardCollisionBox;
	static SDL_Rect kJumpingCollisionBox;
};

enum NinjaAnimationFrames
{
	kDeathStart = 0,
	kDeathEnd = 15,
	kLandingStart = 16,
	kLandingEnd = 19,
	kJumpStart = 20,
	kJumpEnd = 31,
	kWalkStart = 32,
	kWalkEnd = 44,
	kStanding = 45,
	kStandSlash = 46,
	kEffectSlice = 47,//the slicing crescent thing
	kWalkSlashStart = 48,
	kWalkSlashEnd = 60,
	kWallCling = 61
};

#endif