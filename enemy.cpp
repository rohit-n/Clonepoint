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

#define GUNPOINT_MIN_MOVE_DISTANCE_SQUARED 15000
#define GUNPOINT_MAX_MOVE_DISTANCE_SQUARED 17000
#define PATROLSPEED 0.06
#define INVESTIGATESPEED 0.25
#define TIMETOPATROL 4000 //Time for enemy to switch from IDLE_CAUTION to PATROLLING.
#define TIMETOTURN 500
#define GUARDREACTIONTIME 500
#define PROFESSIONALREACTIONTIME 250
#define TIME_TO_RESET_REACTION_TIME 20000
#define ENEMYALERTTIME 500
#define GUARD_SWITCH_TIME 500
#define GUARD_DISTANCE_TO_USE_SWITCH 16.0f
#define ENEMY_TIME_GUN_UP 1000

Enemy::Enemy(float x, float y, Direction startingDir, bool startPatrol, EnemyType type) : LivingEntity(x, y, startingDir)
{
	setCollisionRectDims(16, 40, ENTDIM);
	_waitingForAlert = false;
	_type = type;
	_gun = NULL;
	if (startPatrol)
	{
		_state = PATROLLING;
		_velocity.x = (startingDir == Right) ? PATROLSPEED : -PATROLSPEED;
		changeAnimationSequence(Locator::getAnimationManager()->getSequence(ANIM_ENEMY_PATROLLING));
	}
	else
	{
		_state = IDLE;
		changeToStaticSprite(Locator::getSpriteManager()->getGuardSpriteIndex(GUARD_IDLE_GROUND));
	}
	_playerInSight = false;
	_hasSeenPlayer = false;
	_target = vec2f(0, 0);
	_shootTarget = vec2f(0, 0);
	_desiredStairDirection = NotMoving;
	_timeToPatrol = TIMETOPATROL;
	_timeToTurn1 = TIMETOTURN;
	_timeToTurn2 = TIMETOTURN;
	_timeToHitSwitch = GUARD_SWITCH_TIME;
	setResolve(0, Shot_None);
	_secondaryTarget = NULL;
	_numSwitchAttempts = 0;
	_lightToActivate = NULL;
	_strongestLight = NULL;
	_targetSwitch = NULL;
	_targetType = TARGET_NONE;
	_alertType = ALERT_NONE;
	_fullyPunched = false;
	_ignoreFall = true;
	_readyToShoot = false;
	_heldAtGunpoint = false;
	_scene_trace_bullet = false;
	_timeToAlert = ENEMYALERTTIME;
	_timeToResetReactionTime = TIME_TO_RESET_REACTION_TIME;

	resetReactionTime();

	if (_type == Enemy_Sniper)
	{
		_readyToShoot = true;
		_state = IDLE;
		changeToStaticSprite(Locator::getSpriteManager()->getGuardSpriteIndex(GUARD_IDLE_GROUND));
	}
}

void Enemy::setPosition(float x, float y)
{
	Entity::setPosition(x, y);
	_ignoreFall = true;
}

void Enemy::landOnGround()
{
	LivingEntity::landOnGround();
	_ignoreFall = false;
	if (_state == FALLING)
	{
		_velocity.x = 0;
		changeToStaticSprite(Locator::getSpriteManager()->getGuardSpriteIndex(GUARD_FALLEN));
	}
}

