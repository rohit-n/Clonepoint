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

#define GLASSBREAKSPEEDSQUARED 64 //threshold at which moving objects break glass volumes.
#define CEILINGFRICTION 0.035f
#define FLOORFRICTION 0.04f

void Scene::handleMapCollisions(LivingEntity* ent, unsigned int dT)
{
	vec2f position;
	vec2f velocity;
	vec2f targetPos;
	Rect entRect;
	Rect velRect;

	CollisionVolume* vol, *vol2;
	size_t i, j;

	for (i = 0; i < _numCollideVols; i++)
	{
		vol = _currentMap->getCollideVolPointerAt(i);

		position = ent->getCollisionRectPosition();
		velocity = ent->getVelocity() * dT;
		targetPos = position + velocity;
		entRect = ent->getCollisionRect();

		if (position.x < targetPos.x)
			velRect.x = position.x;
		else velRect.x = targetPos.x;

		if (position.y < targetPos.y)
			velRect.y = position.y;
		else velRect.y = targetPos.y;

		velRect.w = fabs(velocity.x);
		velRect.h = fabs(velocity.y);

		if (velocity.x > 0.0f)
		{
			velRect.x += entRect.w;
		}
		if (velocity.y > 0.0f)
		{
			velRect.y += entRect.h;
		}
		//TODO: Do not like this at all.
		if (velRect.w == 0.0f && velRect.h != 0.0f)
		{
			velRect.w = 1.0f;
		}

		if (velRect.h == 0.0f && velRect.w != 0.0f)
		{
			velRect.h = 1.0f;
		}

		//extra check for entity rect is necessary if volume is very thin.
		if ((vol->active() || (ent != _player.get() && vol->guardblock())) && (check_collision(velRect, vol->rect) || check_collision(entRect, vol->rect)))
		{
			if (vol->glass() && velocity.length_squared() >= GLASSBREAKSPEEDSQUARED)
			{
				addNoise(position.x - _camera.x, position.y - _camera.y, 512, true, ALERT_RUN, nullptr);
				glassShatter(vol, ent->getVelocity());
				ent->setVelX(ent->getVelocity().x / 2);
				continue;
			}

			if (velocity.y > 0.0f && position.y < vol->rect.y && velRect.y + velRect.h > vol->rect.y)
			{
				bool abandonGround = false;
				vec2f testPoint = velocity.x > 0.0f ? vec2f(entRect.x + entRect.w + velocity.x, entRect.y) : vec2f(entRect.x + velocity.x, entRect.y);

				if (ent == _player.get() && (_player->getAttachType() == RightSide || _player->getAttachType() == LeftSide))
				{
					if (_player->getAttachType() == RightSide || _player->getAttachType() == LeftSide)
					{
						_player->detach();
					}
				}

				for (j = 0; j < _numCollideVols; j++)
				{
					if (i == j) continue;
					vol2 = _currentMap->getCollideVolPointerAt(j);
					if (vol2->active() && vec2InRect2(testPoint, vol2->rect))
					{
						abandonGround = true;
						break;
					}
				}

				if (!abandonGround)
				{
					//hit the ground.
					ent->setVelY(0);
					if (velocity.x != 0.0f && ent == _player.get())
					{
						setAccel(_player->getAccelerationStruct(), true, velocity.x < 0.0f ? FLOORFRICTION : -FLOORFRICTION, 0.0f);
					}
					ent->landOnGround();
					ent->setCollisionRectPosition(position.x, vol->rect.y - entRect.h);
					if (ent == _player.get())
					{
						if (!_player->isAlive())
						{
							_player->setCollisionRectDims(40, 18, ENTDIM);
							_player->setCollisionRectPosition(position.x, vol->rect.y - _player->getCollisionRect().h);
						}
					}
					return;
				}
				else
				{
					continue;
				}
			}
			else
			{
				bool ceilCheck = velocity.y < 0.0f && position.y > vol->rect.y;
				bool abandonCeil = false;
				vec2f testPoint = velocity.x > 0.0f ? vec2f(entRect.x + entRect.w + velocity.x, entRect.y + entRect.h) : vec2f(entRect.x + velocity.x, entRect.y + entRect.h);

				if (ent == _player.get() && _player->getAttachType() != LeftSide && _player->getAttachType() != RightSide)
				{
					ceilCheck = ceilCheck && position.y + entRect.h > vol->rect.y + vol->rect.h;
				}

				if (ceilCheck && ent == _player.get())
				{
					for (j = 0; j < _numCollideVols; j++)
					{
						if (i == j) continue;
						vol2 = _currentMap->getCollideVolPointerAt(j);
						if (vol2->active() && vec2InRect2(testPoint, vol2->rect))
						{
							abandonCeil = true;
							break;
						}
					}

					if (!abandonCeil)
					{
						//attach to ceiling.
						_player->attachToVolume(vol, Ceiling);
						ent->setCollisionRectPosition(position.x, vol->rect.y + vol->rect.h);
						ent->setVelY(0);
						if (velocity.x != 0.0f)
						{
							setAccel(_player->getAccelerationStruct(), true, velocity.x < 0.0f ? CEILINGFRICTION : -CEILINGFRICTION, 0.0f);
						}
						checkPlayerCeilingEnd();
						return;
					}
					else
					{
						continue;
					}
				}
			}

			if (velocity.x != 0.0f)
			{
				ent->setCollisionRectPosition(velocity.x > 0.0f ? vol->rect.x - entRect.w : vol->rect.x + vol->rect.w, position.y);

				if (ent == _player.get())
				{
					ent->setVelX(0);
					ent->setVelY(0);
					setAccel(_player->getAccelerationStruct(), false, 0.0f, 0.0f);

					if (!_player->isOnGround() || _player->getAttachType() == Ceiling)
					{
						_player->attachToVolume(vol, velocity.x > 0.0f ? RightSide : LeftSide);
					}
					i = 0;
					continue;
				}
				else //guards turn back, or change to a cautious state.
				{
					Enemy* enemy = static_cast<Enemy*>(ent);
					if (enemy->getState() == INVESTIGATING)
					{
						//The enemy was investigating something but ran into a dead end.
						enemy->changeState(IDLE_CAUTION);
					}
					else if (enemy->getState() == PATROLLING)
					{
						enemy->setVelX(-enemy->getVelocity().x);
					}
					else
					{
						enemy->setVelX(0);
					}
					break;
				}
			}
		}
	}
}

