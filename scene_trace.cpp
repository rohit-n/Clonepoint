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

#define GUNPOINT_MIN_DISTANCE_SQUARED 2000

float lerp(float x, float x0, float x1, float y0, float y1)
{
	return ((x - x0) * (y1 - y0) / (x1 - x0)) + y0;
}

bool Scene::isTraceBlocked(Entity* source, vec2f a, vec2f b, vec2f* c, int* volIndex, bool* isDoor, float vertEpsilon, interpFunc func)
{
	//assume trace is not blocked at beginning.
	c->x = b.x;
	c->y = b.y;
	*volIndex = -1;
	*isDoor = false;

	bool leftToRight = (b.x >= a.x);
	bool goingDown = (b.y >= a.y);

	if (fabs(a.x - b.x) < vertEpsilon) //if this trace is pretty much vertical...
	{
		return traceInDirection(source, a, b, c, goingDown, TDVertical, volIndex, isDoor, func);
	}
	else
	{
		return traceInDirection(source, a, b, c, leftToRight, TDHorizontal, volIndex, isDoor, func);
	}
}

bool Scene::traceInDirection(Entity* source, vec2f a, vec2f b, vec2f* end, bool downOrRight, TraceDirection direction, int* volIndex, bool* isDoor, interpFunc func)
{
	int j;
	int incr = downOrRight ? 1 : -1;
	float endOppositeCheck;
	float target;
	int start;
	vec2f interpolatedPoint, oldPoint;
	oldPoint.x = a.x;
	oldPoint.y = a.y;
	bool stop = false;

	if (direction == TDHorizontal)
	{
		start = a.x;
		target = downOrRight ? b.x : fabs(b.x - 2 * a.x);
		endOppositeCheck = b.x;
	}
	else
	{
		start = a.y;
		target = downOrRight ? b.y : fabs(b.y - 2 * a.y);
		endOppositeCheck = b.y;
	}

	if (func != &Scene::interpStandard && func != &Scene::traceGuardSight) //don't stop at b - go to map boundary.
	{
		endOppositeCheck = 0;
		target = (direction == TDHorizontal) ? _currentMap->getMapWidth() : _currentMap->getMapHeight();
	}

	for (j = start; j < target; j += incr) //scan from left to right, or right to left.
	{
		if ((!downOrRight && j <= endOppositeCheck) || stop) //stupid hack to avoid copying this for loop.
		{
			break;
		}

		interpolatedPoint = (direction == TDHorizontal) ? vec2f(j, lerp(j, a.x, b.x, a.y, b.y)) : vec2f(lerp(j, a.y, b.y, a.x, b.x), j);
		(this->*func)(source, interpolatedPoint, func == &Scene::interpStandard ? oldPoint : a, volIndex, isDoor, &stop, end);
		if (stop)
		{
			return true;
		}
		oldPoint = interpolatedPoint;
	}

	return false;
}

void Scene::traceBullet(LivingEntity* entity, vec2f target, GunShotTraceType gstt, bool fired)
{
	vec2f position = entity->getCollisionRectCenterPosition();

	bool madeNoise = false;

	if (fired)
	{
		if (gstt == Shot_FromEnemyVoluntary)
		{
			isTraceBlocked(nullptr, position, target, &_unused_c, &_unused_index, &_unused_bool, 16.0f, &Scene::bulletFiredFromEnemy);
		}
		else
		{
			if (gstt == Shot_FromPlayer)
			{
				if (_numPlayerBullets == 0)
				{
					return;
				}
				_numPlayerBullets--;
				madeNoise = true;
				_playerShotFired = true;
				isTraceBlocked(nullptr, position, target, &_unused_c, &_unused_index, &_unused_bool, 16.0f, &Scene::bulletFiredFromPlayer);
			}
			else
			{
				isTraceBlocked(entity, position, target, &_unused_c, &_unused_index, &_unused_bool, 64.0f, &Scene::bulletFiredFromEnemyInvoluntary);
			}
		}
		Locator::getAudio()->playSound("pistol");
	}
	else
	{
		isTraceBlocked(nullptr, position, target, &_unused_c, &_unused_index, &_unused_bool, 16.0f, &Scene::traceLaserSight);
	}

	if (!_player->isAlive() && dynamic_cast<Enemy*>(entity))
	{
		_loadMenuVisible = true;
		static_cast<Enemy*>(entity)->changeState(IDLE);
	}

	if (madeNoise) //make noise last, in case an enemy is killed and another may be alerted in its place
		addNoise(position.x - _camera.x, position.y - _camera.y, 512, true, ALERT_RUN, nullptr);
}

