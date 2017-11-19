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
#include "Level.h"
#include "Hitbox.h"
#include "LAudio.h"

class Player
{
public:
	Player();
	static const int kClipWidth = 32;
	static const int kClipHeight = 32;

	//Set the texture to be rendered
	void setTexture(LTexture* texture);

	//Reads keyboard input
	//the bool ensures that only movement handling is done, that attacks and jumps will be ignored
	void handleEvent(SDL_Event& e, bool movementOnly = false);

	//handles everything that should be done within a frame
	void step();

	//since hitboxes are deleted every frame, Ninja needs to check for hitbox collision at the end of the frame
	void endstep();

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

	//return x,y position
	int& getX();
	int& getY();

	//returns player health
	int& getHealth();
	int& getMaxHealth();
	int& getLives();

	SDL_Rect& getCollisionBox();

	bool checkDead();

	//set the number of lives to the const value
	void resetLives();

	//set the players position to spawn point
	void respawn();

	bool checkUpPressed();
private:
	LTexture* mTexture;
	std::vector<SDL_Rect> mAnimationFrameRectangles;
	SDL_Rect mCollisionBox;
	SDL_RendererFlip mFlipType;
	Level* mLevel;
	int mX;
	int mY;
	int mWallJumpNumber;//number of times the Ninja has jumped without touching the ground, diminishing walljump power
	int mXVelocity;
	int mYVelocity;//reminder that negatives go upwards on screen
	float mYVelocityF;//precise gravity, is truncated to mYVelocity so collision math can be done
	int mHealth;
	int mMaxHealth;
	int mLives;
	Uint8 mTouchIndex;
	Uint16 mAnimationFrame;
	Uint8 mAttackCoolDown;
	Sint8 mStarCooldown; //attack cooldown for the shuriken, signed because negative numbers will be for the animation frame
	Uint8 mInvincibilityFrames;
	Hitbox mSwordHitbox;
	Hitbox mStarHitbox;
	std::vector<SDL_Rect> mStarPositions; //x,y for the position w,h for the velocity
	bool mWallClinging;
	bool mJump;
	bool mAirborne;
	bool mUpPressed;
	Uint8 mHitStun;

	bool mLeftKeyDown;//booleans to allow left and right press controls to be handled better
	bool mRightKeyDown;
	void mHandleDirection();
	void mLeftPress();//the function for when the left key is pressed
	void mJumpPress();//the function for when the jump key is pressed
	void mRightPress();//the function for when the right key is pressed
	void mAttackPress();//the function for when the attack key is pressed
	void mProjectilePress();//the function for when the ninja star key is pressed
	void mLeftRelease();//the function for when the left key is released
	void mRightRelease();//the function for when the right key is released
	void mAxisLeft();//game controller alternative function to mLeft Press and Release
	void mAxisRight();//game controller alternative function to mRight Press and Release
	void mAxisNone();//when controller axis is in deadzone
	void mUpPress();//the function for when the up key is pressed
	void mUpRelease();//the function for when the up key is pressed

	//buncha things used to check collision
	void mCheckClinging();//checks if clinging to a wall and changes mWallClinging
	void mCollisionSquisher();//modifies collision boxes if needed
	void mBoundPlayer();//Bounds player velocity and character within the level
	void mAddHurtbox();//Puts a player hurtbox into the mLevel's vector
	void mAttack();//Attacks if currently attacking
				   //NOTE that this is not the attack event handler, that would be mAttackPress()
	void mCheckHurt();//checks for collision with hurtboxes and hitboxes

	static float kGravityF;//SOON TO REPLACE INTEGER GRAVITY
	static const int kTerminalVelocity = 7;//player terminal velocity
	static const int kJumpVelocity = 9;//velocity gained during jump
	static const int kMovementSpeed = 4;
	static const Uint8 kSwordLag = 24;//amount of frames after using the sword before the sword may be used again
	static const Uint8 kStarLag = 12;//"" star may be used again
	static const Uint8 mStarSpeed = 6;//the speed that a ninja star will travel
	static const Uint8 kSwordFrames = 6;
	static const Uint8 kHitStunFrames = 5;
	static const Uint8 kInvincibilityFrames = 20;
	static const Uint8 kBlinkingFrequency = 3;
	static SDL_Rect kStandardCollisionBox;
	static SDL_Rect kJumpingCollisionBox;
	static const int kMaxHealth = 50;
	static const int kStartingLives = 4;
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
	kWallCling = 61,
	kStandStun = 62,
	kLookUp = 62,
	kStandStunSword = 63,
	kLogDeathStart = 64,
	kLogDeathEnd = 79,
	kAirStun = 16,
	kStarEffectStart = 80,//A SHURIKEN of DOOM
	kStarEffectEnd = 83,

};

#endif