void Enemy::update(unsigned int dT)
{
	_fixDirection = _heldAtGunpoint || (_state == PINNED);
	switch (_state)
	{
	case AIMING:
		if (_reactionTime <= 0 && !_heldAtGunpoint && _playerInSight)
		{
			_gun->fire(Shot_FromEnemyVoluntary);
			if (_reactionTime <= 0)
			{
				resetReactionTime();
			}
			else
			{
				_gun->unlink();
			}
		}
	case PATROLLING:
	case INVESTIGATING:
		if (_secondaryTarget != NULL && _overlappingStairs != NULL && _overlappingStairs == _secondaryTarget)
		{
			//time to move through stairs.
			setStairMovement(_desiredStairDirection);
		}
		//If enemy is close enough to their target switch...
		else if (_secondaryTarget != NULL && dynamic_cast<LightSwitch*>(_secondaryTarget) && vec2f_distance(getCollisionRectPosition(), _secondaryTarget->getCollisionRectPosition()) < GUARD_DISTANCE_TO_USE_SWITCH)
		{
			changeState(USING_SWITCH);
		}
		else if (_targetSwitch != NULL && (_targetType == TARGET_LIGHTSWITCH || _targetType == TARGET_ALARM) && vec2f_distance(getCollisionRectPosition(), _target) < GUARD_DISTANCE_TO_USE_SWITCH)
		{
			changeState(USING_SWITCH);
		}
	case IDLE:
		LivingEntity::update(dT);
		updateCollisionRectPosition();
		break;
	case IDLE_CAUTION:
		LivingEntity::update(dT);
		updateCollisionRectPosition();

		_timeToTurn1 -= dT;

		if (_timeToTurn1 <= 0 && _timeToTurn2 == TIMETOTURN)
		{
			reverseDirection();
			_timeToTurn2 -= dT;
		}
		else if (_timeToTurn1 <= 0 && _timeToTurn2 > 0)
		{
			_timeToTurn2 -= dT;
		}
		else if (_timeToTurn1 <= 0 && _timeToTurn2 <= 0 && _timeToPatrol == TIMETOPATROL)
		{
			reverseDirection();
			_timeToPatrol -= dT;
		}

		if (_timeToPatrol < TIMETOPATROL)
		{
			_timeToPatrol -= dT;
		}

		if (_timeToPatrol <= 0)
		{
			_timeToPatrol = TIMETOPATROL;
			_timeToTurn1 = TIMETOTURN;
			_timeToTurn2 = TIMETOTURN;
			changeState(PATROLLING);
		}

		break;
	case USING_SWITCH:
		LivingEntity::update(dT);
		updateCollisionRectPosition();
		_timeToHitSwitch -= dT;
		if (_timeToHitSwitch <= 0)
		{
			_timeToHitSwitch = GUARD_SWITCH_TIME;
			if (_secondaryTarget != NULL && dynamic_cast<LightSwitch*>(_secondaryTarget))
			{
				if (_targetType == TARGET_LIGHTSWITCH)
				{
					changeState(PATROLLING);
				}
				else if (_targetType != TARGET_NONE)
				{
					changeState(INVESTIGATING);
				}

				((LightSwitch*)_secondaryTarget)->activate();
				if (_targetSwitch == _secondaryTarget)
				{
					_targetSwitch = NULL;
				}
				_secondaryTarget = NULL;
				_numSwitchAttempts++;
			}
			else if (_targetSwitch != NULL)
			{
				if (_targetType == TARGET_LIGHTSWITCH)
				{
					_targetSwitch->activate();
					_numSwitchAttempts++;
					if ((_lightToActivate != NULL && _lightToActivate->isActive()) || _numSwitchAttempts == 3) //activate the light, or give up.
					{
						_targetSwitch = NULL;
						_targetType = TARGET_NONE;
						changeState(PATROLLING);
						_lightToActivate = NULL;
						_numSwitchAttempts = 0;
					}
				}

				if (_targetType == TARGET_ALARM)
				{
					(static_cast<Alarm*>(_targetSwitch))->deactivate();
					changeState(IDLE_CAUTION);
				}
			}
		}
		break;
	case PINNED:
	case KNOCKED_OUT:
	case FALLING:
		LivingEntity::update(dT);
		updateCollisionRectPosition();
		break;
	}

	if (_playerInSight)
	{
		_shootTarget = _target;
		if (_heldAtGunpoint)
		{
			vec2f sub = getCollisionRectCenterPosition() - _shootTarget;
			float len = sub.length_squared();
			if (len < GUNPOINT_MIN_MOVE_DISTANCE_SQUARED)
			{
				changeAnimationSequence(Locator::getAnimationManager()->getSequence(ANIM_ENEMY_PATROLLING));
				_velocity.x = vec2f_normalize(sub).x / 48.0f;
			}
			else if (len > GUNPOINT_MAX_MOVE_DISTANCE_SQUARED)
			{
				changeToStaticSprite(Locator::getSpriteManager()->getGuardSpriteIndex(GUARD_IDLE_CAUTION));
				_velocity.x = 0;
			}
		}
	}
	else
	{
		_shootTarget.x = getCollisionRectCenterPosition().x + (_dir == Right ? 64 : -64);
		_shootTarget.y = getCollisionRectCenterPosition().y;
	}

	if (_readyToShoot)
	{
		_reactionTime -= dT;
		_timeToResetReactionTime -= dT;
		if (_reactionTime <= 0)
		{
			_reactionTime = 0;
		}

		if (_timeToResetReactionTime <= 0)
		{
			setReadyToShoot(false);
		}
	}

	if (_resolve.timeSinceShot > 0)
	{
		_resolve.timeSinceShot += dT;
		if (_resolve.timeSinceShot < ENEMY_TIME_GUN_UP)
		{
			changeToStaticSprite(Locator::getSpriteManager()->getGuardSpriteIndex(GUARD_IDLE_CAUTION));
		}
		else
		{
			if (_resolve.shotType == Shot_FromEnemyVoluntary)
			{
				//if player is dead, this will remain.
				changeToStaticSprite(Locator::getSpriteManager()->getGuardSpriteIndex(GUARD_IDLE_GROUND));
			}

			if (_resolve.shotType == Shot_FromEnemyInvoluntary)
			{
				//why did my gun fire by itself?
				changeState(IDLE_CAUTION);
			}
			setResolve(0, Shot_None);
		}
	}

	if (_waitingForAlert)
	{
		_timeToAlert -= dT;
		if (_timeToAlert <= 0)
		{
			_waitingForAlert = false;
			_timeToAlert = ENEMYALERTTIME;
			if (isPositionBehind(_target.x) && _desiredStairDirection == NotMoving)
			{
				reverseDirection();
				setVelX(-_velocity.x);
			}
			switch(_alertType)
			{
			case ALERT_WALK:
				changeState(PATROLLING);
				break;
			case ALERT_RUN:
				changeState(INVESTIGATING);
				break;
			default:
				break;
			}
		}
	}

	if (_gun != NULL)
		_gun->update(dT);

	int r;
	if (_activeSequence == Locator::getAnimationManager()->getSequence(ANIM_ENEMY_KNOCK_OUT))
	{
		if (_currentAnimFinished)
		{
			changeToStaticSprite(Locator::getSpriteManager()->getGuardSpriteIndex(GUARD_FALLEN_PLAYER));
			_fullyPunched = false;
		}
		else if (_sprite == Locator::getSpriteManager()->getGuardSpriteIndex(GUARD_PUNCHED) && !_fullyPunched)
		{
			r = ceil(((double)rand() / (double)(RAND_MAX)) * 10);
			if (r > 5)
			{
				Locator::getAudio()->playSound("punch1");
			}
			else
			{
				Locator::getAudio()->playSound("punch2");
			}

			_fullyPunched = true;
		}
	}

	if (_activeSequence == Locator::getAnimationManager()->getSequence(ANIM_ENEMY_ENTER_STAIRS) && _currentAnimFinished)
	{
		changeToStaticSprite(7);
	}

	if (_activeSequence == Locator::getAnimationManager()->getSequence(ANIM_ENEMY_EXIT_STAIRS) && _currentAnimFinished)
	{
		_secondaryTarget = NULL;
		_desiredStairDirection = NotMoving;
		setSprite();
		if (_state == PATROLLING)
		{
			_velocity.x = (_dir == Right) ? PATROLSPEED : -PATROLSPEED;
		}
		if (_state == INVESTIGATING)
		{
			_velocity.x = (_dir == Right) ? INVESTIGATESPEED : -INVESTIGATESPEED;
			if (isPositionBehind(_target.x))
			{
				setVelX(-_velocity.x);
			}
		}
	}
}

