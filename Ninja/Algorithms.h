#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#ifdef _WIN32 || _WIN64
#include <SDL.h>
#include <SDL_image.h>
#elif defined __APPLE__ && __MACH__ || defined Macintosh || defined macintosh || defined __linux__
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#endif

#include <vector>
#include <string>
#include <iostream>
#include <bitset>


//checks a is greater than b and less than c
bool between(int a, int b, int c);

//Checks if collision happens
bool checkCollision(SDL_Rect& a, SDL_Rect& b);

//Checks collision with an array
bool checkCollision(SDL_Rect& player, std::vector<SDL_Rect>& boxes);

//flags returned by checkTouching
enum checkTouchingFlags//example kTouchingTop means a is on top of b
{ 
	kTouchingNone = 0,
	kTouchingTop = 1,
	kTouchingBottom = 2, 
	kTouchingLeft = 4,
	kTouchingRight = 8
};

//Returns a single touching for one block
checkTouchingFlags checkTouching(SDL_Rect& a, SDL_Rect& b);

//Returns an index of touching, which shows which moves are possible
Uint8 getTouchingIndex(SDL_Rect& a, std::vector<SDL_Rect>& b);

//Returns all collided rectangles
std::vector<SDL_Rect> getCollisionIndex(SDL_Rect& player, std::vector<SDL_Rect>& boxes);

//given many collision boxes, finds the optimal movement
SDL_Point optimizedMove(SDL_Rect& player, std::vector<SDL_Rect>& boxes, int& xV, int& yV, Uint8& touchIndex);

//creates a vector of the clips for a spritesheet
std::vector<SDL_Rect> spriteClipper(int width, int height, SDL_Rect clipsize);


#endif