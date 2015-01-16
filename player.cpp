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

Player::Player(float x, float y, Direction startingDir) : LivingEntity(x, y, startingDir)
{
	//TODO: Don't hardcode this.
	setCollisionRectDimsAndPosition(x, y, 18, 40, ENTDIM);

	_jumping = false;
	_pinning = false;
	_inElevator = false;
	_attach = NotAttached;
	_lastAttach = NotAttached;
	_attachedVolume = nullptr;
	_door = nullptr;
	_pinnedEnemy = nullptr;
	_hackedTerminal = nullptr;
	_numPunches = 0;
	_numHackedTerminals = 0;
	_armRotation = 0;
	_aimingGun = false;

	_runningSequence = Locator::getAnimationManager()->getSequence(ANIM_PLAYER_RUNNING);

	_climbingUpSequence = Locator::getAnimationManager()->getSequence(ANIM_PLAYER_CLIMBING_UP);

	_ceilingSequence = Locator::getAnimationManager()->getSequence(ANIM_PLAYER_CEILING);

	_hackingSequence = Locator::getAnimationManager()->getSequence(ANIM_PLAYER_HACKING);

	_attachDownSequence = Locator::getAnimationManager()->getSequence(ANIM_PLAYER_ATTACH_DOWN);

	_reachRoofSequence = Locator::getAnimationManager()->getSequence(ANIM_PLAYER_REACH_ROOF);

	_attachToCeiling = Locator::getAnimationManager()->getSequence(ANIM_PLAYER_ATTACH_TO_CEILING);

	_attachFromCeiling = Locator::getAnimationManager()->getSequence(ANIM_PLAYER_ATTACH_FROM_CEILING);

	_enterStairs = Locator::getAnimationManager()->getSequence(ANIM_PLAYER_ENTER_STAIRS);

	_exitStairs = Locator::getAnimationManager()->getSequence(ANIM_PLAYER_EXIT_STAIRS);

	_walkingSequence = Locator::getAnimationManager()->getSequence(ANIM_PLAYER_WALKING_AIMING);

	_activeSequence = _runningSequence;

	_lightVisibility = 0;
}

Player::~Player()
{
	LOGF((stdout, "running player destructor!\n"));
	_activeSequence = nullptr;
}

