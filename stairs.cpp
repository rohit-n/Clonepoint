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

#include "entity.h"

Stairs::Stairs(float x, float y) : Entity(x, y)
{
	_upstairs = nullptr;
	_downstairs = nullptr;
	_well = nullptr;

	setCollisionRectDims(30, 42, ENTDIM);
	_sprite = Locator::getSpriteManager()->getIndex("./data/sprites/objects.sprites", "stairs");
}

void Stairs::registerStairwell(Stairwell* well)
{
	_well = well;
}

Stairs* Stairs::getUpstairs()
{
	return _upstairs;
}

Stairs* Stairs::getDownstairs()
{
	return _downstairs;
}

void Stairs::setUpstairs(Stairs* target)
{
	_upstairs = target;
	if (target->getDownstairs() == nullptr)
		target->setDownstairs(this);
}

void Stairs::setDownstairs(Stairs* target)
{
	_downstairs = target;
	if (target->getUpstairs() == nullptr)
		target->setUpstairs(this);
}

Stairwell::Stairwell(int x)
{
	_x = x;
}

Stairwell::~Stairwell()
{
	_stairs.clear();
}

void Stairwell::addStairs(Stairs* stairs)
{
	_stairs.push_back(stairs);
}

int Stairwell::getX()
{
	return _x;
}

void Stairwell::setDirections(unsigned int yMax)
{
	size_t i;
	size_t prevStairs = 0;
	unsigned int yMin = yMax;
	unsigned int y;

	for (i = 0; i < _stairs.size(); i++)
	{
		if (_stairs[i]->getPosition().y < yMin)
		{
			yMin = _stairs[i]->getPosition().y;
			prevStairs = i;
		}
	}

	//stair positions are unordered, so scan down and find the downstairs of above stairs!

	for (y = yMin; y < yMax; y++)
	{
		for (i = 0; i < _stairs.size(); i++)
		{
			if (_stairs[i]->getPosition().y == y && _stairs[prevStairs] != _stairs[i])
			{
				_stairs[prevStairs]->setDownstairs(_stairs[i]);
				prevStairs = i;
				break;
			}
		}
	}
	_stairs.clear();
}