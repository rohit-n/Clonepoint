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

Entity::Entity(float x, float y)
{
	_position.x = x;
	_position.y = y;

	_velocity.x = 0;
	_velocity.y = 0;

	_offset.x = 0;
	_offset.y = 0;

	_rotation = 0.0f;

	_activeSequence = NULL;

	_sprite = 0;

	_animDT = 0;
	_currentSequenceIndex = 0;
	_currentAnimFinished = true;
	_highlighted = false;
}

Entity::Entity(float x, float y, unsigned int sprite)
{
	//code duplicated from above to avoid g++ delegation warning.
	_position.x = x;
	_position.y = y;

	_velocity.x = 0;
	_velocity.y = 0;

	_offset.x = 0;
	_offset.y = 0;

	_rotation = 0.0f;

	_activeSequence = NULL;

	_sprite = sprite;

	_animDT = 0;
	_currentSequenceIndex = 0;
	_currentAnimFinished = false;
}

Entity::~Entity()
{
}

vec2f Entity::getPosition()
{
	return _position;
}

void Entity::setPosition(float x, float y)
{
	_position.x = x;
	_position.y = y;

	_collisionRect.x = x + _offset.x;
	_collisionRect.y = y + _offset.y;
}

void Entity::setCollisionRectPosition(float x, float y)
{
	_collisionRect.x = x;
	_collisionRect.y = y;

	_position.x = x - _offset.x;
	_position.y = y - _offset.y;
}

void Entity::setPosition(vec2f newPos)
{
	_position = newPos;
}

void Entity::setVelocity(float x, float y)
{
	_velocity.x = x;
	_velocity.y = y;
}

void Entity::setPosX(float x)
{
	_position.x = x;
}

void Entity::setPosY(float y)
{
	_position.y = y;
}

void Entity::setVelX(float x)
{
	_velocity.x = x;
}

void Entity::setVelY(float y)
{
	_velocity.y = y;
}

vec2f Entity::getVelocity()
{
	return _velocity;
}

float Entity::getRotation()
{
	return _rotation;
}

void Entity::update(unsigned int dT)
{
	_position.x += _velocity.x * dT;
	_position.y += _velocity.y * dT;

	if (_activeSequence)
	{
		_sprite = Locator::getAnimationManager()->getNextSprite(_activeSequence, &_currentSequenceIndex, &_animDT, dT, &_currentAnimFinished);
	}
}

Rect Entity::getCollisionRect()
{
	return _collisionRect;
}

vec2f Entity::getCollisionRectPosition()
{
	return vec2f(_collisionRect.x, _collisionRect.y);
}

vec2f Entity::getCollisionRectCenterPosition()
{
	return vec2f(_collisionRect.x + _collisionRect.w / 2, _collisionRect.y + + _collisionRect.h / 2);
}

void Entity::setCollisionRectDims(float w, float h, int entDim)
{
	_collisionRect.w = w;
	_collisionRect.h = h;

	_offset.x = (entDim - w) / 2;
	_offset.y = (entDim - h) / 2;

	updateCollisionRectPosition();
}

void Entity::setCollisionRectDimsAndPosition(float x, float y, float w, float h, int entDim)
{
	setCollisionRectDims(w, h, entDim);
	setCollisionRectPosition(x, y);
}

void Entity::updateCollisionRectPosition()
{
	_collisionRect.x = _position.x + _offset.x;
	_collisionRect.y = _position.y + _offset.y;
}

unsigned int Entity::getCurrentSprite()
{
	return _sprite;
}

void Entity::changeToStaticSprite(unsigned int sprite)
{
	_sprite = sprite;
	_activeSequence = NULL;
	_currentAnimFinished = true;
}

void Entity::changeAnimationSequence(AnimationSequence* sequence)
{
	if (_activeSequence != sequence)
	{
		changeAnimationSequenceForce(sequence);
	}
}

void Entity::changeAnimationSequenceForce(AnimationSequence* sequence)
{
	_activeSequence = sequence;
	if (_activeSequence)
	{
		_sprite = _activeSequence->getSpriteAt(0);
		_animDT = _activeSequence->getMsPerFrame();
		_currentSequenceIndex = 0;
		_currentAnimFinished = false;
	}
}

Particle::Particle(float x, float y, unsigned int sprite) : Entity(x, y, sprite)
{
	setCollisionRectDims(8, 8, 8);
	_alive = false;
}

void Particle::update(unsigned int dT)
{
	Entity::update(dT);
	_velocity.y += GRAVITY;
	updateCollisionRectPosition();
}

TutorialMark::TutorialMark(float x, float y, StringMessage ts) : Entity(x, y)
{
	setCollisionRectDims(32, 48, ENTDIM);
	_ts = ts;
	_sprite = Locator::getSpriteManager()->getIndex("./data/sprites/objects.sprites", "tutorial");
}

MainComputer::MainComputer(float x, float y, bool active) : Entity(x, y)
{
	setCollisionRectDims(64, 32, ENTDIM);
	_active = active;

	_sprite = Locator::getSpriteManager()->getIndex("./data/sprites/objects.sprites", _active ? "terminalon" :  "terminaloff");
}

bool MainComputer::isActive()
{
	return _active;
}

void MainComputer::setActive(bool b)
{
	_active = b;
	_sprite = Locator::getSpriteManager()->getIndex("./data/sprites/objects.sprites", _active ? "terminalon" :  "terminaloff");
}

CircuitBox::CircuitBox(float x, float y, Circuit circuit) : Entity(x, y)
{
	setCollisionRectDims(6, 34, ENTDIM);
	_hacked = false;
	_circuit = circuit;
	std::string sprite;
	switch (_circuit)
	{
	case BLUE:
		sprite = "circuitboxblue";
		break;
	case GREEN:
		sprite = "circuitboxgreen";
		break;
	case VIOLET:
		sprite = "circuitboxviolet";
		break;
	case YELLOW:
		sprite = "circuitboxyellow";
		break;
	default:
		sprite = "";
		break;
	}
	_sprite = sprite != "" ? Locator::getSpriteManager()->getIndex("./data/sprites/objects.sprites", sprite) : 0;
}

bool CircuitBox::isHacked()
{
	return _hacked;
}

void CircuitBox::setHacked(bool hacked, bool playSound)
{
	if (_hacked && hacked)
	{
		return;
	}
	_hacked = hacked;
	if (_hacked && playSound)
	{
		Locator::getAudio()->playSound("circuitbox");
	}
}

Circuit CircuitBox::getCircuit()
{
	return _circuit;
}

void setAccel(Acceleration* accel, bool accelerating, float start, float target)
{
	accel->accelerating = accelerating;
	accel->accel = start;
	accel->target = target;
}