#include "LevelID.h"

bool operator==(LevelID& a, LevelID& b)
{
	return ((a.LevelNo == b.LevelNo) &&
		(a.ZoneNo == b.ZoneNo));
}