void Scene::glassShatter(CollisionVolume* vol, vec2f velocity)
{
	Locator::getAudio()->playSound("glass_break");
	bool vertical = (vol->rect.h > vol->rect.w);
	float midX = vol->rect.x + (vol->rect.w / 2);
	float midY = vol->rect.y + (vol->rect.h / 2);
	float goal = vertical ? vol->rect.h : vol->rect.w;

	int increment = 15;
	int i;
	double r;

	for (i = 0; i < goal; i += increment)
	{
		r = ((double)rand() / (double)(RAND_MAX)) + 1;
		if (vertical)
		{
			addParticle(midX, vol->rect.y + i, velocity.x / 2 * r, velocity.y / 2);
		}
		else
		{
			addParticle(vol->rect.x + i, midY, velocity.x / 2 * r, velocity.y / 2);
		}
	}

	vol->flags &= ~COLLISION_ACTIVE;
}

void Scene::handleParticles()
{
	CollisionVolume* vol;
	Particle* p;
	size_t i, j;
	vec2f position;

	for (i = 0; i < _particles.size(); i++)
	{
		p = getParticleAt(i);

		if (p->isAlive())
		{
			position = p->getPosition();
			for (j = 0; j < _numCollideVols; j++)
			{
				vol = _currentMap->getCollideVolPointerAt(j);
				if (vol->active() && (check_collision(p->getCollisionRect(), vol->rect)))
				{
					p->setAlive(false);
					break;
				}
			}

			if (position.x < 0 || position.y < 0 || position.x > _currentMap->getMapWidth()|| position.y > _currentMap->getMapHeight())
			{
				p->setAlive(false);
			}
		}
	}
}

