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

#include "scene.h"

#define HANDSCANNER_DOOR_MIN_DISTANCE 64.0f
#define SWITCH_LIGHT_MIN_DISTANCE 512.0f
#define SWITCH_GUARD_HEIGHT_DIFF 12.0f
#define STAIRS_GUARD_HEIGHT_DIFF 128.0f

bool Scene::stairsReachableToEnemy(Enemy* enemy, Stairs* stairs)
{
	//TODO: this check may fail when it should not...
	if (stairs->getPosition().y == enemy->getPosition().y)
	{
		return !isTraceBlocked(nullptr, enemy->getCollisionRectPosition(), stairs->getCollisionRectPosition(), &_unused_c, &_unused_index, &_unused_bool, 64.0f, &Scene::interpStandard);
	}

	return false;
}

bool Scene::switchReachableToEnemy(Enemy* enemy, LightSwitch* ls)
{
	float diff = ls->getCollisionRectPosition().y - enemy->getCollisionRectPosition().y;
	bool door;
	if (SWITCH_GUARD_HEIGHT_DIFF >= diff && diff >= 0.0f)
	{
		bool blocked = isTraceBlocked(nullptr, enemy->getCollisionRectPosition(), ls->getCollisionRectPosition(), &_unused_c, &_unused_index, &door, 64.0f, &Scene::interpStandard);

		return blocked ? door : true;

	}
	else
	{
		return false;
	}
}

void Scene::handleEnemyPathfind(Enemy* enemy)
{
	float posX = enemy->getCollisionRectPosition().x;
	float posY = enemy->getCollisionRectPosition().y;
	float targX = enemy->getTarget().x;
	float targY = enemy->getTarget().y;
	bool door;
	int volIndex;

	Door* blockingDoor = nullptr;

	bool traceBlocked = isTraceBlocked(nullptr, enemy->getCollisionRectPosition(), enemy->getTarget(), &_unused_c, &_unused_index, &_unused_bool, 64.0f, &Scene::interpStandard);
	bool traceBlockedHoriz = isTraceBlocked(nullptr, enemy->getCollisionRectPosition(), vec2f(targX, posY), &_unused_c, &volIndex, &door, 64.0f, &Scene::interpStandard);

	if (traceBlockedHoriz)
	{
		if (door)
		{
			//try opening the blocking door.
			blockingDoor = getDoorOfCollisionVolume(_currentMap->getCollideVolPointerAt(volIndex));
			if (blockingDoor != nullptr)
			{
				if (enemy->getNumSwitchAttempts() < 3)
				{
					enemyTryOpenDoor(enemy, blockingDoor);
				}
				else
				{
					//Tried to open door, failed. Give up.
					enemy->changeState(IDLE_CAUTION);
					enemy->resetSwitchAttempts();
				}
				return;
			}
		}
	}
	else if (traceBlocked && !enemy->goingForStairs())
	{
		//direct path from enemy to target is blocked - try finding stairs.
		size_t i;
		Stairs* sw, *candidate;
		candidate = nullptr;
		float smallestDist = -1;
		float dist;
		for (i = 0; i < _currentMap->getNumberOfStairs(); i++)
		{
			sw = _currentMap->getStairsAt(i);
			//if a stair is vertically close to our target...
			if (fabs(sw->getCollisionRectPosition().y - targY) < STAIRS_GUARD_HEIGHT_DIFF)
			{
				//can the enemy find stairs to reach *this* one?
				Stairs* targStairs = (posY < targY) ? sw->getUpstairs() : sw->getDownstairs();
				if (targStairs != nullptr)
				{
					if (stairsReachableToEnemy(enemy, targStairs))
					{
						dist = fabs(targStairs->getCollisionRectPosition().x - posX);
						if (candidate == nullptr || dist < smallestDist)
						{
							candidate = targStairs;
							smallestDist = dist;
						}
					}
				}
			}
		}
		if (candidate != nullptr)
		{
			enemy->setDesiredStairsAndDirection(candidate, (posY < targY) ? MovingDown : MovingUp);
		}
	}
	else
	{
		if (!traceBlocked && fabs(targX - posX) < 16.0f && (enemy->getTargetType() != TARGET_LIGHTSWITCH && enemy->getTargetType() != TARGET_ALARM)
		        && !enemy->isAnimatingThroughStairs()) //needed to prevent interrupting stair animation.
		{
			//went to source of noise, or player's last seen position - return to normal soon.
			enemy->changeState(enemy->getType() == Enemy_Sniper ? AIMING : IDLE_CAUTION);
			if (enemy->getType() == Enemy_Sniper)
			{
				enemy->forgetTarget();
			}
			enemy->setSecondaryTarget(nullptr);
		}
	}
}