void Player::update(unsigned int dT)
{
	LivingEntity::update(dT);

	_fixDirection = _aimingGun;
	_affectedByGravity = (_attach == NotAttached);

	if (_attachedVolume != nullptr && !_attachedVolume->active)
	{
		detach();
	}

	if (_activeSequence == _hackingSequence && _currentAnimFinished)
	{
		_hackedTerminal->setActive(false);
		_hackedTerminal = nullptr;
		_numHackedTerminals++;
		changeToStaticSprite(Locator::getSpriteManager()->getPlayerSpriteIndex(_alive ? PLAYER_IDLE_GROUND : PLAYER_DEAD_GROUND));
	}

	if (_activeSequence == _attachDownSequence && _currentAnimFinished)
	{
		changeToStaticSprite(Locator::getSpriteManager()->getPlayerSpriteIndex(PLAYER_IDLE_SIDE));
	}

	if (_activeSequence == _reachRoofSequence && _currentAnimFinished)
	{
		changeToStaticSprite(Locator::getSpriteManager()->getPlayerSpriteIndex(PLAYER_IDLE_GROUND));
	}

	if (_activeSequence == _enterStairs && _currentAnimFinished)
	{
		// changeToStaticSprite(Locator::getSpriteManager()->getPlayerSpriteIndex(PLAYER_IDLE_SIDE));
		changeToStaticSprite(52);
	}

	if (_activeSequence == _exitStairs && _currentAnimFinished)
	{
		changeToStaticSprite(Locator::getSpriteManager()->getPlayerSpriteIndex(PLAYER_IDLE_SIDE));
	}

	if (_activeSequence == _attachToCeiling && _currentAnimFinished)
	{
		changeToStaticSprite(Locator::getSpriteManager()->getPlayerSpriteIndex(PLAYER_IDLE_CEILING));
	}

	if (_activeSequence == _attachFromCeiling && _currentAnimFinished)
	{
		changeToStaticSprite(Locator::getSpriteManager()->getPlayerSpriteIndex(PLAYER_IDLE_SIDE));
	}

	if (_inElevator)
	{
		int offset = _dir == Right ? 52 : 60;
		setCollisionRectPosition(_door->getShaft()->getElevatorPosition().x + offset, _door->getShaft()->getElevatorPosition().y + 8);
		_velocity.x = 0;
		_velocity.y = 0;
	}

	updateCollisionRectPosition();

	if (_activeSequence != _hackingSequence
	        && _activeSequence != _attachDownSequence
	        && _activeSequence != _reachRoofSequence
	        && _activeSequence != _attachToCeiling
	        && _activeSequence != _attachFromCeiling
	        && _activeSequence != _enterStairs
	        && _activeSequence != _exitStairs
	        && !_inElevator
	        && !isMovingThroughStairs())
	{
		if (_onGround)
		{
			if (_velocity.x != 0.0f)
			{
				changeAnimationSequence(_aimingGun ? _walkingSequence : _runningSequence);
			}
			else
			{
				if (_alive)
				{
					changeToStaticSprite(Locator::getSpriteManager()->getPlayerSpriteIndex(_aimingGun ? PLAYER_IDLE_GROUND_AIMING : PLAYER_IDLE_GROUND));
				}
				else
				{
					changeToStaticSprite(Locator::getSpriteManager()->getPlayerSpriteIndex(PLAYER_DEAD_GROUND));
				}
			}
		}
		else
		{
			if (_attach == Ceiling)
			{
				if (_velocity.x != 0.0f)
					changeAnimationSequence(_ceilingSequence);
				else
					changeToStaticSprite(Locator::getSpriteManager()->getPlayerSpriteIndex(PLAYER_IDLE_CEILING));
			}
			else if (_attach == LeftSide || _attach == RightSide)
			{
				if (_velocity.y > 0.0f)
					changeToStaticSprite(Locator::getSpriteManager()->getPlayerSpriteIndex(PLAYER_SLIDE_DOWN));
				else if (_velocity.y < 0.0f)
					changeAnimationSequence(_climbingUpSequence);
				else
					changeToStaticSprite(Locator::getSpriteManager()->getPlayerSpriteIndex(PLAYER_IDLE_SIDE));
			}
			else
			{
				if (_alive)
				{
					if (fabs(_velocity.x) > fabs(_velocity.y) && _sprite != Locator::getSpriteManager()->getPlayerSpriteIndex(PLAYER_RISING) && _sprite != Locator::getSpriteManager()->getPlayerSpriteIndex(PLAYER_FALLING) && _jumping)
					{
						changeToStaticSprite(Locator::getSpriteManager()->getPlayerSpriteIndex(PLAYER_DIVING));
					}
					else
					{
						if (_sprite != Locator::getSpriteManager()->getPlayerSpriteIndex(PLAYER_DIVING))
						{
							changeToStaticSprite(Locator::getSpriteManager()->getPlayerSpriteIndex(_velocity.y < 0 ? PLAYER_RISING : PLAYER_FALLING));
						}
					}
				}
				else
				{
					changeToStaticSprite(Locator::getSpriteManager()->getPlayerSpriteIndex(PLAYER_DEAD_AIR));
				}
			}
		}
	}

	if (_pinning)
	{
		setCollisionRectPosition(_pinnedEnemy->getCollisionRectPosition().x, _collisionRect.y);
	}
}

void Player::setVelX(float x)
{
	Entity::setVelX(x);
	if (_attach == Ceiling && x == 0 && _activeSequence != _attachToCeiling)
	{
		changeToStaticSprite(Locator::getSpriteManager()->getPlayerSpriteIndex(PLAYER_IDLE_CEILING));
	}
}

void Player::setVelY(float y)
{
	Entity::setVelY(y);
	if ((_attach == LeftSide || _attach == RightSide) && y == 0 && _activeSequence != _attachDownSequence && _activeSequence != _attachFromCeiling)
	{
		changeToStaticSprite(Locator::getSpriteManager()->getPlayerSpriteIndex(PLAYER_IDLE_SIDE));
	}
}

void Player::die()
{
	LivingEntity::setAlive(false);
	releasePin();

	if (_attach == NotAttached)
	{
		setCollisionRectDims(40, 18, ENTDIM);
	}

	detach();
	setAimingGun(false);
}

void Player::landOnGround()
{
	LivingEntity::landOnGround();
	_jumping = false;
}

void Player::jump(float x, float y)
{
	_onGround = false;
	_jumping = true;
	releasePin();
	detach();
	_velocity.x = x;
	_velocity.y = y;
	Locator::getAudio()->playSound("jump");
}

bool Player::isJumping()
{
	return _jumping;
}

bool Player::isPinning()
{
	return _pinning;
}

bool Player::isHacking()
{
	return _activeSequence == _hackingSequence;
}

unsigned int Player::getNumHackedTerminals()
{
	return _numHackedTerminals;
}