void Scene::bulletFiredFromEnemy(Entity* source, vec2f interpolatedPoint, vec2f start, int* index, bool* b1, bool* stop, vec2f* end)
{
	breakOnGlass(interpolatedPoint, start, stop);
	if (*stop)
	{
		return;
	}

	if (vec2InRect(interpolatedPoint, _player->getCollisionRect()) && _player->isAlive())
	{
		_player->die();
		_stopTrajPoint = 0;
		handlePlayerShotWhileAttached();
	}
}

void Scene::bulletFiredFromPlayer(Entity* source, vec2f interpolatedPoint, vec2f start, int* index, bool* b1, bool* stop, vec2f* end)
{
	breakOnGlass(interpolatedPoint, start, stop);
	if (*stop)
	{
		return;
	}

	killEnemies(source, interpolatedPoint, stop);
}

//gun fired by guard involuntarily from activation can kill other guards or player.
void Scene::bulletFiredFromEnemyInvoluntary(Entity* source, vec2f interpolatedPoint, vec2f start, int* index, bool* b1, bool* stop, vec2f* end)
{
	breakOnGlass(interpolatedPoint, start, stop);
	if (*stop)
	{
		return;
	}

	killEnemies(source, interpolatedPoint, stop);

	if (vec2InRect(interpolatedPoint, _player->getCollisionRect()) && _player->isAlive())
	{
		_player->die();
		_stopTrajPoint = 0;
		handlePlayerShotWhileAttached();
	}
}

void Scene::traceLaserSight(Entity* source, vec2f interpolatedPoint, vec2f start, int* index, bool* b1, bool* stop, vec2f* end)
{
	size_t k;
	CollisionVolume* vol;
	bool leftToRight = interpolatedPoint.x > start.x;
	for (k = 0; k < _numCollideVols; k++)
	{
		vol = _currentMap->getCollideVolPointerAt(k);
		if (vec2InRect(interpolatedPoint, vol->rect) && vol->active)
		{
			if (!vol->glass)
			{
				*stop = true;
				break;
			}
		}
	}

	for (k = 0; k < _currentMap->getNumberOfEnemies(); k++)
	{
		Enemy* enemy = _currentMap->getEnemyAt(k);
		if (enemy->isAlive())
		{
			if (vec2InRect(interpolatedPoint, enemy->getCollisionRect()))
			{
				bool hold = leftToRight xor (enemy->getDirection() == Right);
				hold = hold && (start - interpolatedPoint).length_squared() >= GUNPOINT_MIN_DISTANCE_SQUARED;
				enemy->setHeldAtGunpoint(hold);
				*stop = true;
				break;
			}
			else
			{
				if (enemy->isHeldAtGunpoint())
				{
					enemy->setHeldAtGunpoint(false);
				}
			}
		}
	}
	_laserEnd = interpolatedPoint;
}

void Scene::traceGuardSight(Entity* source, vec2f interpolatedPoint, vec2f start, int* index, bool* b1, bool* stop, vec2f* end)
{
	size_t k;
	*stop = false;
	vec2f oldPoint = start;
	for (k = 0; k < _numCollideVols; k++)
	{
		//if the trace hits an active collision volume that is NOT glass, abandon check.
		if (vec2InRect(interpolatedPoint, _currentMap->getCollideVolAt(k).rect) && _currentMap->getCollideVolAt(k).active && !_currentMap->getCollideVolAt(k).glass)
		{
			*index = k;
			if (_currentMap->getCollideVolAt(k).door)
			{
				*b1 = true;
			}
			handleTraceHit(&oldPoint, &interpolatedPoint, end, _currentMap->getCollideVolPointerAt(k));
			*stop = true;
			return;
		}
	}

	if (handleTraceHitMapBounds(&interpolatedPoint, end))
	{
		*stop = true;
	}

	for (k = 0; k < _currentMap->getNumberOfEnemies(); k++)
	{
		Enemy* enemy = _currentMap->getEnemyAt(k);
		if (enemy != source && enemy->isAlive())
		{
			if (vec2InRect(interpolatedPoint, enemy->getCollisionRect()))
			{
				*stop = true;
				return;
			}
		}
	}
}

