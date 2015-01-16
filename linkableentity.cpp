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

#define TRAPDOOR_TIMEMS 2000
#define VAULTDOOR_TIMEMS 3000

LinkableEntity::LinkableEntity(float x, float y, Circuit c) : Entity(x, y)
{
	_circuit = c;
	_other = nullptr;
}

LinkableEntity* LinkableEntity::getTarget()
{
	return _other;
}

Circuit LinkableEntity::getCircuitType()
{
	return _circuit;
}

void LinkableEntity::link(LinkableEntity* target, bool playSound)
{
	if (this == target || _circuit != target->getCircuitType())
	{
		return;
	}

	if (playSound)
	{
		Locator::getAudio()->playSound("link");
	}
	_other = target;
}

void LinkableEntity::unlink()
{
	_other = nullptr;
}

void LinkableEntity::activate()
{
	if (_other != nullptr)
	{
		//TODO: Find some other way of avoiding a segfault.
		//I would rather allow for entities to activate each other at least for a time.
		if (!hasCycle())
		{
			_other->activate();
		}

		if (dynamic_cast<EnemyGun*>(_other))
		{
			unlink();
		}
	}
}

//Thank you, Robert Floyd.
bool LinkableEntity::hasCycle()
{
	if(_other == nullptr)
	{
		return false;
	}
	LinkableEntity* slow;
	LinkableEntity* fast;

	slow = fast = this;

	while(true)
	{
		slow = slow->getTarget();

		if(fast->getTarget() != nullptr)
			fast = fast->getTarget()->getTarget();
		else
			return false;

		if(slow == nullptr || fast == nullptr)
			return false;

		if(slow == fast)
			return true;
	}
}

LightSwitch::LightSwitch(float x, float y, Circuit c, bool hs) : LinkableEntity(x, y, c)
{
	_isHandScanner = hs;
	std::string sprite;
	if (!_isHandScanner)
	{
		setCollisionRectDims(20, 16, ENTDIM);
		sprite = "lightswitch";
	}
	else
	{
		setCollisionRectDims(6, 10, ENTDIM);
		sprite = "handscanner";
	}
	_sprite = Locator::getSpriteManager()->getIndex("./data/sprites/linkable.sprites", sprite);
}

void LightSwitch::activate()
{
	if (!_isHandScanner)
	{
		Locator::getAudio()->playSound("switch");
	}
	LinkableEntity::activate();
}

bool LightSwitch::isHandScanner()
{
	return _isHandScanner;
}

ElevatorSwitch::ElevatorSwitch(float x, float y, Circuit c) : LinkableEntity(x, y, c)
{
	setCollisionRectDims(8, 16, ENTDIM);
	_sprite = Locator::getSpriteManager()->getIndex("./data/sprites/linkable.sprites", "elev_switch_off");
}

void ElevatorSwitch::registerDoor(ElevatorDoor* door)
{
	_door = door;
}

ElevatorDoor* ElevatorSwitch::getElevatorDoor()
{
	return _door;
}

void ElevatorSwitch::activate()
{
	if (_door->getShaft()->getOpenDoor() != _door)
	{
		_door->getShaft()->setTarget(_door);
	}
}

void ElevatorSwitch::activateTarget()
{
	LinkableEntity::activate();
}

void ElevatorSwitch::changeSprite(unsigned int sprite)
{
	_sprite = sprite;
}

Door::Door(float x, float y, Circuit c, bool open, DoorType type) : LinkableEntity(x, y, c)
{
	_type = type;
	_cvol = new CollisionVolume();
	_cvol2 = nullptr;

	switch(type)
	{
	case Door_Trap:
		setCollisionRectDims(96, 12, ENTDIM);
		_opened = false;
		_sprite = Locator::getSpriteManager()->getIndex("./data/sprites/linkable.sprites", "trapdoorclosed");
		break;
	case Door_Vault:
		setCollisionRectDims(16, 128, ENTDIM);
		_opened = false;
		_sprite = Locator::getSpriteManager()->getIndex("./data/sprites/linkable.sprites", "vault_closed");
		_cvol2 = new CollisionVolume();
		break;
	default:
		setCollisionRectDims(8, 48, ENTDIM);
		_opened = open;
		_sprite = Locator::getSpriteManager()->getIndex("./data/sprites/linkable.sprites", open ? "dooropen" : "doorclosed");
		break;
	}

	if (type != Door_Vault)
	{
		_cvol->rect.x = _collisionRect.x;
		_cvol->rect.y = _collisionRect.y;
		_cvol->rect.w = _collisionRect.w;
		_cvol->rect.h = _collisionRect.h;
	}
	else
	{
		_cvol->rect.x = _collisionRect.x + 4;
		_cvol->rect.y = _collisionRect.y + 76;
		_cvol->rect.w = _collisionRect.w - 8;
		_cvol->rect.h = _collisionRect.h - 76;

		_cvol2->rect.x = _collisionRect.x;
		_cvol2->rect.y = _collisionRect.y;
		_cvol2->rect.w = _collisionRect.w;
		_cvol2->rect.h = 76;
		_cvol2->active = true;
	}

	_cvol->active = !open;
	_cvol->glass = false;
	_cvol->door = true;
	_cvol->guardblock = false;
	_timeToClose = _type == Door_Trap ? TRAPDOOR_TIMEMS : VAULTDOOR_TIMEMS;
	_dirty = false;
}