void Player::pinEnemy(Enemy* enemy)
{
	_velocity = vec2f(0, 0);
	detach();
	_numPunches = 0;
	_pinnedEnemy = enemy;
	enemy->changeState(PINNED);
	_pinning = true;
}

void Player::hackTerminal(MainComputer* computer)
{
	_hackedTerminal = computer;
	changeAnimationSequence(_hackingSequence);
}

void Player::setNumTerminalsHacked(unsigned int num)
{
	_numHackedTerminals = num;
}

void Player::punchPinnedEnemy()
{
	_numPunches++;
	_pinnedEnemy->changeAnimationSequence(Locator::getAnimationManager()->getSequence(ANIM_ENEMY_KNOCK_OUT));
}

void Player::releasePin()
{
	if (_pinnedEnemy)
	{
		_pinnedEnemy->changeState(KNOCKED_OUT);
	}
	_numPunches = 0;
	_pinnedEnemy = nullptr;
	_pinning = false;
}

Enemy* Player::getPinnedEnemy()
{
	return _pinnedEnemy;
}

unsigned int Player::getNumPunches()
{
	return _numPunches;
}

bool Player::isInElevator()
{
	return _inElevator;
}

void Player::enterElevator(ElevatorDoor* door)
{
	setAimingGun(false);
	_inElevator = true;
	_door = door;
	changeToStaticSprite(Locator::getSpriteManager()->getPlayerSpriteIndex(PLAYER_IN_ELEVATOR));
}

void Player::switchElevator(ElevatorDoor* door)
{
	_door = door;
}

void Player::leaveElevator()
{
	_inElevator = false;
	_door = nullptr;
}

ElevatorDoor* Player::getElevatorDoor()
{
	return _door;
}

AttachType Player::getAttachType()
{
	return _attach;
}

AttachType Player::getLastAttachType()
{
	return _lastAttach;
}

void Player::attachToVolume(CollisionVolume* volume, AttachType at)
{
	if (!_alive)
	{
		return;
	}

	_attachedVolume = volume;
	_lastAttach = _attach;
	_attach = at;
	_onGround = false;

	if (at == LeftSide)
	{
		_dir = Left;
	}

	if (at == RightSide)
	{
		_dir = Right;
	}

	if (at == LeftSide || at == RightSide)
	{
		_velocity.x = 0;
	}
}

CollisionVolume* Player::getAttachedVolume()
{
	return _attachedVolume;
}

void Player::detach()
{
	_lastAttach = _attach;
	_attach = NotAttached;
	_attachedVolume = nullptr;
}

void Player::setLightVisibility(unsigned int lightVisibility)
{
	_lightVisibility = lightVisibility;
}

unsigned int Player::getLightVisibility()
{
	return _lightVisibility;
}

bool Player::isAttachingDown()
{
	return _activeSequence == _attachDownSequence && !_currentAnimFinished;
}

bool Player::isGoingUpRoof()
{
	return _activeSequence == _reachRoofSequence && !_currentAnimFinished;
}

void Player::setStairMovement(StairTraversal st)
{
	if (isAnimatingThroughStairs() || isMovingThroughStairs())
	{
		return;
	}
	LivingEntity::setStairMovement(st);

	if (isMovingThroughStairs())
	{
		setCollisionRectPosition(_overlappingStairs->getCollisionRectPosition().x + 8, _overlappingStairs->getCollisionRectPosition().y);
		changeAnimationSequence(_enterStairs);
	}
}

void Player::arriveAtStairs(Stairs* st)
{
	LivingEntity::arriveAtStairs(st);
	Rect stairsRect = st->getCollisionRect();
	setCollisionRectPosition(st->getCollisionRectPosition().x + 8, stairsRect.y + stairsRect.h - _collisionRect.h - 1);
	changeAnimationSequence(_exitStairs);
}

bool Player::isAnimatingThroughStairs()
{
	return (_activeSequence == _exitStairs) || (_activeSequence == _enterStairs);
}

int Player::getArmRotation()
{
	return _armRotation;
}

void Player::setArmRotation(int rotation)
{
	_armRotation = rotation;
}

void Player::setAimingGun(bool b)
{
	if (!_onGround || !_alive || _inElevator || _pinning || isMovingThroughStairs() || isHacking())
	{
		_aimingGun = false;
		return;
	}

	if (_aimingGun && b)
	{
		return;
	}

	_aimingGun = b;

	if (_aimingGun)
	{
		Locator::getAudio()->playSound("pistol_ready");
	}
}

bool Player::isAimingGun()
{
	return _aimingGun;
}