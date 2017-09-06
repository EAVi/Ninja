#include "Algorithms.h"

using namespace std;

const int tilesize = 16;

int abs(int a)
{
	if (a < 0)//if negative
		return -a;//return positive
	//else
	return a;//return positive
}

bool between(int a, int b, int c)
{
	if (b < c)
		return (a > b && a < c);
	else return (a < b && a > c);
}

bool checkCollision(SDL_Rect& a, SDL_Rect& b)
{
	int aTop = a.y;
	int aLeft = a.x;
	int aBottom = a.y + a.h;
	int aRight = a.x + a.w;

	int bTop = b.y;
	int bLeft = b.x;
	int bBottom = b.y + b.h;
	int bRight = b.x + b.w;

	//separating axis test
	//Note, using the equals sign allows them to touch without overlap
	if (aRight <= bLeft)
		return false;
	if (aLeft >= bRight)
		return false;
	if (aTop >= bBottom)
		return false;
	if (aBottom <= bTop)
		return false;
	
	return true;
}

bool checkCollision(SDL_Rect& player, vector<SDL_Rect>& boxes)
{
	for (Uint16 i = 0, j = boxes.size(); i < j; ++i)
	{
		if (checkCollision(player, boxes[i]))
			return true;
	}
	return false;
}

checkTouchingFlags checkTouching(SDL_Rect& a, SDL_Rect & b)
{
	int aTop = a.y;
	int aLeft = a.x;
	int aBottom = a.y + a.h;
	int aRight = a.x + a.w;

	int bTop = b.y;
	int bLeft = b.x;
	int bBottom = b.y + b.h;
	int bRight = b.x + b.w;
	
	//if collision, then
	if (checkCollision(a, b))
		return kTouchingNone;

	//if the x values are in the right boundaries
	if (between(aLeft, bLeft, bRight) || 
		between(aRight, bLeft, bRight) ||
		between(bLeft, aLeft, aRight) || 
		between(bRight, aLeft, aRight) ||
		aRight == bRight || aLeft == bLeft)
	{
		if (aTop == bBottom)
			return kTouchingBottom;
		if (aBottom == bTop)
			return kTouchingTop;
	}

	//if the y values are in the right boundaries
	if (between(aTop, bTop, bBottom) || 
		between(aBottom, bTop, bBottom) ||
		between(bTop, aTop, aBottom) ||
		between(bBottom, aTop, aBottom) ||
		aTop == bTop || aBottom == bBottom)
	{
		if (aRight == bLeft)
			return kTouchingLeft;
		if (aLeft == bRight)
			return kTouchingRight;
	}

	return kTouchingNone;
}

Uint8 getTouchingIndex(SDL_Rect& a, vector<SDL_Rect>& b)
{
	Uint8 tempIndex = 0;
	Uint8 tempFlag = 0;
	for (Uint16 i = 0, j = b.size(); i < j; ++i)
	{
		tempFlag = checkTouching(a, b[i]);
		tempIndex = tempIndex | tempFlag; //adds touching flag to the index if it isn't already there
	}
	//cout << bitset<8>(tempIndex) << endl; //debugging
	return tempIndex;
}

vector<SDL_Rect> getCollisionIndex(SDL_Rect& player, vector<SDL_Rect>& boxes)
{
	vector<SDL_Rect> tempIndex;

	for (Uint16 i = 0, j = boxes.size(); i < j; ++i)
	{
		if (checkCollision(player, boxes[i]))
			tempIndex.push_back(boxes[i]);
	}
	return tempIndex;
}