bool Scene::checkIfEntOnGround(LivingEntity* ent)
{
	bool checkOnGround = false;
	bool trapFall = true; //applies to guards - true if guard's aabb doesn't touch ground at all.

	Rect cRect = ent->getCollisionRect();
	vec2f testPointLeft, testPointRight;
	CollisionVolume* vol;
	size_t i;
	size_t j;

	testPointLeft = vec2f(cRect.x, cRect.y + cRect.h + 1);
	testPointRight = vec2f(cRect.x + cRect.w, cRect.y + cRect.h + 1);
	bool checkLeft, checkRight;

	if (cRect.y + cRect.h > _currentMap->getMapHeight())
	{
		return false;
	}

	for (i = 0; i < _numCollideVols; i++)
	{
		vol = _currentMap->getCollideVolPointerAt(i);
		if (vol->active())
		{
			checkLeft = vec2InRect(testPointLeft, vol->rect);
			checkRight = vec2InRect(testPointRight, vol->rect);

			if (ent == _player.get())
			{
				if (checkLeft || checkRight)
				{
					//player is still on ground if at least one point colliding.
					checkOnGround = true;
					break;
				}
			}
			if (dynamic_cast<Enemy*>(ent))
			{
				if (checkLeft || checkRight) //guard cannot suddenly fall from trap door if at least one point colliding.
				{
					trapFall = false;
				}

				if (checkLeft && checkRight)
				{
					checkOnGround = true;
					break;
				}

				if (checkLeft xor checkRight) //Only one point is colliding with this volume - check if other point is colliding with another.
				{
					for (j = i + 1; j < _numCollideVols; j++)
					{
						vol = _currentMap->getCollideVolPointerAt(j);
						if (vol->active())
						{
							if (vec2InRect(checkLeft ? testPointRight : testPointLeft, vol->rect))
							{
								checkOnGround = true;
								break;
							}
						}
					}
				}
			}
		}
		if (checkOnGround)
		{
			break;
		}
	}

	if (!checkOnGround && ent->isOnGround())
	{
		if (dynamic_cast<Enemy*>(ent))
		{
			Enemy* enemy = static_cast<Enemy*>(ent);
			if (trapFall)
			{
				ent->setOnGround(false);
				if (enemy->getState() != PINNED && !enemy->ignoringFall())
				{
					enemy->changeState(FALLING);
				}
			}
			else
			{
				if (enemy->getState() == INVESTIGATING)
				{
					//Enemy was investigating, but ran into dead end.
					checkLeft = checkRight = false;
					for (i = 0; i < _numCollideVols; i++)
					{
						vol = _currentMap->getCollideVolPointerAt(i);
						if (vol->active())
						{
							checkLeft = vec2InRect(testPointLeft, vol->rect);
							checkRight = vec2InRect(testPointRight, vol->rect);
							if (checkLeft || checkRight)
								break;
						}
					}

					float finalX;

					if (checkLeft)
					{
						finalX = vol->rect.x + vol->rect.w - cRect.w;
					}
					else if (checkRight)
					{
						finalX = vol->rect.x;
					}
					else
					{
						finalX = cRect.x - (ent->getVelocity().x * 10);
					}

					ent->setCollisionRectPosition(finalX, cRect.y);
					enemy->changeState(IDLE_CAUTION);
				}
				else if (enemy->getState() == PATROLLING)
				{
					ent->setVelX(-ent->getVelocity().x);
				}
			}
		}
		else
		{
			ent->setOnGround(false);
		}
		return false;
	}
	else
	{
		return true;
	}
}

//check if the player has finished climbing up "roof" of a collision volume.
void Scene::checkPlayerClimbFinish()
{
	Rect pRect = _player->getCollisionRect();
	CollisionVolume* vol = _player->getAttachedVolume();
	//set to true if player finds an active collision volume in the way of reaching the roof.
	//may be ignored if player first finds *another* volume that simply lets them continue
	//climbing up.
	bool shouldStop = false;

	vec2f testPoint;
	size_t i, j;
	AttachType at = _player->getAttachType();
	//A test rect as if player fully climbed roof. Used to check for last collision.
	Rect testRect = _player->getCollisionRect();
	testRect.y = vol->rect.y - testRect.h;
	if (at == RightSide)
	{
		testPoint = vec2f(pRect.x + pRect.w + 1, pRect.y);
		testRect.x += testRect.w;
	}
	else if (at == LeftSide)
	{
		testPoint = vec2f(pRect.x - 1, pRect.y);
		testRect.x -= testRect.w;
	}
	else
	{
		return;
	}

	if (!vec2InRect(testPoint, vol->rect))
	{
		//player no longer attached to their current attachedVolume - check if another collision volume was hit before detaching.
		CollisionVolume* finalVol;
		for (i = 0; i < _numCollideVols; i++)
		{
			finalVol = _currentMap->getCollideVolPointerAt(i);
			if (finalVol->active())
			{
				if (vec2InRect(testPoint, finalVol->rect))
				{
					_player->attachToVolume(finalVol, at);
					return;
				}
				else
				{
					if (check_collision(testRect, finalVol->rect))
					{
						shouldStop = true;
					}
				}
			}
		}

		if (!shouldStop) //check if an Enforcer is in the way, too.
		{
			Enemy* enemy;
			for (j = 0; j < _currentMap->getNumberOfEnemies(); j++)
			{
				enemy = _currentMap->getEnemyAt(j);
				if (enemy->getType() == Enemy_Enforcer && check_collision(testRect, enemy->getCollisionRect()))
				{
					shouldStop = true;
					break;
				}
			}
		}

		if (shouldStop)
		{
			//Player cannot fully go over volume - possibly a door or Enforcer in the way. Stay where you are.
			_player->setVelY(0);
			_player->setCollisionRectPosition(pRect.x, vol->rect.y);
			return;
		}

		//player has climbed up to roof - raise them up and go left or right a little.

		if (at == RightSide)
			_player->setCollisionRectPosition(pRect.x + pRect.w, pRect.y - pRect.h);
		else if (at == LeftSide)
			_player->setCollisionRectPosition(pRect.x - pRect.w, pRect.y - pRect.h);
		_player->detach();

		_player->setVelY(0);
		_player->changeAnimationSequence(Locator::getAnimationManager()->getSequence(ANIM_PLAYER_REACH_ROOF));
	}
}