CollisionVolume* Door::getCollisionVolume()
{
	return _cvol;
}

CollisionVolume* Door::getCollisionVolume2()
{
	return _cvol2;
}

void Door::activate()
{
	if (_opened)
		closeSound();
	else
		openSound();

	updateCollisionVolume();
	LinkableEntity::activate();
}

bool Door::isOpened()
{
	return _opened;
}

void Door::open()
{
	std::string name;

	switch(_type)
	{
	case Door_Trap:
		name = "trapdooropen";
		break;
	case Door_Vault:
		name = "vault_open";
		changeAnimationSequence(Locator::getAnimationManager()->getSequence(ANIM_VAULT_OPEN));
		break;
	default:
		name = "dooropen";
		break;
	}

	_opened = true;
	_dirty = true;
	_sprite = Locator::getSpriteManager()->getIndex("./data/sprites/linkable.sprites", name);
}

void Door::close()
{
	std::string name;

	switch(_type)
	{
	case Door_Trap:
		name = "trapdoorclosed";
		break;
	case Door_Vault:
		name = "vault_closed";
		changeAnimationSequence(Locator::getAnimationManager()->getSequence(ANIM_VAULT_CLOSE));
		break;
	default:
		name = "doorclosed";
		break;
	}

	_opened = false;
	_dirty = true;
	_timeToClose = _type == Door_Trap ? TRAPDOOR_TIMEMS : VAULTDOOR_TIMEMS;
	_sprite = Locator::getSpriteManager()->getIndex("./data/sprites/linkable.sprites", name);
}

void Door::openSound()
{
	Locator::getAudio()->playSound("door_open");
	open();
}

void Door::closeSound()
{
	Locator::getAudio()->playSound("door_close");
	close();
}

void Door::update(unsigned int dT)
{
	Entity::update(dT);
	if (_type != Door_Normal)
	{
		if (_type == Door_Vault && _activeSequence)
		{
			if (_currentAnimFinished)
			{
				changeToStaticSprite(Locator::getSpriteManager()->getIndex("./data/sprites/linkable.sprites", _opened ? "vault_open" : "vault_closed"));
			}
		}
		if (_opened)
		{
			_timeToClose -= dT;
			if (_timeToClose <= 0)
			{
				closeSound();
				updateCollisionVolume();
			}
		}
	}
}

void Door::updateCollisionVolume()
{
	_cvol->active = !_opened;
}

size_t Door::getNumberOfOverlappingLights()
{
	return _overlappingLights.size();
}

FieldOfView* Door::getLightAndAnglesAt(int i, int* angle1, int* angle2)
{
	*angle1 = _overlappingLights[i].angle1;
	*angle2 = _overlappingLights[i].angle2;
	return _overlappingLights[i].fov;
}

void Door::addOverlappingLight(FieldOfView* fov, int angle1, int angle2)
{
	LightAndAngles laa;
	laa.fov = fov;
	laa.angle1 = angle1;
	laa.angle2 = angle2;
	_overlappingLights.push_back(laa);
}

bool Door::isDirty()
{
	return _dirty;
}

void Door::setDirty(bool b)
{
	_dirty = b;
}

DoorType Door::getType()
{
	return _type;
}

int Door::getTimeToClose()
{
	return _timeToClose;
}

MotionScanner::MotionScanner(float x, float y, Circuit c) : LinkableEntity(x, y, c)
{
	setCollisionRectDims(14, 48, ENTDIM);
	_trespassed = false;
	_trespasser = nullptr;
	_sprite = Locator::getSpriteManager()->getIndex("./data/sprites/linkable.sprites", "motionscanner");
}

bool MotionScanner::isTrespassed()
{
	return _trespassed;
}

Entity* MotionScanner::getTrespasser()
{
	return _trespasser;
}

void MotionScanner::setTrespassed(bool b)
{
	_trespassed = b;
}

void MotionScanner::setTrespasser(Entity* ent)
{
	_trespasser = ent;
	_trespassed = true;
}

void MotionScanner::resetTrespasser()
{
	_trespassed = false;
	_trespasser = nullptr;
}

SecurityCamera::SecurityCamera(float x, float y, Circuit c, Direction dir, FieldOfView* fov) : LinkableEntity(x, y, c)
{
	setCollisionRectDims(22, 20, ENTDIM);
	_trespassed = false;
	_direction = dir;
	_fov = fov;
	_sprite = Locator::getSpriteManager()->getIndex("./data/sprites/linkable.sprites", dir == Right ? "camera_right" : "camera_left");
}

void SecurityCamera::activate()
{
	if (_trespassed)
	{
		return;
	}
	LinkableEntity::activate();
}

bool SecurityCamera::isTrespassed()
{
	return _trespassed;
}