void Enemy::seePlayer(float x, float y)
{
	_target = vec2f(x, y);
	_playerInSight = true;
	_hasSeenPlayer = true;

	if (_state != AIMING)
		changeState(AIMING);
}

void Enemy::alertToPosition(float x, float y, AlertType aType, TargetType tType)
{
	if (_state == FALLING || _state == KNOCKED_OUT || _state == PINNED)
	{
		return;
	}

	if (aType == ALERT_LOOK)
	{
		if (_state == IDLE && isPositionBehind(x))
		{
			reverseDirection();
		}
		return;
	}

	if (!_waitingForAlert)
	{
		_waitingForAlert = true;
		if (tType == TARGET_PLAYER)
		{
			_timeToAlert = 1;
		}
	}

	_target.x = x;
	_target.y = y;
	_targetType = tType;
	_alertType = aType;
	//if target requires running (like noise), turn immediately. Otherwise, turn
	//only when alert time runs out (like when light goes out and switch is behind.)
	if (aType == ALERT_RUN && _desiredStairDirection == NotMoving && isPositionBehind(x))
	{
		reverseDirection();
		setVelX(-_velocity.x);
	}
}

bool Enemy::hasSeenPlayer()
{
	return _hasSeenPlayer;
}

void Enemy::loseSightOfPlayer(bool wentInElevator, vec2f newTarget)
{
	_playerInSight = false;
	if (_type == Enemy_Sniper)
	{
		return;
	}
	_velocity.x = 0; //done in case guard is maintaining distance from aiming player
	if (!wentInElevator) //otherwise, wait to strike...
	{
		alertToPosition(newTarget.x, newTarget.y, ALERT_RUN, TARGET_PLAYER);
	}
	else
	{
		changeToStaticSprite(Locator::getSpriteManager()->getGuardSpriteIndex(GUARD_IDLE_CAUTION));
	}
}

