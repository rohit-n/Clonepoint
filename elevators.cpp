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

ElevatorDoor::ElevatorDoor(float x, float y) : Entity(x, y)
{
	setCollisionRectDims(32, 48, ENTDIM);
	_open = false;
	_switch = nullptr;
	_sprite = Locator::getSpriteManager()->getIndex("./data/sprites/objects.sprites", "elevatorclosed");
}

void ElevatorDoor::open(bool animate)
{
	_open = true;
	_switch->activateTarget();
	_switch->changeSprite(Locator::getSpriteManager()->getIndex("./data/sprites/linkable.sprites", "elev_switch_open"));
	if (!isClosing())
	{
		if (animate)
		{
			Locator::getAudio()->playSound("elevator_arrive");
			changeAnimationSequence(Locator::getAnimationManager()->getSequence(ANIM_ELEVATOR_OPEN));
		}
		else
		{
			changeToStaticSprite(Locator::getSpriteManager()->getIndex("./data/sprites/objects.sprites", "elevatoropen"));
		}
	}
	else
	{
		changeToStaticSprite(Locator::getSpriteManager()->getIndex("./data/sprites/objects.sprites", "elevatorclosed"));
	}

}

void ElevatorDoor::close(bool animate)
{
	_open = false;
	_switch->changeSprite(Locator::getSpriteManager()->getIndex("./data/sprites/linkable.sprites", "elev_switch_off"));
	if (!animate)
	{
		changeToStaticSprite(Locator::getSpriteManager()->getIndex("./data/sprites/objects.sprites", "elevatorclosed"));
	}
	else
	{
		changeAnimationSequence(Locator::getAnimationManager()->getSequence(ANIM_ELEVATOR_CLOSE));
	}
}

bool ElevatorDoor::isOpen()
{
	return _open;
}

void ElevatorDoor::registerShaft(ElevatorShaft* shaft)
{
	_shaft = shaft;
}

void ElevatorDoor::registerSwitch(ElevatorSwitch* eSwitch)
{
	_switch = eSwitch;
}

ElevatorShaft* ElevatorDoor::getShaft()
{
	return _shaft;
}

void ElevatorDoor::update(unsigned int dT)
{
	Entity::update(dT);
	if (isOpening() && _currentAnimFinished)
	{
		changeToStaticSprite(Locator::getSpriteManager()->getIndex("./data/sprites/objects.sprites", "elevatoropen"));
	}

	if (isClosing() && _currentAnimFinished)
	{
		changeToStaticSprite(Locator::getSpriteManager()->getIndex("./data/sprites/objects.sprites", "elevatorclosed"));
	}
}

bool ElevatorDoor::isOpening()
{
	return _activeSequence == Locator::getAnimationManager()->getSequence(ANIM_ELEVATOR_OPEN);
}

bool ElevatorDoor::isClosing()
{
	return _activeSequence == Locator::getAnimationManager()->getSequence(ANIM_ELEVATOR_CLOSE);
}

ElevatorSwitch* ElevatorDoor::getSwitch()
{
	return _switch;
}

ElevatorShaft::ElevatorShaft(int x)
{
	_moving = false;
	_waitingForClose = false;
	_target = nullptr;
	_x = x;
	_openDoor = nullptr;
	_elevatorPosition = vec2f(_x, 0);
	_yVel = 0;

	_rect.x = _x + 48;
	_rect.y = 0;
	_rect.w = 32;
	_rect.h = 48;

	_acceleration.accelerating = false;
	_acceleration.accel = 0.0f;
	_acceleration.target = 0.0f;
}

ElevatorShaft::~ElevatorShaft()
{
	_doors.clear();
}

Rect ElevatorShaft::getRect()
{
	return _rect;
}

vec2f ElevatorShaft::getElevatorPosition()
{
	return _elevatorPosition;
}