//check if the player has finished climbing down a collision volume and needs to go under.
//TODO: Combine with above function.
void Scene::checkPlayerClimbDownFinish()
{
	Rect pRect = _player->getCollisionRect();
	CollisionVolume* vol = _player->getAttachedVolume();

	//set to true if player finds an active collision volume in the way of a ceiling attachment.
	//may be ignored if player first finds *another* volume that simply lets them continue
	//sliding down.
	bool shouldStop = false;

	vec2f testPoint;
	size_t i;
	AttachType at = _player->getAttachType();
	if (at == RightSide)
	{
		testPoint = vec2f(pRect.x + pRect.w + 1, pRect.y + pRect.h);
	}
	else if (at == LeftSide)
	{
		testPoint = vec2f(pRect.x - 1, pRect.y  + pRect.h);
	}
	else
	{
		return;
	}

	if (!vec2InRect(testPoint, vol->rect))
	{
		//player no longer attached to their current attachedVolume - check if another collision volume was hit before detaching.
		CollisionVolume* finalVol;
		for (i = 0; i < _numCollideVols; i++)
		{
			finalVol = _currentMap->getCollideVolPointerAt(i);
			if (finalVol->active())
			{
				if (vec2InRect(testPoint, finalVol->rect))
				{
					_player->attachToVolume(finalVol, at);
					return;
				}
				else
				{
					Rect testRect = _player->getCollisionRect();
					testRect.y = vol->rect.y + vol->rect.h;
					if (at == RightSide)
					{
						testRect.x += testRect.w;
					}
					else
					{
						testRect.x -= testRect.w;
					}
					if (check_collision(testRect, finalVol->rect))
					{
						shouldStop = true;
					}
				}
			}
		}

		if (shouldStop)
		{
			//Player cannot fully go under volume - possibly a door in the way. Stay where you are.
			_player->setVelY(0);
			_player->setCollisionRectPosition(pRect.x, vol->rect.y + vol->rect.h - pRect.h);
			return;
		}

		//player has climbed down volume - do a ceiling attachment with the bottom.
		if (at == RightSide)
			_player->setCollisionRectPosition(pRect.x + pRect.w - 1, vol->rect.y + vol->rect.h);
		else if (at == LeftSide)
			_player->setCollisionRectPosition(pRect.x - pRect.w + 1, vol->rect.y + vol->rect.h);
		_player->attachToVolume(vol, Ceiling);

		_player->changeAnimationSequence(Locator::getAnimationManager()->getSequence(ANIM_PLAYER_ATTACH_TO_CEILING));

		_moveDelay = true;
		_player->setVelY(0); //necessary or player would just keep going down.
	}
}

