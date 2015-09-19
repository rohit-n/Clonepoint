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
#define STAIRTRAVERSETIME 1000 //Time in milliseconds to fully traverse stairs.

LivingEntity::LivingEntity(float x, float y, Direction startingDir) : Entity(x, y)
{
	_onGround = false;
	_alive = true;
	_fixDirection = false;
	_affectedByGravity = true;
	_dir = startingDir;
	_stairsEntered = NULL;

	//stair related
	_traversal = NotMoving;
	_stairTimer = STAIRTRAVERSETIME;
	_overlappingStairs = NULL;

	_acceleration.accelerating = false;
	_acceleration.accel = 0.0f;
	_acceleration.target = 0.0f;
}

void LivingEntity::setAlive(bool b)
{
	_alive = b;
}

bool LivingEntity::isAlive()
{
	return _alive;
}

void LivingEntity::update(unsigned int dT)
{
	Entity::update(dT);

	if (!_onGround && _affectedByGravity)
	{
		_velocity.y += GRAVITY;
	}

	if (_alive)
	{
		if (!_fixDirection)
		{
			if (_velocity.x > 0 && _dir == Left)
				_dir = Right;

			if (_velocity.x < 0 && _dir == Right)
				_dir = Left;
		}
	}

	if (isMovingThroughStairs())
	{
		if (_stairTimer > 0)
		{
			_stairTimer -= dT;
		}
		else
		{
			_stairTimer = STAIRTRAVERSETIME;
			if (_overlappingStairs != NULL) //could be null if save game is loaded while moving through stairs.
			{
				Stairs* target = (_traversal == MovingUp) ? _overlappingStairs->getUpstairs() : _overlappingStairs->getDownstairs();

				if (target != NULL)
				{
					arriveAtStairs(target);
				}
			}
			_traversal = NotMoving;
		}
	}

	if (_acceleration.accelerating)
	{
		if (fabs(_velocity.x - _acceleration.target) < fabs(_acceleration.accel))
		{
			_acceleration.accelerating = false;
			_acceleration.accel = 0.0f;
			_velocity.x = _acceleration.target;
		}
		_velocity.x += _acceleration.accel;
	}
}

bool LivingEntity::isOnGround()
{
	return _onGround;
}

void LivingEntity::landOnGround()
{
	_velocity.y = 0.0f;
	_onGround = true;

	if (!_alive)
	{
		_velocity.x = 0.0f;
	}
}

void LivingEntity::setOnGround(bool b)
{
	_onGround = b;
}

Direction LivingEntity::getDirection()
{
	return _dir;
}

void LivingEntity::reverseDirection()
{
	if (_dir == Left)
		_dir = Right;
	else _dir = Left;
}

void LivingEntity::setStairMovement(StairTraversal st)
{
	//lock bounding box to stairs only if path through stairs exists.
	Stairs* target = (st == MovingUp) ? _overlappingStairs->getUpstairs() : _overlappingStairs->getDownstairs();

	if (target != NULL)
	{
		_stairsEntered = _overlappingStairs;
		_traversal = st;
		setCollisionRectPosition(_overlappingStairs->getCollisionRectPosition().x, _overlappingStairs->getCollisionRectPosition().y);
		setDirection(Right);
		_velocity.x = 0;
		_velocity.y = 0;
	}
}

StairTraversal LivingEntity::getStairTraversal()
{
	return _traversal;
}

void LivingEntity::setOverlappingStairs(Stairs* sw)
{
	_overlappingStairs = sw;
}

bool LivingEntity::isOverlappingStairs()
{
	return _overlappingStairs != NULL;
}

void LivingEntity::arriveAtStairs(Stairs* st)
{
	setCollisionRectPosition(st->getCollisionRectPosition().x, st->getCollisionRectPosition().y);
	_stairsEntered = NULL;
}

bool LivingEntity::isMovingThroughStairs()
{
	return _traversal != NotMoving;
}

int LivingEntity::getStairTimer()
{
	return _stairTimer;
}

void LivingEntity::setDirection(Direction dir)
{
	_dir = dir;
}

Acceleration* LivingEntity::getAccelerationStruct()
{
	return &_acceleration;
}

float LivingEntity::getAcceleration()
{
	return _acceleration.accel;
}

bool LivingEntity::isAccelerating()
{
	return _acceleration.accelerating;
}

Stairs* LivingEntity::getStairsEntered()
{
	return _stairsEntered;
}

bool LivingEntity::isPositionBehind(float x)
{
	return ((x > getCollisionRectPosition().x && _dir == Left) ||
	        (x < getCollisionRectPosition().x && _dir == Right));
}