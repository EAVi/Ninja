#include "Algorithms.h"

using namespace std;

const int tilesize = 16;

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


/*
*/
SDL_Point optimizedMove(SDL_Rect& player, vector<SDL_Rect>& boxes, int& xV, int& yV, Uint8& touchIndex)
{
	SDL_Rect tempRect = player;
	tempRect.x += xV;
	tempRect.y += yV;

	//v0.4 to get a good index, expand all edges by 1 
	//before getting the collision index, then unexpand
	tempRect.x -= 1; tempRect.y -= 1;
	tempRect.w += 2; tempRect.h += 2;

	//checks collision for intended movement
	vector<SDL_Rect> colliders = getCollisionIndex(tempRect, boxes);

	//v0.4 unexpand
	tempRect.x += 1; tempRect.y += 1;
	tempRect.w -= 2; tempRect.h -= 2;


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

/*
SDL_Point optimizedMove(SDL_Rect& player, vector<SDL_Rect>& boxes, int& xV, int& yV, Uint8& touchIndex)
{
	SDL_Rect tempRect = player;
	tempRect.x += xV;
	tempRect.y += yV;

	//v0.4 to get a good index, expand all edges by 1 
	//before getting the collision index, then unexpand
	tempRect.x -= 1; tempRect.y -= 1;
	tempRect.w += 2; tempRect.h += 2;

	//checks collision for intended movement
	vector<SDL_Rect> colliders = getCollisionIndex(tempRect, boxes);

	//v0.4 unexpand
	tempRect.x += 1; tempRect.y += 1;
	tempRect.w -= 2; tempRect.h -= 2;

	
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
		else while (tempRect.x < player.x)
		{
			if (!checkCollision(tempRect, colliders))
				break;
			tempRect.x++;
		}
		
	if (xV > 0)
		if ((touchIndex & kTouchingLeft)) tempRect.x = player.x;
		else while (tempRect.x > player.x)
		{
			if (!checkCollision(tempRect, colliders))
				break;
			tempRect.x--;
		}

	tempRect.y = player.y + yV;
	//Y testing, same process, but touching collisions will reset velocity
	if (yV < 0)
		if (touchIndex & kTouchingBottom)
		{
			tempRect.y = player.y;
			yV = 0;
		}
		else while (tempRect.y < player.y)
		{
			if (!checkCollision(tempRect, colliders))
				break;
			tempRect.y++;
		}

	if (yV > 0)
		if (touchIndex & kTouchingTop)
		{
			tempRect.y = player.y;
			yV = 0;
		}
		else while (tempRect.y > player.y)
		{
			if (!checkCollision(tempRect, colliders))
				break;
			tempRect.y--;
		}

	//return the optimized point
	return{tempRect.x, tempRect.y};
}
*/


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