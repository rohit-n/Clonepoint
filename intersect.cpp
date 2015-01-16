/*
Copyright 2013-2015 Rohit Nirmal

This file is part of Clonepoint.

Clonepoint is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Clonepoint is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Clonepoint.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "intersect.h"

bool vec2InRect(vec2f v, Rect r)
{
	return (v.x >= r.x && v.x <= r.x + r.w &&
	        v.y >= r.y && v.y <= r.y + r.h);
}

bool vec2InRect2(vec2f v, Rect r)
{
	return (v.x > r.x && v.x < r.x + r.w &&
	        v.y > r.y && v.y < r.y + r.h);
}

bool check_collision(Rect A, Rect B)
{
	//The sides of the rectangles
	float leftA, leftB;
	float rightA, rightB;
	float topA, topB;
	float bottomA, bottomB;

	//Calculate the sides of rect A
	leftA = A.x;
	rightA = A.x + A.w;
	topA = A.y;
	bottomA = A.y + A.h;

	//Calculate the sides of rect B
	leftB = B.x;
	rightB = B.x + B.w;
	topB = B.y;
	bottomB = B.y + B.h;

	//If any of the sides from A are outside of B
	if( bottomA <= topB )
	{
		return false;
	}

	if( topA >= bottomB )
	{
		return false;
	}

	if( rightA <= leftB )
	{
		return false;
	}

	if( leftA >= rightB )
	{
		return false;
	}

	//If none of the sides from A are outside B
	return true;
}

bool check_collision2(Rect A, Rect B)
{
	//The sides of the rectangles
	float leftA, leftB;
	float rightA, rightB;
	float topA, topB;
	float bottomA, bottomB;

	//Calculate the sides of rect A
	leftA = A.x;
	rightA = A.x + A.w;
	topA = A.y;
	bottomA = A.y + A.h;

	//Calculate the sides of rect B
	leftB = B.x;
	rightB = B.x + B.w;
	topB = B.y;
	bottomB = B.y + B.h;

	//If any of the sides from A are outside of B
	if( bottomA < topB )
	{
		return false;
	}

	if( topA > bottomB )
	{
		return false;
	}

	if( rightA < leftB )
	{
		return false;
	}

	if( leftA > rightB )
	{
		return false;
	}

	//If none of the sides from A are outside B
	return true;
}

bool rectInCircle(Rect r, vec2f center, float radius)
{
	if (vec2f_distance(center, vec2f(r.x, r.y)) <= radius ||
	        vec2f_distance(center, vec2f(r.x + r.w, r.y)) <= radius ||
	        vec2f_distance(center, vec2f(r.x, r.y + r.h)) <= radius ||
	        vec2f_distance(center, vec2f(r.x + r.w, r.y + r.h)) <= radius)
	{
		return true;
	}
	else return false;
}