//player is pressing "down" while on ground - check if they should do a side attach to this volume.
void Scene::tryPlayerAttachSide()
{
	setAccel(_player->getAccelerationStruct(), false, 0.0f, 0.0f);
	Rect pRect = _player->getCollisionRect();
	vec2f testPoint1, testPoint2;
	CollisionVolume* vol;
	CollisionVolume* vol2;
	size_t i, j;

	testPoint1 = vec2f(pRect.x, pRect.y + pRect.h + 1);
	testPoint2 = vec2f(pRect.x + pRect.w, pRect.y + pRect.h + 1);
	bool check1, check2;
	Rect finalRect = {0, 0, pRect.w, pRect.h};

	for (i = 0; i < _numCollideVols; i++)
	{
		vol = _currentMap->getCollideVolPointerAt(i);
		if (vol->active())
		{
			finalRect.y = vol->rect.y;
			check1 = vec2InRect(testPoint1, vol->rect);
			check2 = vec2InRect(testPoint2, vol->rect);
			if (check1 xor check2) //if the player is standing on the edge of a volume's "roof"
			{
				for (j = i + 1; j < _numCollideVols; j++)
				{
					vol2 = _currentMap->getCollideVolPointerAt(j);

					if (vol2->active() && vec2InRect(check1 ? testPoint2 : testPoint1, vol2->rect))
					{
						//if player is on top of two separate collision volumes, don't attach.
						return;
					}
				}

				if (pRect.y < vol->rect.y)
				{
					if (check1)
						finalRect.x = vol->rect.x + vol->rect.w;
					else
						finalRect.x = vol->rect.x - pRect.w;

					for (j = i + 1; j < _numCollideVols; j++)
					{
						vol2 = _currentMap->getCollideVolPointerAt(j);

						if (vol2->active() && check_collision(finalRect, vol2->rect))
						{
							return;
						}
					}

					_player->setCollisionRectPosition(finalRect.x, finalRect.y);
					_player->attachToVolume(vol, check1 ? LeftSide : RightSide);
					_player->changeAnimationSequence(Locator::getAnimationManager()->getSequence(ANIM_PLAYER_ATTACH_DOWN));

					return;
				}
			}
		}
	}
}

//Check if player is finished with ceiling attachment.
//Either player hits another collision volume and
//attaches to that, or they move up.
//The player's attachType is assumed to be "ceiling" at the
//start of this function.
void Scene::checkPlayerCeilingEnd()
{
	bool shouldDelay = true;

	Rect pRect = _player->getCollisionRect();
	vec2f testPoint1, testPoint2;
	CollisionVolume* currentVol = _player->getAttachedVolume();

	if (currentVol == nullptr)
	{
		return;
	}

	CollisionVolume* otherVol;
	size_t i;

	testPoint1 = vec2f(pRect.x, pRect.y - 1);
	testPoint2 = vec2f(pRect.x + pRect.w, pRect.y - 1);
	bool checkLeft, checkRight;
	Rect endRect = pRect;
	endRect.y = currentVol->rect.y + currentVol->rect.h - endRect.h;

	if (currentVol->active())
	{
		checkLeft = vec2InRect(testPoint1, currentVol->rect);
		checkRight = vec2InRect(testPoint2, currentVol->rect);

		if ((checkLeft xor checkRight) || (!checkLeft && !checkRight)) //if the player is approaching the edge of a volume's "ceiling"
		{
			//first check if they hit ANOTHER collision volume. Otherwise, move up.
			for (i = 0; i < _numCollideVols; i++)
			{
				otherVol = _currentMap->getCollideVolPointerAt(i);

				endRect.x = pRect.x;

				if (checkLeft)
				{
					endRect.x += endRect.w;
				}
				else
				{
					endRect.x -= endRect.w;
				}

				if (otherVol != currentVol && otherVol->active())
				{
					if (check_collision2(pRect, otherVol->rect))
					{
						//continue along this new volume.
						if (pRect.y + pRect.h > otherVol->rect.y + otherVol->rect.h)
						{
							_player->attachToVolume(otherVol, Ceiling);
							return;
						}
						else
						{
							shouldDelay = false;
						}
					}

					if (check_collision2(endRect, otherVol->rect))
					{
						//player would be colliding with a volume if they went up. Either stop, or detach.
						if (_player->isAccelerating())
						{
							_player->detach();
						}
						else
						{
							_player->setVelX(0);
							_player->setCollisionRectPosition(checkLeft ? currentVol->rect.x + currentVol->rect.w - pRect.w : currentVol->rect.x, pRect.y);
						}
						return;
					}
				}
			}

			if (checkLeft xor checkRight)
			{
				//The player did not hit another collision volume, so do side attachment to current one and move up.
				if (checkLeft)
				{
					_player->setCollisionRectPosition(currentVol->rect.x + currentVol->rect.w, pRect.y - pRect.h);
					_player->attachToVolume(currentVol, LeftSide);
				}
				else //checkRight
				{
					_player->setCollisionRectPosition(currentVol->rect.x - pRect.w, pRect.y - pRect.h);
					_player->attachToVolume(currentVol, RightSide);
				}

				_player->changeAnimationSequence(Locator::getAnimationManager()->getSequence(ANIM_PLAYER_ATTACH_FROM_CEILING));

				//Prevent player from immediately detaching when moving in any direction.
				_moveDelay = shouldDelay;
				return;
			}
		}

		//Needed to avoid sticking to invisible ceiling when ceiling attaching to volume at certain angles.
		if (!checkLeft && !checkRight)
		{
			_player->detach();
		}
	}
}