bool Enemy::canSeePlayer()
{
	return _playerInSight;
}

vec2f Enemy::getTarget()
{
	return _target;
}

TargetType Enemy::getTargetType()
{
	return _targetType;
}

void Enemy::forgetTarget()
{
	_targetType = TARGET_NONE;
	_alertType = ALERT_NONE;
}

GuardState Enemy::getState()
{
	return _state;
}

AlertType Enemy::getAlertType()
{
	return _alertType;
}

void Enemy::changeState(GuardState state)
{
	if (_state == state)
		return;

	if ((_state == KNOCKED_OUT || _state == FALLING) && !(state == KNOCKED_OUT || state == FALLING))
	{
		setCollisionRectDims(16, 40, ENTDIM);
	}

	if (_state == IDLE_CAUTION)
	{
		//reset some variables for when enemy returns to IDLE_CAUTION.
		_timeToPatrol = TIMETOPATROL;
		_timeToTurn1 = TIMETOTURN;
		_timeToTurn2 = TIMETOTURN;
	}

	_state = state;

	switch (state)
	{
	case IDLE:
	case USING_SWITCH:
		_velocity.x = 0;
		break;
	case IDLE_CAUTION:
		if (!_acceleration.accelerating)
		{
			_velocity.x = 0;
		}
		forgetTarget();
		break;
	case PATROLLING:
		_velocity.x = (_dir == Right) ? PATROLSPEED : -PATROLSPEED;
		break;
	case INVESTIGATING:
		_velocity.x = (_dir == Right) ? INVESTIGATESPEED : -INVESTIGATESPEED;
		break;
	case AIMING:
		_velocity.x = 0;
		break;
	case PINNED:
		setCollisionRectDims(40, 16, ENTDIM);
		setCollisionRectPosition(_collisionRect.x, _collisionRect.y + 12);
		_targetType = TARGET_NONE;
		_alertType = ALERT_LOOK;
		_waitingForAlert = false;
		break;
	case KNOCKED_OUT:
	case FALLING:
		_velocity.x = 0.0f;
		setCollisionRectDims(40, 16, ENTDIM);
		_playerInSight = false;
		_onGround = false;
		_targetType = TARGET_NONE;
		_alertType = ALERT_LOOK;
		break;
	}

	setSprite();
}

