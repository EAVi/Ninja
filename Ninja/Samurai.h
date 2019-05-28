#ifndef SAMURAI_H
#define SAMURAI_H

#include <vector>
#include "Algorithms.h"
#include "LTexture.h"
#include "Enemy.h"
#include "Level.h"

class Samurai : public Enemy
{
public:
	Samurai();//unnecessary default constructor
	Samurai(Uint8 x, Uint8 y, bool right = false, Level* level = NULL);
	~Samurai();//destructor

	void step(std::vector<SDL_Rect>& colliders);//virtual function that will be called for every object for every frame
	void step();
	void attack();//virtual attack function
	void render(int xD, int yD);//render relative to camera
	void checkHurt();//checks for damage taken and hitstun
	void handleAnimation();
	void setTexture(LTexture* texture);//set the samurai texture
	bool checkLiving();//for the level to know when to delete an enemy
	void statemove(std::vector<SDL_Rect>& colliders);//move samurai based on state
	
	//constants et al. 
	enum SamuraiState : Uint8
	{
		kStanding,
		kRunning,
		kLunging,
		kLungeSlashing,
		kKicking,
		//sword throwing and dying are a work in progress
		kDying,
		kDead,
	};

	enum SamuraiAnimationFrames {
		kAnimRunningStart = 0,
		kAnimRunningEnd = 11,
		kAnimStanding = 12,
		kAnimLunging = 13,
		kAnimLungeSlash = 14,
		kAnimLungeSliceEffect = 15,
		kAnimKickStart = 16,
		kAnimKickEnd = 26,
		kAnimSwordThrowing,
		kAnimSwordThrowEffectStart,
		kAnimSwordThrowEffectEnd,
		kAnimDeathStart,
		kAnimDeathEnd,
		//sword throwing, and dying are a work in progress
	};

	static const int kSamurai_Runspeed = 2;
	static const int kSamurai_Lungespeed = 4;
	static const int kSamurai_MaxHealth = 150;
	static const int kSamurai_JumpVelocity = 7;
	static float kSamurai_Gravity;
	static LTexture* kSamurai_Texture;
	static std::vector<SDL_Rect> kSamurai_AnimationClips;
	static SDL_Rect kSamurai_CollisionBox;
	static const int kSamurai_PassiveDamage = 10;
	static const int kSamurai_PassiveHitstun = 20;
	static Hitbox kSamurai_KickHitbox;
	static Hitbox kSamurai_SliceHitbox;
	static const int kSamurai_KickCooldown = 4;
	static const int kSamurai_KickLifespan = 3;
	static const int kSamurai_LungeCooldown = 45;
	static const int kSamurai_SliceLifespan = 4;
	static const int kSamurai_LungeDistance = 75;//distance before the samurai lunges while running
	//end of all the constants

private:
	SamuraiState mState;//current state of the samurai
	int mStateCooldown;

	void mPlayerDeathGimmick();

};

#endif