void Scene::handlePlayerPounceEnemy(Enemy* enemy, unsigned int dT)
{
	//decide if the player should pin the enemy or make them fall out (of a window, maybe).
	vec2f velocity = _player->getVelocity() * dT;
	vec2f position = enemy->getCollisionRectPosition();
	CollisionVolume* vol, *vol2;
	float velX = velocity.x;
	Rect enemyRect = enemy->getCollisionRect();
	Rect velRect = {enemyRect.x, enemyRect.y, velX, enemyRect.h};
	if (velX > 0)
	{
		velRect.x += enemyRect.w;
	}
	else
	{
		velRect.w = 0 - velRect.w;
		velRect.x -= velRect.w;
	}
	vec2f testPoint1, testPoint2;
	bool check1, check2, onGround, glassBroken;
	onGround = glassBroken = false;
	size_t i, j;

	testPoint1 = vec2f(enemyRect.x + velX, enemyRect.y + enemyRect.h + 1);
	testPoint2 = vec2f(enemyRect.x + enemyRect.w + velX, enemyRect.y + enemyRect.h + 1);

	for (i = 0; i < _numCollideVols; i++)
	{
		vol = _currentMap->getCollideVolPointerAt(i);
		if (vol->active())
		{
			if (check_collision(vol->rect, velRect) && vol->glass() && velocity.length_squared() >= GLASSBREAKSPEEDSQUARED)
			{
				glassBroken = true;
				glassShatter(vol, _player->getVelocity());
				continue;
			}

			check1 = vec2InRect(testPoint1, vol->rect);
			check2 = vec2InRect(testPoint2, vol->rect);
			if (check1 && check2)
			{
				onGround = true;
				break;
			}

			if (check1 xor check2) //Only one point is colliding with this volume - check if other point is colliding with another.
			{
				for (j = i + 1; j < _numCollideVols; j++)
				{
					vol2 = _currentMap->getCollideVolPointerAt(j);
					if (vol2->active())
					{
						if (vec2InRect(check1 ? testPoint2 : testPoint1, vol2->rect))
						{
							onGround = true;
							break;
						}
					}
				}
			}
		}

		if (onGround)
		{
			break;
		}
	}

	if (onGround)
	{
		enemy->setVelX(_player->getVelocity().x);
		setAccel(enemy->getAccelerationStruct(), true, velocity.x < 0.0f ? FLOORFRICTION : -FLOORFRICTION, 0.0f);
		_player->setVelX(0);

		if (enemy->getType() == Enemy_Enforcer)
		{
			enemy->changeState(IDLE_CAUTION);
		}
		else
		{
			if (enemy->getDirection() == _player->getDirection())
			{
				_player->reverseDirection();
				enemy->setDirection(_player->getDirection());
			}

			_player->pinEnemy(enemy);
		}
	}
	else
	{
		enemy->setOnGround(false);
		enemy->changeState(FALLING);
		enemy->setAlive(false);
		enemy->setVelX(_player->getVelocity().x / 1.4f);
		_player->setVelX(_player->getVelocity().x / 1.4f);

		if (enemy->isPositionBehind(_player->getCollisionRectPosition().x))
		{
			enemy->reverseDirection();
		}
	}

	if (glassBroken) //add the noise for a broken glass later, so that another enemy besides this is nominated to be alerted
	{
		addNoise(position.x - _camera.x, position.y - _camera.y, 512, true, ALERT_RUN, nullptr);
	}
}

void Scene::handlePlayerShotWhileAttached()
{
	AttachType last = _player->getLastAttachType();
	if (last == RightSide )
	{
		_player->setVelX(-0.1f);
	}

	if (last == LeftSide )
	{
		_player->setVelX(0.1f);
	}
}