void Scene::handleEnemyLightOff(Enemy* enemy)
{
	//Need to try lightswitch 3 times - don't try again unless light comes on again.

	size_t i;
	LightSwitch* sw = nullptr;
	for (i = 0; i < _currentMap->getNumberOfEnts(); i++)
	{
		if (dynamic_cast<LightSwitch*>(_currentMap->getEntAt(i)) && !((LightSwitch*)_currentMap->getEntAt(i))->isHandScanner())
		{
			sw = static_cast<LightSwitch*>(_currentMap->getEntAt(i));

			if (vec2f_distance(enemy->getCollisionRectCenterPosition(), sw->getCollisionRectPosition()) < SWITCH_LIGHT_MIN_DISTANCE && switchReachableToEnemy(enemy, sw))
			{
				enemy->alertToPosition(sw->getCollisionRectPosition().x, sw->getCollisionRectPosition().y, ALERT_WALK, TARGET_LIGHTSWITCH);
				enemy->setLinkableTarget(sw);
				return;
			}
		}
	}
}

void Scene::enemyTryOpenDoor(Enemy* enemy, Door* door)
{
	size_t i;
	bool leftToRight = door->getCollisionRectPosition().x > enemy->getCollisionRectPosition().x;
	bool between = false;
	LightSwitch* sw = nullptr;
	float px;
	for (i = 0; i < _currentMap->getNumberOfEnts(); i++)
	{
		if (dynamic_cast<LightSwitch*>(_currentMap->getEntAt(i)) && static_cast<LightSwitch*>(_currentMap->getEntAt(i))->isHandScanner())
		{
			sw = static_cast<LightSwitch*>(_currentMap->getEntAt(i));
			px = sw->getCollisionRectPosition().x;
			between = (leftToRight && door->getCollisionRectPosition().x > px && px > enemy->getCollisionRectPosition().x) ||
			          (!leftToRight && enemy->getCollisionRectPosition().x > px && px > door->getCollisionRectPosition().x);

			if (vec2f_distance(door->getCollisionRectPosition(), sw->getCollisionRectPosition()) < HANDSCANNER_DOOR_MIN_DISTANCE && between && switchReachableToEnemy(enemy, sw))
			{
				enemy->setSecondaryTarget(sw);
				return;
			}
		}
	}
}

void Scene::traceEnemyFOV(Enemy* enemy)
{
	if (enemy->getType() == Enemy_Sniper && enemy->getState() != AIMING)
	{
		return;
	}

	vec2f position = enemy->getCollisionRectCenterPosition();
	vec2f v1 = vec2f(enemy->getDirection() == Right ? 1 : -1, 0);
	vec2f v2;
	vec2f playerPos = _player->getCollisionRectCenterPosition();
	float dist = vec2f_distance(position, playerPos);
	//Professionals and snipers can see fine in the dark.
	int firstRadius;
	int trueRadius;
	float testAngle;
	float angle;
	if (enemy->getType() == Enemy_Sniper)
	{
		firstRadius = trueRadius = ENEMY_FOV_RADIUS_SNIPER;
		testAngle = ENEMY_FOV_HALFANGLE_SEEN;
	}
	else
	{
		firstRadius = ENEMY_FOV_RADIUS_LIT;
		trueRadius = (enemy->getType() == Enemy_Professional) ? ENEMY_FOV_RADIUS_LIT : (isPlayerInLight() ? ENEMY_FOV_RADIUS_LIT : ENEMY_FOV_RADIUS_DARK);
		testAngle = enemy->canSeePlayer() ? ENEMY_FOV_HALFANGLE_SEEN : ENEMY_FOV_HALFANGLE;
	}

	if (dist < firstRadius)
	{
		v2 = vec2f_normalize(vec2f(playerPos.x - position.x, playerPos.y - position.y));
		angle = acos(v1.dot(v2));

		if (	angle < ToRadian(testAngle) &&
		        !isTraceBlocked(enemy, position, playerPos, &_unused_c, &_unused_index, &_unused_bool, 16.0f, &Scene::traceGuardSight) &&
		        !_player->isInElevator() && !_player->isMovingThroughStairs())
		{
			enemy->setReadyToShoot(true);
			if (dist < trueRadius)
			{
				enemy->seePlayer(playerPos.x, playerPos.y);
			}
		}
		else
		{
			if (enemy->canSeePlayer())
			{
				if (!_player->isMovingThroughStairs())
				{
					enemy->loseSightOfPlayer(_player->isInElevator(), enemy->getTarget());
				}
				else
				{
					Stairs* entered = _player->getStairsEntered(); //the stairs the player just entered out of enemy's sight.
					Stairs* targ = _player->getStairTraversal() == MovingUp ? entered->getUpstairs() : entered->getDownstairs();
					enemy->loseSightOfPlayer(false, targ->getCollisionRectPosition());
				}
			}
		}
	}
	else
	{
		if (enemy->canSeePlayer())
		{
			enemy->loseSightOfPlayer(_player->isInElevator(), enemy->getTarget());
		}
	}
}