SDL_Point optimizedMove(SDL_Rect& player, vector<SDL_Rect>& boxes, int& xV, int& yV, Uint8& touchIndex)
{
	SDL_Rect tempRect = player;
	tempRect.x += xV;
	tempRect.y += yV;

	//to get a good index, stretch the collision box
	stretchBox(tempRect, xV, yV);

	//checks collision for intended movement
	vector<SDL_Rect> colliders = getCollisionIndex(tempRect, boxes);

	//undo the stretch for actual collision checking
	stretchBoxUndo(tempRect, xV, yV);


	//gets a touching index for the colliders, it's just a bunch of flags
	touchIndex = getTouchingIndex(player, colliders);

	//if there's no collision, then it ends here
	if (colliders.size() == 0)
		return{ tempRect.x, tempRect.y };

	//resets the rectangle's Y position, we're gonna do X testing
	tempRect.y = player.y;

	//X testing, does not need to test if xV == 0
	if (xV < 0)
		if ((touchIndex & kTouchingRight)) tempRect.x = player.x;
		else if(checkCollision(tempRect, colliders))
		{
			tempRect.x = player.x - (player.x % tilesize);
			touchIndex |= kTouchingRight;
		}

	if (xV > 0)
		if ((touchIndex & kTouchingLeft)) tempRect.x = player.x;
		else if (checkCollision(tempRect, colliders))
		{
			tempRect.x = tempRect.x - ((tempRect.x + tempRect.w) % tilesize);
			touchIndex |= kTouchingLeft;
		}

	tempRect.y = player.y + yV;
	//Y testing, same process, but touching collisions will reset velocity
	if (yV < 0)
		if (touchIndex & kTouchingBottom)
		{
			tempRect.y = player.y;
			yV = 0;
		}
		else if (checkCollision(tempRect, colliders))
		{
			tempRect.y = player.y - player.y % tilesize;
			touchIndex |= kTouchingBottom;
		}

	if (yV > 0)
		if (touchIndex & kTouchingTop)
		{
			tempRect.y = player.y;
			yV = 0;
		}
		else if (checkCollision(tempRect, colliders))
		{
			tempRect.y = tempRect.y - ((tempRect.y + tempRect.h) % tilesize);
			touchIndex |= kTouchingTop;
		}

	//return the optimized point
	return{ tempRect.x, tempRect.y };
}

bool checkEdge(SDL_Rect & lowerL, SDL_Rect & lowerR, SDL_Rect & boxes)
{
	return (!checkCollision(lowerL, boxes) || !checkCollision(lowerR, boxes));
}


bool checkEdges(SDL_Rect & player, std::vector<SDL_Rect>& boxes)
{
	//to get a good index, expand all edges by 1 
	//before getting the collision index, then unexpand
	player.x -= 1; player.y -= 1;
	player.w += 2; player.h += 2;

	//checks collision indexes
	vector<SDL_Rect> colliders = getCollisionIndex(player, boxes);

	//unexpand
	player.x += 1; player.y += 1;
	player.w -= 2; player.h -= 2;

	SDL_Rect left, right, up;
	up = player;
	up.x -= up.w;
	up.w = up.w * 3;
	left = player;
	left.x--;
	left.y += left.h + 1;
	left.w  = left.h = 1;
	right = left;
	right.x += player.w + 1;

	

	if (checkCollision(up, colliders)) return true;

	bool lcheck = true, rcheck = true;

	if (checkCollision(left, colliders))
		lcheck = false;
	if (checkCollision(right, colliders))
		rcheck = false;

	return (lcheck || rcheck);
}

vector<SDL_Rect> spriteClipper(int width, int height, SDL_Rect clipsize)
{
	clipsize.x = 0;
	clipsize.y = 0;
	vector<SDL_Rect> tempClipper;

	for (int y = 0; y < height; y += clipsize.h)
	{
		clipsize.y = y;
		for (int x = 0; x < width; x += clipsize.w)
		{
			clipsize.x = x;
			tempClipper.push_back(clipsize);
			
		}
		clipsize.y += 0;
	}

	return tempClipper;
}