void ElevatorShaft::update()
{
	if (_moving)
	{
		if (!_acceleration.accelerating && fabs(_elevatorPosition.y - _target->getCollisionRectPosition().y) < fabs(_yVel) * 32.0f)
		{
			//start slowing down to destination.
			setAccel(&_acceleration, true, _yVel < 0.0f ? 0.02f : -0.02f, _yVel < 0.0f ? -0.1f : 0.1f);
			Locator::getAudio()->playSound("elevator_decelerate");
		}
		if (_acceleration.accelerating)
		{
			if (fabs(_yVel - _acceleration.target) < fabs(_acceleration.accel))
			{
				_acceleration.accelerating = false;
				_acceleration.accel = 0.0f;
				_yVel = _acceleration.target;
			}
			_yVel += _acceleration.accel;
		}
		_elevatorPosition.y += _yVel;
		_rect.y = _elevatorPosition.y;
	}

	if (_waitingForClose && !_openDoor->isClosing() && _target != nullptr)
	{
		_yVel = _target->getCollisionRectPosition().y - _elevatorPosition.y;
		_yVel /= fabs(_yVel);
		_yVel *= 0.1f;
		setAccel(&_acceleration, true, _yVel < 0.0f ? -0.01f : 0.01f, _yVel < 0.0f ? -1.5f : 1.5f);
		setMoving(true);
		_waitingForClose = false;
		Locator::getAudio()->playSound("elevator_leave");
	}
}

int ElevatorShaft::getX()
{
	return _x;
}

void ElevatorShaft::setOpenDoor(ElevatorDoor* door, bool animate)
{
	int index = containsDoor(door);
	if (index < 0)
	{
		return;
	}

	_yVel = 0;
	_openDoor = door;
	_openDoor->open(animate);
	_moving = false;

	for (ElevatorDoor* door : _doors)
	{
		if (door != _openDoor)
		{
			door->close(false);
		}
	}

	_elevatorPosition.y = door->getCollisionRectPosition().y;
	_rect.y = _elevatorPosition.y;
}

void ElevatorShaft::setOpenDoorFirst()
{
	setOpenDoor(_doors[0], false);
}

void ElevatorShaft::setTarget(ElevatorDoor* target)
{
	int index = containsDoor(target);
	if (index < 0)
	{
		return;
	}

	_openDoor->close(true);
	_waitingForClose = true;
	_target = target;
	_target->getSwitch()->changeSprite(Locator::getSpriteManager()->getIndex("./data/sprites/linkable.sprites", "elev_switch_wait"));
}

void ElevatorShaft::addDoor(ElevatorDoor* ed)
{
	_doors.push_back(ed);
}

bool ElevatorShaft::isMoving()
{
	return _moving;
}

int ElevatorShaft::containsDoor(ElevatorDoor* door)
{
	unsigned int i;
	for (i = 0; i < _doors.size(); i++)
	{
		if (_doors[i] == door)
		{
			return (int)i;
		}
	}
	return -1;
}

void ElevatorShaft::setMoving(bool b)
{
	_moving = b;
}

ElevatorDoor* ElevatorShaft::getOpenDoor()
{
	return _openDoor;
}

ElevatorDoor* ElevatorShaft::getTarget()
{
	return _target;
}

void ElevatorShaft::calculateDoorOrders(int yMax)
{
	size_t i;
	size_t prevDoor = -1;
	float yMin = (float)yMax;
	int y;
	for (i = 0; i < _doors.size(); i++)
	{
		if (_doors[i]->getPosition().y < yMin)
		{
			yMin = _doors[i]->getPosition().y;
			prevDoor = i;
		}
	}

	_order.push_back(prevDoor);

	//door positions are unordered in vector, so scan down and get the real order!

	for (y = (int)yMin; y < yMax; y++)
	{
		for (i = 0; i < _doors.size(); i++)
		{
			if (_doors[i]->getPosition().y == y && _doors[prevDoor] != _doors[i])
			{
				prevDoor = i;
				_order.push_back(prevDoor);
				break;
			}
		}
	}
}

ElevatorDoor* ElevatorShaft::getDoorAbove(ElevatorDoor* door)
{
	int index = getDoorIndexOrdered(containsDoor(door));
	if (index <= 0)
	{
		return nullptr; //door is already the highest or does not exist in this shaft
	}

	return _doors[_order[index - 1]];
}

ElevatorDoor* ElevatorShaft::getDoorBelow(ElevatorDoor* door)
{
	int index = getDoorIndexOrdered(containsDoor(door));
	if (index == -1 || index + 1 >= (int)_order.size())
	{
		return nullptr; //door is already the lowest or does not exist in this shaft
	}

	return _doors[_order[index + 1]];
}

int ElevatorShaft::getDoorIndexOrdered(int index)
{
	unsigned int i;
	for (i = 0; i < _order.size(); i++)
	{
		if (_order[i] == index)
		{
			return (int)i;
		}
	}
	return -1;
}

float ElevatorShaft::getVelocity()
{
	return _yVel;
}