void Enemy::setSprite()
{
	switch (_state)
	{
	case IDLE:
		changeToStaticSprite(Locator::getSpriteManager()->getGuardSpriteIndex(GUARD_IDLE_GROUND));
		break;
	case IDLE_CAUTION:
		changeToStaticSprite(Locator::getSpriteManager()->getGuardSpriteIndex(GUARD_IDLE_CAUTION));
		break;
	case PATROLLING:
	case INVESTIGATING:
		changeAnimationSequence(Locator::getAnimationManager()->getSequence(ANIM_ENEMY_PATROLLING));
		break;
	case AIMING:
		changeToStaticSprite(Locator::getSpriteManager()->getGuardSpriteIndex(GUARD_IDLE_CAUTION));
		break;
	case PINNED:
		changeAnimationSequence(Locator::getAnimationManager()->getSequence(ANIM_ENEMY_PLAYER_STRUGGLE));
		break;
	case KNOCKED_OUT:
		changeToStaticSprite(Locator::getSpriteManager()->getGuardSpriteIndex(GUARD_FALLEN));
		break;
	case FALLING:
		changeToStaticSprite(Locator::getSpriteManager()->getGuardSpriteIndex(GUARD_FALLING));
		break;
	case USING_SWITCH:
		break;
	}
}

bool Enemy::goingForStairs()
{
	return _secondaryTarget != NULL && dynamic_cast<Stairs*>(_secondaryTarget);
}

void Enemy::setDesiredStairsAndDirection(Stairs* sw, StairTraversal st)
{
	if (isPositionBehind(sw->getCollisionRect().x))
	{
		reverseDirection();
		setVelX(-_velocity.x);
	}
	setSecondaryTarget(sw);
	_desiredStairDirection = st;
}

void Enemy::setSecondaryTarget(Entity* ent)
{
	if (ent != NULL)
	{
		if (isPositionBehind(ent->getCollisionRectPosition().x))
		{
			setVelX(-_velocity.x);
		}
	}
	_secondaryTarget = ent;
}

Entity* Enemy::getSecondaryTarget()
{
	return _secondaryTarget;
}

unsigned int Enemy::getNumSwitchAttempts()
{
	return _numSwitchAttempts;
}

void Enemy::resetSwitchAttempts()
{
	_numSwitchAttempts = 0;
}

void Enemy::setStrongestLight(FieldOfView* light)
{
	if (_lightToActivate != NULL && light != NULL && _state == IDLE)
	{
		changeState(PATROLLING);
	}

	if ((light == _lightToActivate && light != NULL) || (light != NULL && _lightToActivate != NULL && light->getLightFixture() == _lightToActivate->getLightFixture()))
	{
		//Nothing to activate, since it's now on.
		_lightToActivate = NULL;
		_targetType = TARGET_NONE;
		//_alertType = ALERT_LOOK;
	}
	_strongestLight = light;
}

void Enemy::setLightToActivate(FieldOfView* toActivate)
{
	if (_state == FALLING || _state == KNOCKED_OUT)
	{
		return;
	}
	_lightToActivate = toActivate;
}

void Enemy::loseStrongestLight()
{
	_lightToActivate = _strongestLight;
}

FieldOfView* Enemy::getStrongestLight()
{
	return _strongestLight;
}

FieldOfView* Enemy::getLightToActivate()
{
	return _lightToActivate;
}

void Enemy::setLinkableTarget(LinkableEntity* ls)
{
	if (_state == FALLING || _state == KNOCKED_OUT)
	{
		return;
	}
	_targetSwitch = ls;
}

bool Enemy::ignoringFall()
{
	return _type == Enemy_Sniper ? true : _ignoreFall;
}