void Scene::interpStandard(Entity* source, vec2f interpolatedPoint, vec2f start, int* index, bool* b1, bool* stop, vec2f* end)
{
	size_t k;
	*stop = false;
	vec2f oldPoint = start;
	for (k = 0; k < _numCollideVols; k++)
	{
		//if the trace hits an active collision volume that is NOT glass, abandon check.
		if (vec2InRect(interpolatedPoint, _currentMap->getCollideVolAt(k).rect) && _currentMap->getCollideVolAt(k).active && !_currentMap->getCollideVolAt(k).glass)
		{
			*index = k;
			if (_currentMap->getCollideVolAt(k).door)
			{
				*b1 = true;
			}
			handleTraceHit(&oldPoint, &interpolatedPoint, end, _currentMap->getCollideVolPointerAt(k));
			*stop = true;
			return;
		}
	}

	if (handleTraceHitMapBounds(&interpolatedPoint, end))
	{
		*stop = true;
	}
}

void Scene::breakOnGlass(vec2f interpolatedPoint, vec2f start, bool* stop)
{
	size_t k;
	CollisionVolume* vol;
	bool leftToRight = interpolatedPoint.x > start.x;
	for (k = 0; k < _numCollideVols; k++)
	{
		vol = _currentMap->getCollideVolPointerAt(k);
		if (vec2InRect(interpolatedPoint, vol->rect) && vol->active)
		{
			if (vol->glass)
			{
				//if the bullet hits glass, break it.
				glassShatter(vol, vec2f(leftToRight ? 0.1f : -0.1f, 0));
			}
			else
			{
				*stop = true;
				return;
			}
		}
	}
}

void Scene::killEnemies(Entity* source, vec2f interpolatedPoint, bool* stop)
{
	size_t k;
	for (k = 0; k < _currentMap->getNumberOfEnemies(); k++)
	{
		Enemy* enemy = _currentMap->getEnemyAt(k);
		if (enemy != source && enemy->isAlive())
		{
			if (vec2InRect(interpolatedPoint, enemy->getCollisionRect()))
			{
				enemy->setAlive(false);
				enemy->changeState(KNOCKED_OUT);
				*stop = true;
				break;
			}
		}
	}
}

bool Scene::isMouseCursorSeen(int mx, int my)
{
	size_t i;
	Enemy* enemy;
	vec2f position;
	vec2f v1, v2;
	vec2f mousePos = vec2f(mx + _camera.x, my + _camera.y);
	int firstRadius;
	float dist, angle, testAngle;
	GuardState state;
	for (i = 0; i < _currentMap->getNumberOfEnemies(); i++)
	{
		enemy = _currentMap->getEnemyAt(i);
		state = enemy->getState();
		if (state == KNOCKED_OUT || state == FALLING || state == PINNED)
		{
			continue;
		}
		vec2f position = enemy->getCollisionRectCenterPosition();
		vec2f v1 = vec2f(enemy->getDirection() == Right ? 1 : -1, 0);
		dist = vec2f_distance(position, mousePos);
		if (enemy->getType() == Enemy_Sniper)
		{
			firstRadius = ENEMY_FOV_RADIUS_SNIPER;
			testAngle = ENEMY_FOV_HALFANGLE_SEEN;
		}
		else
		{
			firstRadius = ENEMY_FOV_RADIUS_LIT;
			testAngle = enemy->canSeePlayer() ? ENEMY_FOV_HALFANGLE_SEEN : ENEMY_FOV_HALFANGLE;
		}
		if (dist < firstRadius)
		{
			v2 = vec2f_normalize(vec2f(mousePos.x - position.x, mousePos.y - position.y));
			angle = acos(v1.dot(v2));
			if (angle < ToRadian(testAngle) &&
			        !isTraceBlocked(enemy, position, mousePos, &_unused_c, &_unused_index, &_unused_bool, 16.0f, &Scene::traceGuardSight))
			{
				return true;
			}
		}
	}
	return false;
}