void SecurityCamera::setTrespassed(bool b)
{
	_trespassed = b;
}

FieldOfView* SecurityCamera::getFOV()
{
	return _fov;
}

LightFixture::LightFixture(float x, float y, Circuit c, bool switchedOn) : LinkableEntity(x, y, c)
{
	setCollisionRectDims(79, 6, ENTDIM);
	_switchedOn = switchedOn;
	_sprite = Locator::getSpriteManager()->getIndex("./data/sprites/linkable.sprites", "lightfixture");
}

LightFixture::~LightFixture()
{
	_lights.clear();
}

void LightFixture::activate()
{
	setSwitchedOn(!_switchedOn);
	LinkableEntity::activate();
}

void LightFixture::setSwitchedOn(bool sw)
{
	_switchedOn = sw;
	toggleAllFOVs();
}

bool LightFixture::isSwitchedOn()
{
	return _switchedOn;
}

void LightFixture::addFOV(FieldOfView* fov)
{
	fov->registerLightFixture(this);
	_lights.push_back(fov);
}

void LightFixture::toggleAllFOVs()
{
	for(auto& elem : _lights)
		elem->setActive(_switchedOn);
}

PowerSocket::PowerSocket(float x, float y, Circuit c) : LinkableEntity(x, y, c)
{
	setCollisionRectDims(9, 5, ENTDIM);
	_live = false;
	_sprite = Locator::getSpriteManager()->getIndex("./data/sprites/linkable.sprites", "powersocket");
}

void PowerSocket::activate()
{
	LinkableEntity::activate();
	//TODO: Play sound here.
	_live = true;
}

void PowerSocket::deactivate()
{
	_live = false;
}

bool PowerSocket::isLive()
{
	return _live;
}

SoundDetector::SoundDetector(float x, float y, Circuit c) : LinkableEntity(x, y, c)
{
	setCollisionRectDims(24, 26, ENTDIM);
	_sprite = Locator::getSpriteManager()->getIndex("./data/sprites/linkable.sprites", "sounddetector");
	_soundedAlarm = false;
}

void SoundDetector::unlink()
{
	LinkableEntity::unlink();
	_soundedAlarm = false;
}

void SoundDetector::activate()
{
	bool hasAlarm = false;
	if (!hasCycle())
	{
		LinkableEntity* ent = this;
		while (ent != nullptr)
		{
			if (dynamic_cast<Alarm*>(ent))
			{
				hasAlarm = true;
				break;
			}
			ent = ent->getTarget();
		}
	}
	else
	{
		return;
	}

	if (hasAlarm)
	{
		if (!_soundedAlarm)
		{
			_soundedAlarm = true;
		}
		else
		{
			return;
		}
	}

	LinkableEntity::activate();
}

Alarm::Alarm(float x, float y, Circuit c) : LinkableEntity(x, y, c)
{
	setCollisionRectDims(10, 32, ENTDIM);
	_sprite = Locator::getSpriteManager()->getIndex("./data/sprites/linkable.sprites", "alarm_off");
	_sounded = false;
}

void Alarm::activate()
{
	changeAnimationSequence(Locator::getAnimationManager()->getSequence(ANIM_ALARM_ACTIVE));
	setSounded(true);
	Locator::getAudio()->playSound("alarm");
}

void Alarm::deactivate()
{
	changeToStaticSprite(Locator::getSpriteManager()->getIndex("./data/sprites/linkable.sprites", "alarm_off"));
}

void Alarm::setSounded(bool b)
{
	_sounded = b;
}

bool Alarm::isSounded()
{
	return _sounded;
}

void Alarm::setAnimating(bool b)
{
	if (b)
	{
		changeAnimationSequence(Locator::getAnimationManager()->getSequence(ANIM_ALARM_ACTIVE));
	}
	else
	{
		changeToStaticSprite(Locator::getSpriteManager()->getIndex("./data/sprites/linkable.sprites", "alarm_off"));
	}
}

bool Alarm::isActivated()
{
	return !_currentAnimFinished;
}

EnemyGun::EnemyGun(float x, float y, Circuit c) : LinkableEntity(x, y, c)
{
	setCollisionRectDims(8, 8, ENTDIM);
	_sprite = 0;
	_enemy = nullptr;
}

void EnemyGun::activate()
{
	LinkableEntity::activate();
	_enemy->_fireWeapon(Shot_FromEnemyInvoluntary);
}

void EnemyGun::fire(GunShotTraceType gstt)
{
	LinkableEntity* other = _other; //if this gun is linked to another, _other may be made null when activating.
	LinkableEntity::activate();
	if (other == nullptr)
	{
		_enemy->_fireWeapon(gstt);
	}
	else
	{
		_enemy->setReactionTime(1000);
	}
}

void EnemyGun::update(unsigned int dT)
{
	_position = _enemy->getPosition();
	updateCollisionRectPosition();
}

void EnemyGun::setEnemy(Enemy* enemy)
{
	_enemy = enemy;
}

Enemy* EnemyGun::getEnemy()
{
	return _enemy;
}