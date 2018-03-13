#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#if defined _WIN32 || _WIN64
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

//returns the absolute value of a given value
int absValue(int a);

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

/*
given a single collision box, check if on edge 
uses three rectangles to check if there's an edge ahead
*/
bool checkEdge(SDL_Rect& lowerL, SDL_Rect& lowerR, SDL_Rect& boxes);

/*
checks edge stuff for certain blocks
Why this functions is needed:
some enemies just need to check whether there's a edge or wall in front of them or not.
Optimized move may cause a bit of overhead, this is an attempt to improve time complexity on small-fry enemies
*/
bool checkEdges(SDL_Rect& player, std::vector<SDL_Rect>& boxes);

//creates a vector of the clips for a spritesheet
std::vector<SDL_Rect> spriteClipper(int width, int height, SDL_Rect clipsize);

/*
checks if a and b are glue-able and glues them if they are
for example: a = {0,0,255,255} b = {256,0,20,255}
the result is {0,0,276,255}
*/
SDL_Rect glueRectangles(SDL_Rect a, SDL_Rect b);

/*
gets all rectangles and glues gluable ones together
this is to reduce the number of collision boxes in the level
meaning more efficient collision detection
*/
std::vector<SDL_Rect> rectangleMerge(std::vector<SDL_Rect> rects);

//before checking for collision indexes, you need to stretch the collision boxes
//to get a valid index. Using the velocities will allow a better index
void stretchBox(SDL_Rect & a, int & xV, int & yV);

//undoes the stretchBox operation, assuming the same parameters are given
void stretchBoxUndo(SDL_Rect & a, int & xV, int & yV);

//takes a screenshot of the current frame of the renderer,
//saves to path and filename given by string fname
bool screenShot(SDL_Window * w, SDL_Renderer * r, std::string fname);

#endif
