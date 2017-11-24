#ifndef DEMON_H
#define DEMON_H

#include <vector>
#include "Algorithms.h"
#include "LTexture.h"
#include "Enemy.h"
#include "Level.h"

class Demon : public Enemy
{
public:
	Demon();

	//you need a destructor for polymorphism, so this will do nothing
	~Demon(); 

	Demon(Uint8 x, Uint8 y, bool right = false, Level* level = NULL);


	void step(std::vector<SDL_Rect>& colliders);//virtual function that will be called for every object for every frame
	void step();
	void attack();//virtual attack function
	void render(int xD, int yD);//render relative to camera
	void checkHurt();//checks for damage taken and hitstun
	void handleAnimation();
	void setTexture(LTexture* texture);//sets texture for all default enemy
	bool checkLiving();//for the level to know when to delete an enemy
	void flyMove(std::vector<SDL_Rect>& colliders, SDL_Rect& offBox);

	enum DemonAnimationFrames
	{
		kFlyStart = 0,
		kFlyEnd = 12,
		kRunStart = 13,
		kRunEnd = 30,
		kPowerupStart= 31,
		kPowerupEnd = 39,
		kChargeStart = 40,
		kChargeEnd = 51,
		kFireballStart = 52,
		kFireballEnd= 59,
		kDeathStart = 60,
		kDeathEnd = 83
	};

	//modes of movement and attack
	enum DemonStyles : Uint8
	{
		kFly = 0,
		kRun,
		kPowerup,
		kCharge,
	};

	//health
	static const int kDemon_MaxHealth = 110;
	
	//texture
	static LTexture* kDemon_Texture;
	
	//animation clips
	static std::vector<SDL_Rect> kDemon_AnimationClips;
	
	//Collision box for the demon's fireball
	static Hitbox kDemon_ProjectileCollision;
	
	//the projectile speed of the demon's fireball
	static const Uint8 kDemon_ProjectileSpeed = 2;

	static const Uint8 kDemon_ProjectileCooldown = 100;

	static const Uint8 kDemon_PowerupTime = 120;

private:

	//Like the ninja's projectiles, it holds x,y,xv,yv
	std::vector<SDL_Rect> mProjectiles;

	Uint8 mFireballCooldown;

	DemonStyles mStyle;

	void mCheckStyle();

	bool mFlyWall;//demon will fly along the ground until 

	//when the player fights the boss the first time, losing against the boss results storyline progression
	//this is done by creating a door when the player dies
	void mPlayerDeathGimmick();

};

#endif