SDL_Rect glueRectangles(SDL_Rect a, SDL_Rect b)
{
	//if none of the conditions are met, this will be the error condition
	SDL_Rect temp = {-1,-1,-1,-1};

	int aTop = a.y;
	int aLeft = a.x;
	int aBottom = a.y + a.h;
	int aRight = a.x + a.w;

	int bTop = b.y;
	int bLeft = b.x;
	int bBottom = b.y + b.h;
	int bRight = b.x + b.w;

	//check if a is inside b
	if (between(aLeft, bLeft - 1, bRight + 1)
		&& between(aRight, bLeft - 1, bRight + 1)
		&& between(aTop, bTop - 1, bBottom + 1)
		&& between(aBottom, bTop - 1, bBottom + 1))
		return b;

	//check if b is inside a
	if (between(bLeft, aLeft - 1, aRight + 1)
		&& between(bRight, aLeft - 1,  aRight + 1)
		&& between(bTop, aTop - 1, aBottom + 1)
		&& between(bBottom, aTop - 1, aBottom + 1))
		return a;

	//check if aligned on x coordinate
	if (aLeft == bLeft && aRight == bRight)
	{
		//if it's aligned on the x coordinate, then you have to glue on the y-axis, only if they're touching or colliding on the y axis

		//find the highest top (smallest y), the rectangles should be connected for this to work
		if (between(aTop, bTop - 1, bBottom + 1))//if (bTop <= aTop <= bBottom)
		{
			temp.x = a.x;//a.x == b.x
			temp.w = a.w;//a.w == b.w
			temp.y = bTop;
		}

		if (between(bTop, aTop - 1, aBottom + 1))//if (aTop <= bTop <= aBottom)
		{
			temp.x = a.x;
			temp.w = a.w;
			temp.y = aTop;
		}

		//to find the height of the resultant rectangle, find the largest bottom
		//much like in real life, bigger bottom == best candidate (heheheheh)
		if (aBottom >= bBottom && temp.x != -1)//a has the deeper bottom, take that height
			temp.h = aBottom - temp.y;
		else if (bBottom >= aBottom && temp.x != -1)//b has the deeper bottom, take that height
			temp.h = bBottom - temp.y;
	}

	//do the same stuff but switch the axis
	//check if aligned on y coordinate
	if (aTop == bTop && aBottom == bBottom)
	{
		//if it's aligned on the y coordinate, then you have to glue on the x-axis, only if they're touching or colliding on the x axis

		//find the leftmost coordinate (smallest x)
		if (between(aLeft, bLeft - 1, bRight + 1))//if (bLeft <= aLeft <= bRight)
		{
			temp.y = a.y;//a.y == b.y
			temp.h = a.h;//a.h == b.h
			temp.x = bLeft;
		}

		if (between(bLeft, aLeft - 1, aRight + 1))//if (aLeft <= aTop <= bRight)
		{
			temp.y = a.y;
			temp.h = a.h;
			temp.x = aLeft;
		}

		//then find the rightmost coordinate
		//the butt joke doesn't work in this axis :(
		if (aRight >= bRight && temp.y != -1)//a has the furthest right, take that right
			temp.w = aRight - temp.x;
		else if (bRight >= aRight && temp.y != -1)//b has the furthest right, take that right
			temp.w = bRight - temp.x;
	}

	return temp;
}

std::vector<SDL_Rect> rectangleMerge(std::vector<SDL_Rect> rects)
{
	int origSize = rects.size() + 1;//needs to be +1 to enter the loop
	vector<SDL_Rect> temp = rects;
	vector<SDL_Rect> copy;
	copy.clear();

	while (origSize != copy.size())//will keep on going until the number doesn't reduce
	{
		copy = temp;
		temp.clear();
		bool* used = new bool[copy.size()];//to keep track of whether a rectangle has been already merged into another
		
		//initializing the used array to all false
		for (int i = 0, j = copy.size(); i < j; ++i)
			used[i] = false;

		for (int i = 0, j = copy.size(); i < j; ++i)
		{
			for (int k = i + 1; k < j; ++k)
			{
				SDL_Rect glued = { -1,-1,-1,-1 };//the glued together resultant, with a default error
				if (!used[i] && !used[k])
					glued = glueRectangles(copy[i], copy[k]);
				if (glued.x != -1)//{-1,-1,-1,-1} is the default error, and I won't have any negative values in my game
				{
					used[i] = used[k] = true;
					temp.push_back(glued);
				}
			}
			
			if (!used[i]) temp.push_back(copy[i]);
		}
		delete used;

		/*
		//debugging stuff, just shows the steps
		for (int i = 0, j = temp.size(); i < j; ++i)
			cout << temp[i].x << ',' << temp[i].y << ',' << temp[i].w << ',' << temp[i].h << '|';
		cout << endl;
		*/

		origSize = temp.size();
	}
	return temp;
}

void stretchBox(SDL_Rect & a, int & xV, int & yV)
{
	//increase the box by 1 unit on every edge
	a.x -= 1; a.w += 2;
	a.y -= 1; a.h += 2;

	//expand the box based on the x and y velocity
	a.w += abs(xV);
	a.h += abs(yV);

	//negative velocities should make the rectangle
	//longer toward the direction of movement
	if (xV < 0)
		a.x += xV;
	if (yV < 0)
		a.y += yV;
}

void stretchBoxUndo(SDL_Rect & a, int & xV, int & yV)
{
	//decrease the box by 1 unit on every edge
	a.x += 1; a.w -= 2;
	a.y += 1; a.h -= 2;

	a.w -= abs(xV);
	a.h -= abs(yV);

	if (xV < 0)
		a.x -= xV;
	if (yV < 0)
		a.y -= yV;
}