void Enemy::setIgnoreFall(bool b)
{
	_ignoreFall = b;
}

LinkableEntity* Enemy::getTargetSwitch()
{
	return _targetSwitch;
}

void Enemy::setReactionTime(int time)
{
	_reactionTime = time;
}

int Enemy::getReactionTime()
{
	return _reactionTime;
}

void Enemy::setReadyToShoot(bool b)
{
	if (_readyToShoot && b)
	{
		return;
	}

	_readyToShoot = b;

	if (_readyToShoot)
	{
		Locator::getAudio()->playSound("pistol_ready");
	}
	else
	{
		if (_type != Enemy_Sniper)
		{
			_timeToResetReactionTime = TIME_TO_RESET_REACTION_TIME;
			resetReactionTime();
		}
		else
		{
			_timeToResetReactionTime = 0;
			_readyToShoot = true;
		}
	}
}

bool Enemy::isWaitingForAlert()
{
	return _waitingForAlert;
}

void Enemy::setWaitingForAlert(bool b)
{
	_waitingForAlert = b;
}

EnemyType Enemy::getType()
{
	return _type;
}

void Enemy::resetReactionTime()
{
	switch (_type)
	{
	case Enemy_Guard:
	case Enemy_Enforcer:
		_reactionTime = GUARDREACTIONTIME;
		break;
	case Enemy_Professional:
		_reactionTime = PROFESSIONALREACTIONTIME;
		break;
	case Enemy_Sniper:
		_reactionTime = 0;
		break;
	}
}

void Enemy::setHeldAtGunpoint(bool b)
{
	if (_heldAtGunpoint && !b)
	{
		_reactionTime = 500;
	}
	_heldAtGunpoint = b;
	if (_type == Enemy_Professional)
	{
		_heldAtGunpoint = false;
	}
}

bool Enemy::isHeldAtGunpoint()
{
	return _heldAtGunpoint;
}

void Enemy::setCanSeePlayer(bool b)
{
	_playerInSight = b;
}

void Enemy::_fireWeapon(GunShotTraceType gstt)
{
	_scene_trace_bullet = true;
	setResolve(1, gstt);
}

void Enemy::setGun(EnemyGun* gun)
{
	_gun = gun;
}

EnemyGun* Enemy::getGun()
{
	return _gun;
}

void Enemy::setResolve(int timeToReact, GunShotTraceType gstt)
{
	_resolve.timeSinceShot = timeToReact;
	_resolve.shotType = gstt;
	if (gstt != Shot_None)
	{
		_velocity.x = 0;
	}
}

EnemyShotResolve Enemy::getResolve()
{
	return _resolve;
}

void Enemy::arriveAtStairs(Stairs* st)
{
	LivingEntity::arriveAtStairs(st);
	Rect stairsRect = st->getCollisionRect();
	setCollisionRectPosition(stairsRect.x + 8, stairsRect.y + stairsRect.h - _collisionRect.h - 1);
	changeAnimationSequence(Locator::getAnimationManager()->getSequence(ANIM_ENEMY_EXIT_STAIRS));
}

void Enemy::setStairMovement(StairTraversal st)
{
	if (isAnimatingThroughStairs() || isMovingThroughStairs())
	{
		return;
	}
	LivingEntity::setStairMovement(st);
	_waitingForAlert = false;

	if (isMovingThroughStairs())
	{
		Rect stairsRect = _overlappingStairs->getCollisionRect();
		setCollisionRectPosition(stairsRect.x + 8, stairsRect.y + stairsRect.h - _collisionRect.h - 1);
		changeAnimationSequence(Locator::getAnimationManager()->getSequence(ANIM_ENEMY_ENTER_STAIRS));
	}
}

bool Enemy::isAnimatingThroughStairs()
{
	return (_activeSequence == Locator::getAnimationManager()->getSequence(ANIM_ENEMY_ENTER_STAIRS)) ||
	       (_activeSequence == Locator::getAnimationManager()->getSequence(ANIM_ENEMY_EXIT_STAIRS));
}