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

#define CROSSLINKPAN 5
#define CROSSLINKPANTRHESHOLD 30
#define ENEMYGUNLINKCOST 3
#define TIME_TO_SHOW_MSG 5000
#define TIME_TO_SHOW_TUTORIAL_MSG 3000
#define PLAYER_LIGHT_THRESHOLD 50
#define PLAYERSPEED 0.20
#define PLAYERAIMINGSPEEDMOD 0.25
#define PLAYERACCELERATION 0.05f
#define PLAYERCEILSPEEDMOD 0.575
#define PLAYERCLIMBSPEED 0.114
#define PLAYERCLIMBDOWNSPEED 0.178
#define BASE_PLAYER_JUMP_POWER 0.65f
#define BASE_TIME_TO_FULL_JUMP 1000 //Time in milliseconds needed to fully charge jump. Decreases with upgrades.
#define JUMPSTARTTIME 550 //Time for actually starting the timer. Done to avoid waiting long for a substantial jump.
#define JUMPCLAMP 1.5f

Scene::Scene()
{
	_player.reset(new Player(0, 0, Right));
	_crosslink = false;
	_selecting = false;
	_levelOver = false;
	_moveDelay = false;
	_elevatorLocked = false;
	_stopTrajPoint = 0;
	_totalObjectives = 0;
	_numEnemies = 0;
	_currentMap.reset();
	_linker = nullptr;
	_switcher = nullptr;
	_computer = nullptr;
	_elevator = nullptr;
	_circuitBox = nullptr;
	_mouseDragPos = vec2f(0, 0);
	_jumpImpulse = vec2f(0, 0);

	_playerOverlappingEnts[UESwitch] =
	    _playerOverlappingEnts[UETerminal] =
	        _playerOverlappingEnts[UEElevator] =
	            _playerOverlappingEnts[UECircuitBox] =
	                _playerOverlappingEnts[UEStairs] = false;

	_endRect.w = 1;
	_endRect.h = 1;
	_endRect.x = 1;
	_endRect.y = 1;

	_circuitUnlocked[BLUE] = _circuitUnlocked[GREEN] = _circuitUnlocked[VIOLET] = _circuitUnlocked[YELLOW] = false;

	_playerJumpPower = BASE_PLAYER_JUMP_POWER;
	_timeToFullChargeJump = BASE_TIME_TO_FULL_JUMP;

	_jumpPowerTimer = _timeToFullChargeJump - JUMPSTARTTIME;
	_jumpPower = 0.0f;

	_currentSave = 0;
	_saveTimer = 0;
	_loadMenuVisible = false;
	_saveTimeSince[0] = _saveTimeSince[1] = _saveTimeSince[2] = -1;
	_mapFilename[0] = '\0';

	_particles.push_back(std::unique_ptr<Particle>(new Particle(0, 0, 0)));
	_particles.push_back(std::unique_ptr<Particle>(new Particle(0, 0, 1)));
	_particles.push_back(std::unique_ptr<Particle>(new Particle(0, 0, 2)));
	_particles.push_back(std::unique_ptr<Particle>(new Particle(0, 0, 3)));
	_particles.push_back(std::unique_ptr<Particle>(new Particle(0, 0, 4)));
	_particles.push_back(std::unique_ptr<Particle>(new Particle(0, 0, 5)));
	_particles.push_back(std::unique_ptr<Particle>(new Particle(0, 0, 6)));
	_particles.push_back(std::unique_ptr<Particle>(new Particle(0, 0, 7)));
	_particles.push_back(std::unique_ptr<Particle>(new Particle(0, 0, 1)));
	_particles.push_back(std::unique_ptr<Particle>(new Particle(0, 0, 2)));

	_testAttach = NotAttached;

	_mousedOverObject = MO_Nothing;

	_lightEnteredAlpha = 0.0f;
	_inputPopupAlpha = 1.0f;
	_showInputPopup = false;

	_laserEnd = vec2f(0, 0);

	_playerShotFired = false;
	_timeToSniper = 1000;
	_numPlayerBullets = 0;
	_playerEnergy = 0;
	_stringMessageTimer = -1;
	_stringMessage = NUMBER_OF_STRING_MESSAGES;
}

Scene::~Scene()
{
	LOGF((stdout, "Running scene destructor!\n"));
	_particles.clear();
}

void Scene::loadMap(const char* filename, bool savegame)
{
	//TODO: Remove redundancy with constructor.
	_player->setAlive(true);
	_player->setCollisionRectDims(18, 40, ENTDIM);
	_loadMenuVisible = false;
	_saveTimer = 0;
	_totalObjectives = 0;
	_numEnemies = 0;
	size_t i;

	if (strcmp(_mapFilename, filename)) //if we're loading map for first time or switching from another map
	{
		if (_currentMap)
		{
			_currentMap.reset();
		}
		_currentMap.reset(new Map());
		_currentMap->loadFromFile(filename, savegame);

		if (!savegame)
		{
			makeInitialLinks();
		}

		_numCollideVols = _currentMap->getNumberOfCollideVols();

		for (i = 0; i < _currentMap->getNumberOfLights(); i++)
		{
			computeVisibility(_currentMap->getLightAt(i));
		}

		for (i = 0; i < _currentMap->getNumberOfEnts(); i++)
		{
			if (dynamic_cast<Door*>(_currentMap->getEntAt(i)))
			{
				Door* door = static_cast<Door*>(_currentMap->getEntAt(i));
				calculateOverlappingLightsOfDoor(door);
			}
			if (dynamic_cast<Enemy*>(_currentMap->getEntAt(i)))
			{
				Enemy* enemy = static_cast<Enemy*>(_currentMap->getEntAt(i));
				enemy->setFireFunction([this](Enemy* enemy, vec2f target, GunShotTraceType gstt)
				{
					this->traceBullet(enemy, target, gstt, true);
				});
			}
		}

		_currentMap->getSniper()->setFireFunction([this](Enemy* enemy, vec2f target, GunShotTraceType gstt)
		{
			this->traceBullet(_currentMap->getSniper(), target, gstt, true);
		});

		vec2f startPos = _currentMap->getPlayerStartPos();
		_player->setCollisionRectPosition(startPos.x, startPos.y);

		if (_currentMap->subwayFound())
		{
			vec2f subwayPos = _currentMap->getSubwayPosition();
			_endRect.w = 60;
			_endRect.h = 128;
			_endRect.x = subwayPos.x + 68;
			_endRect.y = subwayPos.y - ENTDIM;
		}
		else
		{
			_endRect.w = 64;
			_endRect.h = _currentMap->getMapHeight();
			_endRect.x = _currentMap->getMapWidth() - _endRect.w;
			_endRect.y = 0;
		}

		strcpy(_mapFilename, filename);
	}

	for (i = 0; i < _currentMap->getNumberOfEnts(); i++)
	{
		if (dynamic_cast<MainComputer*>(_currentMap->getEntAt(i)))
			_totalObjectives++;

		if (dynamic_cast<Enemy*>(_currentMap->getEntAt(i)))
			_numEnemies++;
	}

	LOGF((stdout, "Map %s has %i main objective(s) and %i enemies.\n", filename, _totalObjectives, _numEnemies));
}

void Scene::reloadMap()
{
	_saveTimeSince[0] = _saveTimeSince[1] = _saveTimeSince[2] = -1;
	loadGame("start.sav");
}

void Scene::handleStairCollision(LivingEntity* le)
{
	size_t i;
	Stairs* sw;
	bool found = false;
	for (i = 0; i < _currentMap->getNumberOfStairs(); i++)
	{
		sw = _currentMap->getStairsAt(i);
		if (check_collision(le->getCollisionRect(), sw->getCollisionRect()))
		{
			le->setOverlappingStairs(sw);
			found = true;
			if (_player.get() == le)
			{
				_playerOverlappingEnts[UEStairs] = true;
			}
			break;
		}
	}

	if (!found)
	{
		le->setOverlappingStairs(nullptr);
	}
}

void Scene::update(unsigned int dT)
{
	ElevatorDoor* target;
	ElevatorShaft* shaft;
	size_t i;
	updateCamera();
	if (!_player->isInElevator())
		handleMapCollisions(_player.get(), dT);
	_player->update(dT);
	_playerOverlappingEnts[UEStairs] = false;
	handleStairCollision(_player.get());
	checkIfEntOnGround(_player.get());

	calculatePlayerVisibility();

	for (i = 0; i < _particles.size(); i++)
	{
		_particles[i]->update(dT);
	}

	handleParticles();

	if (_player->getAttachType() != NotAttached && _player->getVelocity().y != 0.0f)
	{
		if (_player->getVelocity().y < 0)
			checkPlayerClimbFinish();
		else if (_player->getVelocity().y > 0)
			checkPlayerClimbDownFinish();
	}
	else if (_player->getAttachType() == Ceiling && _player->getVelocity().x != 0.0f)
	{
		checkPlayerCeilingEnd();
	}

	_playerOverlappingEnts[UESwitch] = false;
	_playerOverlappingEnts[UETerminal] = false;
	_playerOverlappingEnts[UEElevator] = false;
	_playerOverlappingEnts[UECircuitBox] = false;
	_playerOverlappingEnts[UEEnemy] = _player->isPinning();

	for (i = 0; i < _currentMap->getNumberOfEnts(); i++)
	{
		Entity* ent = _currentMap->getEntAt(i);
		if (dynamic_cast<LivingEntity*>(ent))
		{
			handleMapCollisions((LivingEntity*)ent, dT);
			checkIfEntOnGround((LivingEntity*)ent);
			handleStairCollision((LivingEntity*)ent);
			_currentMap->getEntAt(i)->update(dT);
			if (dynamic_cast<Enemy*>(ent))
			{
				updateEnemy((Enemy*)ent, dT);
			}
		}
		else if (dynamic_cast<Door*>(ent))
		{
			Door* door = static_cast<Door*>(ent);
			door->update(dT);
			if (door->isDirty())
			{
				updateOverlappingFOVs(door);
				if (door->getType() == Door_Vault && !door->isOpened())
				{
					handleVaultDoorClose(door);
				}
				door->setDirty(false);
			}
		}
		else if (dynamic_cast<LightSwitch*>(ent))
		{
			if (check_collision(ent->getCollisionRect(), _player->getCollisionRect()) && !((LightSwitch*)ent)->isHandScanner())
			{
				_playerOverlappingEnts[UESwitch] = true;
				_switcher = static_cast<LightSwitch*>(ent);
			}
		}
		else if (dynamic_cast<MainComputer*>(ent))
		{
			if (check_collision(ent->getCollisionRect(), _player->getCollisionRect()) && ((MainComputer*)ent)->isActive())
			{
				_playerOverlappingEnts[UETerminal] = true;
				_computer = static_cast<MainComputer*>(ent);
			}
		}
		else if (dynamic_cast<ElevatorDoor*>(ent))
		{
			ent->update(dT);
			if (check_collision(ent->getCollisionRect(), _player->getCollisionRect()))
			{
				_elevator = static_cast<ElevatorDoor*>(ent);
				_playerOverlappingEnts[UEElevator] = (!_player->isInElevator() || _player->getElevatorDoor() != _elevator);
				if (_player->isInElevator() && _player->getElevatorDoor() != _elevator)
				{
					_player->switchElevator(_elevator);
				}
			}
		}
		else if (dynamic_cast<CircuitBox*>(ent))
		{
			if (check_collision(ent->getCollisionRect(), _player->getCollisionRect()))
			{
				_circuitBox = static_cast<CircuitBox*>(ent);
				_playerOverlappingEnts[UECircuitBox] = !_circuitBox->isHacked();
			}
		}
		else if (dynamic_cast<MotionScanner*>(ent))
		{
			updateMotionScanner((MotionScanner*)ent);
		}
		else if (dynamic_cast<SecurityCamera*>(ent))
		{
			updateSecurityCamera((SecurityCamera*)ent);
		}
		else if (dynamic_cast<PowerSocket*>(ent))
		{
			updatePowerSocket((PowerSocket*)ent);
		}
		else if (dynamic_cast<Alarm*>(ent))
		{
			Alarm* alarm = static_cast<Alarm*>(ent);
			alarm->update(dT);
			if (alarm->isSounded())
			{
				alarm->setSounded(false);
				addNoise(alarm->getCollisionRectPosition().x - _camera.x, alarm->getCollisionRectPosition().y - _camera.y, 512, true, ALERT_RUN, alarm);
			}
		}
		else if (dynamic_cast<TutorialMark*>(ent))
		{
			if (check_collision(ent->getCollisionRect(), _player->getCollisionRect()))
			{
				_stringMessage = static_cast<TutorialMark*>(ent)->getTutorialString();
				_stringMessageTimer = TIME_TO_SHOW_TUTORIAL_MSG;
			}
		}
	}

	if (_playerOverlappingEnts[UESwitch] ||
		_playerOverlappingEnts[UETerminal] ||
		_playerOverlappingEnts[UEElevator] ||
		_playerOverlappingEnts[UECircuitBox] ||
		_playerOverlappingEnts[UEStairs] ||
		_playerOverlappingEnts[UEEnemy])
	{
		if (!_showInputPopup)
		{
			_showInputPopup = true;
			_inputPopupAlpha = 0.0f;
		}
	}
	else
	{
		_showInputPopup = false;
	}

	for (i = 0; i < _currentMap->getNumberOfShafts(); i++)
	{
		shaft = _currentMap->getShaftAt(i);
		shaft->update();
		target = shaft->getTarget();
		if (target && fabs(shaft->getElevatorPosition().y - target->getCollisionRectPosition().y) < fabs(shaft->getVelocity()))
		{
			shaft->setOpenDoor(target, true);
			addNoise(shaft->getElevatorPosition().x - _camera.x, shaft->getElevatorPosition().y - _camera.y, 512, true, ALERT_LOOK, nullptr);
		}
	}

	//check map bounds
	//TODO: Apply this logic to ALL entities.
	if (_player->getCollisionRectPosition().x < 0)
	{
		_player->setCollisionRectPosition(0, _player->getCollisionRect().y);
	}

	if (_player->getCollisionRectPosition().x + _player->getCollisionRect().w > _currentMap->getMapWidth())
	{
		_player->setCollisionRectPosition(_currentMap->getMapWidth() - _player->getCollisionRect().w, _player->getCollisionRect().y);
	}

	if (_player->getCollisionRectPosition().y < 0)
	{
		_player->setPosY(0);
	}

	if (_player->getCollisionRect().y + _player->getCollisionRect().h > _currentMap->getMapHeight())
	{
		_player->setPosY(0);
		_player->setVelY(0);
	}

	if (check_collision(_player->getCollisionRect(), _endRect))
	{
		if (_totalObjectives == _player->getNumHackedTerminals())
		{
			endLevel();
		}
		else
		{
			_stringMessage = SM_ObjectivesIncomplete;
			_stringMessageTimer = TIME_TO_SHOW_MSG;
		}
	}

	if (_lightEnteredAlpha > 0.0f)
	{
		_lightEnteredAlpha -= (float)dT/1000;
	}

	if (_showInputPopup && _inputPopupAlpha < 1.0f)
	{
		_inputPopupAlpha += (float)dT/400;
	}

	if (_playerShotFired && _timeToSniper > 0)
	{
		if (_timeToSniper > 0)
		{
			_timeToSniper -= dT;
			if (_timeToSniper <= 0)
			{
				_currentMap->addSniper();
			}
		}
		else
		{
			_timeToSniper = 0;
		}
	}

	if (_stringMessageTimer >= 0.0f)
	{
		_stringMessageTimer -= dT;
	}

	//updateSaves(dT);
}

void Scene::updateEnemy(Enemy* enemy, unsigned int dT)
{
	Rect playerRect = _player->getCollisionRect();
	Rect enemyRect = enemy->getCollisionRect();
	GuardState state = enemy->getState();
	if (state == KNOCKED_OUT || state == FALLING)
	{
		if (enemy->getGun() != nullptr)
		{
			_currentMap->removeEnemyGun(enemy->getGun());
		}
	}

	if (!_player->isAimingGun())
	{
		enemy->setHeldAtGunpoint(false);
	}
	if (state != KNOCKED_OUT && state != FALLING && state != PINNED)
	{
		calculateStrongestLight(enemy);
		if (_player->isAlive() && !enemy->isAnimatingThroughStairs())
		{
			traceEnemyFOV(enemy);
		}
	}

	if (enemy->getSecondaryTarget() != nullptr || enemy->getTargetType() != TARGET_NONE)
		handleEnemyPathfind(enemy);

	if (check_collision(playerRect, enemyRect) && _player->isAlive()
	        && !_player->isInElevator() && !_player->isMovingThroughStairs() && state != KNOCKED_OUT && state != FALLING && state != PINNED
	        && !enemy->isAnimatingThroughStairs())
	{
		if (!_player->isOnGround())
		{
			handlePlayerPounceEnemy(enemy, dT);
		}
		else
		{
			if (playerRect.x < enemyRect.x)
			{
				_player->setCollisionRectPosition(enemyRect.x - playerRect.w - (_player->getVelocity().x * dT), playerRect.y);
			}
			else
			{
				_player->setCollisionRectPosition(enemyRect.x + enemyRect.w - (_player->getVelocity().x * dT), playerRect.y);
			}
			_player->setVelX(0);
			enemy->changeState(IDLE_CAUTION);
		}
	}
}

void Scene::updateMotionScanner(MotionScanner* scanner)
{
	size_t j;
	if (scanner->isTrespassed() && !check_collision(scanner->getCollisionRect(), scanner->getTrespasser()->getCollisionRect()))
	{
		//this motion scanner is no longer being trespassed, and is ready to activate again.
		scanner->resetTrespasser();
	}
	else //scanner has not been trespassed yet - look for a trespasser.
	{
		for (j = 0; j < _currentMap->getNumberOfEnts(); j++)
		{
			if (dynamic_cast<LivingEntity*>(_currentMap->getEntAt(j)))
			{
				//only living entities can trespass a motion scanner.
				if (handleMotionScannerWithEnt(scanner, (LivingEntity*)_currentMap->getEntAt(j)))
				{
					break;
				}
			}
		}

		//account for the player as well.
		if (!scanner->isTrespassed())
		{
			handleMotionScannerWithEnt(scanner, _player.get());
		}
	}
}

void Scene::updateSecurityCamera(SecurityCamera* camera)
{
	if (camera->isTrespassed() && !isPlayerInFOV(camera->getFOV()))
	{
		//this camera is no longer being trespassed, and is ready to activate again.
		camera->setTrespassed(false);
		if (!_crosslink)
		{
			camera->getFOV()->setColors(0.5, 0.5, 0);
		}
	}
	else //camera has not been trespassed yet - is the player intersecting it?
	{
		if (isPlayerInFOV(camera->getFOV()))
		{
			camera->activate();
			camera->setTrespassed(true);
			camera->getFOV()->setColors(0.5, 0.0, 0);
		}
	}
}

bool Scene::handleMotionScannerWithEnt(MotionScanner* scanner, LivingEntity* le)
{
	if (check_collision(scanner->getCollisionRect(), le->getCollisionRect()) && !scanner->isTrespassed())
	{
		scanner->activate();
		scanner->setTrespasser(le);
		return true;
	}
	return false;
}

void Scene::updatePowerSocket(PowerSocket* socket)
{
	if (!socket->isLive())
	{
		return;
	}
	size_t j;
	socket->deactivate();
	for (j = 0; j < _currentMap->getNumberOfEnts(); j++)
	{
		if (dynamic_cast<Enemy*>(_currentMap->getEntAt(j)))
		{
			if (check_collision(socket->getCollisionRect(), _currentMap->getEntAt(j)->getCollisionRect()))
			{
				//Any enemy near a live power socket is knocked out.
				((Enemy*)_currentMap->getEntAt(j))->changeState(KNOCKED_OUT);
				break;
			}
		}
	}
}

void Scene::updateCamera()
{
	vec2f pos = getPlayerPosition();

	if (!_crosslink)
	{
		if (pos.x - _camera.x > _camera.w * 0.625f)
			_camera.x = pos.x - _camera.w * 0.625;
		else if (pos.x - _camera.x < _camera.w * 0.375)
			_camera.x = pos.x - _camera.w * 0.375;

		_camera.y = pos.y - _camera.h / 2;
	}

	if( _camera.x < 0 )
	{
		_camera.x = 0;
	}
	if( _camera.y < 0 )
	{
		_camera.y = 0;
	}
	if( _camera.x > _currentMap->getMapWidth() - _camera.w )
	{
		_camera.x = _currentMap->getMapWidth() - _camera.w;
	}
	if( _camera.y > _currentMap->getMapHeight() - _camera.h )
	{
		_camera.y = _currentMap->getMapHeight() - _camera.h;
	}
}

//Should occur every time the resolution is changed.
void Scene::setCameraDims(int w, int h)
{
	_camera.w = w;
	_camera.h = h;

	_camera.x = getPlayerPosition().x - _camera.w / 2;
}

void Scene::movePlayer(bool left, bool right, bool down, bool up)
{
	float finalVel = 0.0f;

	if (_crosslink && (right || left || up || down))
	{
		toggleCrosslinkMode(); //can't move while in crosslink.
	}

	if (up && !down && (_player->getAttachType() == RightSide || _player->getAttachType() == LeftSide))
	{
		_player->setVelY(-PLAYERCLIMBSPEED);
	}
	else if (down && !up && (_player->getAttachType() == RightSide || _player->getAttachType() == LeftSide) && !_player->isAttachingDown())
	{
		_player->setVelY(PLAYERCLIMBDOWNSPEED);
	}
	else if (down && !up && _player->getAttachType() == Ceiling && !_moveDelay)
	{
		_player->detach();
	}
	else if (!down && !up && _player->getAttachType() != NotAttached)
	{
		_player->setVelY(0);
	}
	else if (down && !up && _player->getAttachType() == NotAttached && _player->isOnGround() && !(left || right))
	{
		//check if player is trying to attach to volume from the side.
		tryPlayerAttachSide();
	}

	//player can only move horizontally if attached to ceiling or on ground - once in flight, they cannot change direction.

	if (left xor right)
	{
		if (left)
		{
			//if left side of player is attached, can't move left into volume.
			_testAttach = RightSide;
			finalVel = -PLAYERSPEED;
		}
		else
		{
			//if right side of player is attached, can't move right into volume.
			_testAttach = LeftSide;
			finalVel = PLAYERSPEED;
		}

		if (_player->getAttachType() == Ceiling)
		{
			finalVel *= PLAYERCEILSPEEDMOD;
		}
		if (_player->isAimingGun())
		{
			finalVel *= PLAYERAIMINGSPEEDMOD;
		}

		if (_player->isOnGround() && !_player->isAccelerating())
		{
			if (_player->isPinning())
			{
				_player->releasePin();
			}
			if (_player->isAimingGun())
			{
				_player->setVelX(finalVel);
			}
			else
			{
				setAccel(_player->getAccelerationStruct(), true, left ? -PLAYERACCELERATION : PLAYERACCELERATION, finalVel);
			}
		}
		else if (_player->getAttachType() == Ceiling)
		{
			if (!_player->isAccelerating())
			{
				_player->setVelX(finalVel);
			}
		}
		else if (_player->getAttachType() == _testAttach && !_moveDelay)
		{
			_player->detach();
			_player->setVelX(finalVel);
		}
	}

	if (!right && !left)
	{
		if ((_player->isOnGround() || _player->getAttachType() == Ceiling) && _player->getAcceleration() == 0.0f)
			_player->setVelX(0);
	}

	_testAttach = NotAttached;
}

//called for as long as LMB is held down.
void Scene::handleClick(int mx, int my, unsigned int dT)
{
	//get the map coordinates of where the user clicked.
	float locX = (float)mx + _camera.x;
	float locY = (float)my + _camera.y;

	if (_crosslink)
	{
		//needed for drawing.
		_mouseDragPos.x = locX;
		_mouseDragPos.y = locY;

		if (_selecting) //user is already dragging a link, so don't bother searching.
			return;

		Rect collRect;

		_currentMap->getLinkableIters(&linkBegin, &linkEnd);
		LinkableEntity* le;

		//find if player clicked on any linkable entity.
		for (linkIter = linkBegin; linkIter != linkEnd; ++linkIter)
		{
			le = (*linkIter).get();
			collRect = le->getCollisionRect();
			if (vec2InRect(vec2f(locX, locY), collRect))
			{
				_linker = le;
				if (isCircuitUnlocked(_linker->getCircuitType()))
				{
					_linker->unlink();
					_selecting = true;
					break;
				}
				else
				{
					_linker = nullptr;
				}
			}
		}
	}
	else if (!_player->isInElevator() && (_player->isOnGround() || _player->getAttachType() != NotAttached)) //can only jump if not in crosslink.
	{
		if (_jumpPowerTimer > 0)
		{
			_jumpPowerTimer -= dT;
		}

		if (_jumpPowerTimer < 0)
		{
			_jumpPowerTimer = 0;
		}

		_jumpPower = ((float)(_timeToFullChargeJump - _jumpPowerTimer) / _timeToFullChargeJump);

		if (_player->isOnGround() || _player->getAttachType() != NotAttached)
			calculateJumpTrajectory(mx, my, dT);
	}
}

void Scene::crosslinkPan(int mouseX, int mouseY)
{
	if (!_crosslink) return;

	if (_camera.w - mouseX < CROSSLINKPANTRHESHOLD)
	{
		_camera.x += CROSSLINKPAN;
	}

	if (mouseX < CROSSLINKPANTRHESHOLD)
	{
		_camera.x -= CROSSLINKPAN;
	}

	if (_camera.h - mouseY < CROSSLINKPANTRHESHOLD)
	{
		_camera.y += CROSSLINKPAN;
	}

	if (mouseY < CROSSLINKPANTRHESHOLD)
	{
		_camera.y -= CROSSLINKPAN;
	}
}

void Scene::handleLeftClickRelease(int mx, int my)
{
	float locX = (float)mx + _camera.x;
	float locY = (float)my + _camera.y;

	if (_crosslink)
	{
		setNewLinkAt(mx, my);
	}
	else if (!_player->isInElevator() && !_player->isPinning() && !_player->isAimingGun() && !_player->isAnimatingThroughStairs() && !_player->isMovingThroughStairs())
	{
		playerJump();
		_stopTrajPoint = 0;
	}

	size_t i;
	Rect collRect;
	ElevatorSwitch* es;
	//find if player clicked on an elevator switch.
	if (_player->isInElevator())
	{
		for (i = 0; i < _currentMap->getNumberOfEnts(); i++)
		{
			if (dynamic_cast<ElevatorSwitch*>(_currentMap->getEntAt(i)))
			{
				es = static_cast<ElevatorSwitch*>(_currentMap->getEntAt(i));
				collRect = es->getCollisionRect();
				if (vec2InRect(vec2f(locX, locY), collRect))
				{
					if (es->getElevatorDoor() != _player->getElevatorDoor())
					{
						_player->getElevatorDoor()->getShaft()->setTarget(es->getElevatorDoor());
						break;
					}
				}
			}
		}
	}
}

//occurs when left mouse button is no longer held down.
void Scene::setNewLinkAt(int mx, int my)
{
	if (_linker == nullptr)
	{
		return;
	}

	_selecting = false;
	//get the map coordinates of where the user clicked.
	float locX = (float)mx + _camera.x;
	float locY = (float)my + _camera.y;
	Rect collRect;
	LinkableEntity* le;
	_currentMap->getLinkableIters(&linkBegin, &linkEnd);

	//find if player dragged pointer on any linkable entity.
	for (linkIter = linkBegin; linkIter != linkEnd; ++linkIter)
	{
		le = (*linkIter).get();

		collRect = le->getCollisionRect();
		if (vec2InRect(vec2f(locX, locY), collRect))
		{
			if (le == _linker)
			{
				return;
			}
			if (dynamic_cast<EnemyGun*>(_linker) || dynamic_cast<EnemyGun*>(le))
			{
				if (_playerEnergy < ENEMYGUNLINKCOST)
				{
					return;
				}
				else
				{
					_playerEnergy -= ENEMYGUNLINKCOST;
				}
			}
			_linker->link(le, true);
			_linker = nullptr;
			break;
		}
	}
}

void Scene::calculateJumpTrajectory(int x, int y, unsigned int dT)
{
	_stopTrajPoint = 0;

	AttachType at = _player->getAttachType();

	vec2f playerPos = _player->getCollisionRectCenterPosition();

	float distX = (float)x + _camera.x - playerPos.x;
	float distY = (float)y + _camera.y - playerPos.y;

	// float distMultX = fabs(distX / 100.0f);
	float distMultY = fabs(distY / 200.0f);

	if (distMultY > JUMPCLAMP)
		distMultY = JUMPCLAMP;

	_jumpImpulse = vec2f_normalize(vec2f(distX, distY));
	_jumpImpulse = _jumpImpulse * (_playerJumpPower * _jumpPower);

	// _jumpImpulse.x *= distMultX;
	_jumpImpulse.y *= distMultY;

	float iy = _jumpImpulse.y;
	vec2f point = playerPos + _jumpImpulse;

	_trajPoints[0] = point;
	size_t j;
	bool stop = false;

	//calculate trajectory of jump.
	for (int i = 0; i < 400; i++)
	{
		iy += GRAVITY;
		point.x += (_jumpImpulse.x * dT);
		point.y += (iy * dT);

		if (point.x < 0 || point.y < 0 || point.x > _currentMap->getMapWidth() || point.y > _currentMap->getMapHeight())
		{
			stop = true;
		}
		for (j = 0; j < _numCollideVols && !stop; j++)
		{
			if (vec2InRect(point, _currentMap->getCollideVolAt(j).rect) && _currentMap->getCollideVolAt(j).active)
			{
				stop = true; //this jump will collide with a collision vol. Stop tracing.
				break;
			}
		}
		if (stop)
		{
			_stopTrajPoint = i / 2;
			break;
		}
		if (i % 2 > 0) continue; //we don't need to show where the player will be at EVERY step of their jump.
		_trajPoints[i / 2] = point;
	}

	if ((at == RightSide && _jumpImpulse.x > 0) ||
	        (at == LeftSide && _jumpImpulse.x < 0) ||
	        (at == Ceiling && _jumpImpulse.y < 0))
	{
		_stopTrajPoint = 0;
	}
}

Door* Scene::getDoorOfCollisionVolume(CollisionVolume* vol)
{
	size_t i;
	for (i = 0; i < _currentMap->getNumberOfEnts(); i++)
	{
		if (dynamic_cast<Door*>(_currentMap->getEntAt(i)))
		{
			if (((Door*)_currentMap->getEntAt(i))->getCollisionVolume() == vol)
			{
				return static_cast<Door*>(_currentMap->getEntAt(i));
			}
		}
	}
	return nullptr;
}

//The lighting trace hit a collision volume. Using points before and after the trace collided, get the side of the volume that was hit.
void Scene::handleTraceHit(vec2f* oldv3interp, vec2f* v3interp, vec2f* c, CollisionVolume* volume)
{
	c->x = v3interp->x;
	c->y = v3interp->y;

	//which side of the volume's rect did we hit?
	if (oldv3interp->y < v3interp->y)
	{
		if (oldv3interp->x < volume->rect.x && v3interp->x >= volume->rect.x)
		{
			c->x = volume->rect.x;
		}
		else if (oldv3interp->x > volume->rect.x + volume->rect.w && v3interp->x <= volume->rect.x + volume->rect.w)
		{
			c->x = volume->rect.x + volume->rect.w;
		}
		else
		{
			c->y = volume->rect.y;
		}
	}
	else
	{
		if (oldv3interp->x < volume->rect.x && v3interp->x >= volume->rect.x)
		{
			c->x = volume->rect.x;
		}
		else if (oldv3interp->x > (volume->rect.x + volume->rect.w) && v3interp->x <= (volume->rect.x + volume->rect.w))
		{
			c->x = volume->rect.x + volume->rect.w;
		}
		else
		{
			c->y = volume->rect.y + volume->rect.h;
		}
	}
}

//the lighting trace did NOT hit any collision volume. Maybe it hit the map boundary?
bool Scene::handleTraceHitMapBounds(vec2f* v3interp, vec2f* c)
{
	if (v3interp->x < 0)
	{
		c->x = 0.0f;
		c->y = v3interp->y;
		return true;
	}

	if (v3interp->x > _currentMap->getMapWidth())
	{
		c->x = _currentMap->getMapWidth();
		c->y = v3interp->y;
		return true;
	}

	if (v3interp->y < 0)
	{
		c->x = v3interp->x;
		c->y = 0.0f;
		return true;
	}

	if (v3interp->y > _currentMap->getMapHeight())
	{
		c->x = v3interp->x;
		c->y = _currentMap->getMapHeight();
		return true;
	}
	return false;
}

void Scene::handlePlayerFrob(bool up)
{
	if (_player->isAlive())
	{
		if (_playerOverlappingEnts[UESwitch])
		{
			_switcher->activate();
		}
		if (_player->isOverlappingStairs())
		{
			_player->setStairMovement(up ? MovingUp : MovingDown);
		}
		if (_playerOverlappingEnts[UETerminal])
		{
			_player->setPosition(_computer->getPosition().x - 5, _player->getPosition().y);
			_player->hackTerminal(_computer);
		}
		if (_playerOverlappingEnts[UECircuitBox])
		{
			_circuitBox->setHacked(true, true);
			_circuitUnlocked[_circuitBox->getCircuit()] = true;
		}
		if (_playerOverlappingEnts[UEElevator] && !_player->isInElevator())
		{
			if (_elevator->isOpen())
			{
				_player->enterElevator(_elevator);
				_elevatorLocked = true;
			}
			else
			{
				//call elevator to this location.
				_elevator->getShaft()->setTarget(_elevator);
			}
		}
		if (_player->isInElevator() && !_player->getElevatorDoor()->getShaft()->isMoving() && !_elevatorLocked)
		{
			ElevatorDoor* door = _player->getElevatorDoor();
			ElevatorShaft* shaft = door->getShaft();
			shaft->setTarget(up ? shaft->getDoorAbove(door) : shaft->getDoorBelow(door));
		}
	}
}

void Scene::addNoise(int x, int y, int radius, bool alertGuards, AlertType atype, Alarm* alarm)
{
	//check which enemies hears first.
	//Only the closest enemy that hears will investigate.
	size_t i;
	int chosenEnemy = -1;
	float shortestDistance = radius;
	float currentDistance = 0;
	vec2f p = vec2f(x + _camera.x, y + _camera.y);
	Enemy* e;

	for (i = 0; i < _currentMap->getNumberOfEnemies(); i++)
	{
		e = _currentMap->getEnemyAt(i);
		if (alertGuards)
		{
			if (e->getState() != KNOCKED_OUT && e->getState() != FALLING && e->getType() != Enemy_Sniper)
			{
				currentDistance = vec2f_distance(p, e->getCollisionRectPosition());
				if (currentDistance <= radius)
				{
					if (atype == ALERT_LOOK)
					{
						//every guard in radius may look at this kind of alert.
						e->alertToPosition(p.x, p.y, atype, TARGET_NOISE);
					}

					if (currentDistance < shortestDistance)
					{
						shortestDistance = currentDistance;
						chosenEnemy = i;
					}
				}
			}
		}
	}

	for (i = 0; i < _currentMap->getNumberOfEnts(); i++)
	{
		if (dynamic_cast<SoundDetector*>(_currentMap->getEntAt(i)))
		{
			SoundDetector* sd = static_cast<SoundDetector*>(_currentMap->getEntAt(i));
			currentDistance = vec2f_distance(p, sd->getCollisionRectPosition());
			if (currentDistance <= radius)
			{
				sd->activate();
			}
		}
	}

	//alert them!
	if (chosenEnemy >= 0 && atype != ALERT_LOOK)
	{
		_currentMap->getEnemyAt(chosenEnemy)->alertToPosition(p.x, p.y, atype, alarm ? TARGET_ALARM : TARGET_NOISE);
		if (alarm)
		{
			_currentMap->getEnemyAt(chosenEnemy)->setLinkableTarget(alarm);
		}
	}
}

void Scene::playerJump()
{
	AttachType at = _player->getAttachType();

	_jumpPowerTimer = _timeToFullChargeJump - JUMPSTARTTIME;

	if (_player->isOnGround() || _player->getAttachType() != NotAttached)
	{
		if ((at == RightSide && _jumpImpulse.x > 0) ||
		        (at == LeftSide && _jumpImpulse.x < 0) ||
		        (at == Ceiling && _jumpImpulse.y < 0) ||
		        _player->isHacking())
		{
			return;
		}
		_player->jump(_jumpImpulse.x, _jumpImpulse.y);
	}
}

void Scene::warpPlayerTo(int mx, int my)
{
	_player->setCollisionRectPosition(mx + _camera.x, my + _camera.y);
	_player->setOnGround(false);
	_player->detach();
	_player->releasePin();
}

std::shared_ptr<Map> Scene::getMap()
{
	return _currentMap;
}

Player* Scene::getPlayer()
{
	return _player.get();
}

vec2f Scene::getPlayerPosition()
{
	return _player->getPosition();
}

vec2f Scene::getMouseDragPosition()
{
	return _mouseDragPos;
}

Rect Scene::getCamera()
{
	return _camera;
}

size_t Scene::getNumberOfTrajPoints()
{
	return _stopTrajPoint;
}

vec2f Scene::getTrajPointAt(int i)
{
	return _trajPoints[i];
}

bool Scene::inCrosslinkMode()
{
	return _crosslink;
}

void Scene::toggleCrosslinkMode()
{
	if (!_player->isAlive())
	{
		return;
	}

	size_t i;
	float r, g, b;
	r = g = b = 0;
	SecurityCamera* camera;
	FieldOfView* fov;
	_crosslink = !_crosslink;
	if (_crosslink)
	{
		Locator::getAudio()->playSound("enter_crosslink");
	}
	else
	{
		Locator::getAudio()->playSound("exit_crosslink");
	}

	for (i = 0; i < _currentMap->getNumberOfEnts(); i++)
	{
		if (dynamic_cast<SecurityCamera*>(_currentMap->getEntAt(i)))
		{
			camera = static_cast<SecurityCamera*>(_currentMap->getEntAt(i));
			fov = camera->getFOV();
			getCircuitColor(camera->getCircuitType(), r, g, b);

			if (!_crosslink)
			{
				if (camera->isTrespassed())
				{
					fov->setColors(0.5, 0.0, 0);
				}
				else
				{
					fov->setColors(0.5, 0.5, 0);
				}
			}
			else
			{
				if (isCircuitUnlocked(camera->getCircuitType()))
				{
					fov->setColors(r , g , b);
				}
				else
				{
					fov->setColors(0.25, 0.25, 0.25);
				}
			}
		}
	}
}

bool Scene::isPlayerSelecting()
{
	return _selecting;
}

void Scene::setPlayerSelecting(bool b)
{
	_selecting = b;
}

LinkableEntity* Scene::getLinker()
{
	return _linker;
}

void Scene::endLevel()
{
	_levelOver = true;
}

bool Scene::isLevelOver()
{
	return _levelOver;
}

bool Scene::isDetachmentDelayed()
{
	return _moveDelay;
}

void Scene::resetDelayDetachment()
{
	_moveDelay = false;
}

bool Scene::isElevatorLocked()
{
	return _elevatorLocked;
}

void Scene::resetElevatorLock()
{
	_elevatorLocked = false;
}

void Scene::computeVisibility(FieldOfView* fov)
{
	vec2f pos = fov->getPosition();
	float radius = fov->getRadius();
	int halfsize = fov->getSize();
	int direction = fov->getDirection();
	int i;

	vec2f vi;
	vec2f vn;
	for (i = direction - halfsize; i <= direction + halfsize; i++)
	{
		vi.x = radius * sinf(ToRadian(i)) + pos.x;
		vi.y = radius * cosf(ToRadian(i)) + pos.y;
		isTraceBlocked(nullptr, pos, vi, &vn, &_unused_index, &_unused_bool, 64.0f, &Scene::interpStandard);
		fov->addVertex(vn.x, vn.y);
	}
}

void Scene::updateVisibility(FieldOfView* fov, int angle1, int angle2)
{
	vec2f pos = fov->getPosition();
	float radius = fov->getRadius();
	int halfsize = fov->getSize();
	int direction = fov->getDirection();
	int i;

	vec2f vi;
	vec2f vn;
	int index;
	for (i = angle1; i <= angle2; i++)
	{
		vi.x = radius * sinf(ToRadian(i)) + pos.x;
		vi.y = radius * cosf(ToRadian(i)) + pos.y;
		isTraceBlocked(nullptr, pos, vi, &vn, &_unused_index, &_unused_bool, 64.0f, &Scene::interpStandard);

		index = i - direction + halfsize;
		if (index < 0)
		{
			index += 360;
		}

		fov->changeVertex((index) * 2 + 2, vn.x, vn.y);
	}
}

void Scene::updateOverlappingFOVs(Door* door)
{
	size_t i;
	int angle1;
	int angle2;
	FieldOfView* fov;
	for (i = 0; i < door->getNumberOfOverlappingLights(); i++)
	{
		fov = door->getLightAndAnglesAt(i, &angle1, &angle2);
		updateVisibility(fov, angle1, angle2);
	}
}

//converts angle from acos to how computeVisibility works.
float convertAngle(float angle, float x1, float y1, float x2, float y2)
{
	if (y1 > y2)
	{
		return 90 - angle;
	}
	else
	{
		if (x1 > x2)
		{
			return angle + 90;
		}
		else
		{
			return angle - 270;
		}
	}
}

void Scene::calculateOverlappingLightsOfDoor(Door* door)
{
	size_t i;
	FieldOfView* fov;
	CollisionVolume* vol = door->getCollisionVolume();
	vec2f v1 = vec2f(1, 0);
	vec2f lightPos;
	float angleTopLeft;
	float angleTopRight;
	float angleBottomLeft;
	float angleBottomRight;
	int angle1, angle2;

	for (i = 0; i < _currentMap->getNumberOfLights(); i++)
	{
		fov = _currentMap->getLightAt(i);
		lightPos = fov->getPosition();

		if (rectInCircle(vol->rect, fov->getPosition(), fov->getRadius()))
		{
			angleTopLeft = ToDegree(acos(v1.dot(vec2f_normalize(vec2f(vol->rect.x - lightPos.x, vol->rect.y - lightPos.y)))));
			angleTopRight = ToDegree(acos(v1.dot(vec2f_normalize(vec2f(vol->rect.x + vol->rect.w - lightPos.x, vol->rect.y - lightPos.y)))));
			angleBottomLeft = ToDegree(acos(v1.dot(vec2f_normalize(vec2f(vol->rect.x - lightPos.x, vol->rect.y + vol->rect.h - lightPos.y)))));
			angleBottomRight = ToDegree(acos(v1.dot(vec2f_normalize(vec2f(vol->rect.x + vol->rect.w - lightPos.x, vol->rect.y + vol->rect.h - lightPos.y)))));

			angleTopLeft = convertAngle(angleTopLeft, vol->rect.x, vol->rect.y, lightPos.x, lightPos.y);
			angleTopRight = convertAngle(angleTopRight, vol->rect.x + vol->rect.w, vol->rect.y, lightPos.x, lightPos.y);
			angleBottomLeft = convertAngle(angleBottomLeft, vol->rect.x, vol->rect.y + vol->rect.h, lightPos.x, lightPos.y);
			angleBottomRight = convertAngle(angleBottomRight, vol->rect.x + vol->rect.w, vol->rect.y + vol->rect.h, lightPos.x, lightPos.y);

			angle1 = (int)std::min(std::min(angleTopRight, angleBottomRight), std::min(angleTopLeft, angleBottomLeft));
			angle2 = (int)std::max(std::max(angleTopRight, angleBottomRight), std::max(angleTopLeft, angleBottomLeft));

			if (angle1 < -90 && angle2 > 90)
			{
				//Area spans 1st and 2nd quadrant, but need to set angles correctly.
				door->addOverlappingLight(fov, angle2 - 1, 180);
				door->addOverlappingLight(fov, -180, angle1 + 2);
			}
			else
			{
				door->addOverlappingLight(fov, angle1, angle2);
			}
		}
	}
}

void Scene::calculatePlayerVisibility()
{
	if (_player->isInElevator())
	{
		_player->setLightVisibility(0);
		return;
	}

	int vis = 0;
	size_t i;
	vec2f pos = _player->getCollisionRectCenterPosition();
	float radius = 0;
	vec2f lightPos;
	float dist = 0;
	int prevLight = _player->getLightVisibility();

	for (i = 0; i < _currentMap->getNumberOfLights(); i++)
	{
		if (_currentMap->getLightAt(i)->isActive() && _currentMap->getLightAt(i)->getType() == FOV_LIGHT)
		{
			if (isPlayerInFOV(_currentMap->getLightAt(i)))
			{
				lightPos = _currentMap->getLightAt(i)->getPosition();
				radius = _currentMap->getLightAt(i)->getRadius();
				dist = vec2f_distance(pos, lightPos);
				float attenuation = 100 - ((dist / radius) * 100);
				vis = vis + (int)attenuation;
			}
		}
	}

	if (vis > 100)
		vis = 100;

	if (prevLight < PLAYER_LIGHT_THRESHOLD && vis >= PLAYER_LIGHT_THRESHOLD)
	{
		_lightEnteredAlpha = 0.3f;
	}
	_player->setLightVisibility(vis);
}

bool Scene::isPlayerInFOV(FieldOfView* fov)
{
	vec2f lightPos = fov->getPosition();
	float radius = fov->getRadius();

	vec2f pos = _player->getCollisionRectCenterPosition();
	float dist = vec2f_distance(pos, lightPos);
	int direction = fov->getDirection();
	int halfsize = fov->getSize();

	vec2f v1 = vec2f(sinf(ToRadian(direction)), cosf(ToRadian(direction)));
	vec2f v2 = vec2f_normalize(vec2f(pos.x - lightPos.x, pos.y - lightPos.y));
	float testAngle = acos(v1.dot(v2));

	return 	dist < radius &&
	        !isTraceBlocked(nullptr, pos, lightPos, &_unused_c, &_unused_index, &_unused_bool, 16.0f, &Scene::interpStandard) &&
	        testAngle < ToRadian(halfsize);
}

void Scene::calculateStrongestLight(Enemy* enemy)
{
	size_t i;
	int strongestLight = -1;
	float strongestAttenuation = 0;
	vec2f pos = enemy->getCollisionRectCenterPosition();
	float dist = 0.0f;
	vec2f lightPos;
	FieldOfView* fov;

	for (i = 0; i < _currentMap->getNumberOfLights(); i++)
	{
		fov = _currentMap->getLightAt(i);

		if (fov->getType() != FOV_LIGHT)
		{
			continue;
		}

		if (fov->isActive())
		{
			lightPos = fov->getPosition();
			dist = vec2f_distance(pos, lightPos);
			if (dist < fov->getRadius() && !isTraceBlocked(nullptr, pos, lightPos, &_unused_c, &_unused_index, &_unused_bool, 64.0f, &Scene::interpStandard))
			{
				float attenuation = 100 - ((dist / fov->getRadius()) * 100);
				if (attenuation > strongestAttenuation)
				{
					strongestAttenuation = attenuation;
					strongestLight = i;
				}
			}
		}
		else
		{
			if (enemy->getStrongestLight() == fov)
			{
				//Light just turned off - find a switch.
				enemy->loseStrongestLight();
				handleEnemyLightOff(enemy);
			}
		}
	}

	if (strongestLight >= 0 && strongestAttenuation > 30)
	{
		enemy->setStrongestLight(_currentMap->getLightAt(strongestLight));
	}
	else
	{
		enemy->setStrongestLight(nullptr);
	}
}

//At map load, make initial links between any linkable entities and lights at ends of polylines.
void Scene::makeInitialLinks()
{
	int i;
	int size = _currentMap->getNumberOfLines();
	for (i = 0; i < size; i++)
	{
		attemptLinkBetween(_currentMap->getLineAt(i).p1, _currentMap->getLineAt(i).p2);
	}

	size = _currentMap->getNumberOfLightLinks();
	for (i = 0; i < size; i++)
	{
		attemptLightLinkBetween(_currentMap->getLightLinkAt(i).p1, _currentMap->getLightLinkAt(i).p2);
	}
}

//find two linkable entities whose bounding boxes hit ends of line. If found, link from p1 to p2.
void Scene::attemptLinkBetween(vec2f p1, vec2f p2)
{
	LinkableEntity* le1 = nullptr;
	LinkableEntity* le2 = nullptr;
	LinkableEntity* le;
	_currentMap->getLinkableIters(&linkBegin, &linkEnd);
	for (linkIter = linkBegin; linkIter != linkEnd; ++linkIter)
	{
		le = (*linkIter).get();
		if (vec2InRect(p1, le->getCollisionRect()) && le1 == nullptr)
		{
			le1 = le;

		}
		if (vec2InRect(p2, le->getCollisionRect()) && le2 == nullptr)
		{
			le2 = le;
		}

		if (le1 != nullptr && le2 != nullptr)
		{
			break;
		}
	}

	if (le1 != nullptr && le2 != nullptr)
	{
		le1->link(le2, false);
	}
}

void Scene::attemptLightLinkBetween(vec2f p1, vec2f p2)
{
	LightFixture* fixture = nullptr;
	FieldOfView* fov = nullptr;
	size_t i;

	for (i = 0; i < _currentMap->getNumberOfEnts(); i++)
	{
		if (dynamic_cast<LightFixture*>(_currentMap->getEntAt(i)))
		{
			if (vec2InRect(p1, _currentMap->getEntAt(i)->getCollisionRect()))
			{
				fixture = static_cast<LightFixture*>(_currentMap->getEntAt(i));
				break;
			}
		}
	}

	for (i = 0; i < _currentMap->getNumberOfLights(); i++)
	{
		if (_currentMap->getLightAt(i)->getType() == FOV_LIGHT)
		{
			if (vec2InRect(p2, _currentMap->getLightAt(i)->getCollisionRect()))
			{
				fov = _currentMap->getLightAt(i);
				break;
			}
		}
	}

	if (fixture != nullptr && fov != nullptr)
	{
		fixture->addFOV(fov);
	}
}

bool Scene::isCircuitUnlocked(Circuit c)
{
	return c == RED ? true : _circuitUnlocked[c];
}

void Scene::getCircuitColor(Circuit c, float &r, float &g, float &b)
{
	switch (c)
	{
	case RED:
		r = 1.0f;
		g = 0.0f;
		b = 0.0f;
		break;
	case BLUE:
		r = 0.0f;
		g = 0.0f;
		b = 1.0f;
		break;
	case GREEN:
		r = 0.0f;
		g = 1.0f;
		b = 0.0f;
		break;
	case VIOLET:
		r = 0.58f;
		g = 0.0f;
		b = 0.83f;
		break;
	case YELLOW:
		r = 1.0f;
		g = 1.0f;
		b = 0.0f;
		break;
	}
}

bool Scene::isPlayerInLight()
{
	return _player->getLightVisibility() >= PLAYER_LIGHT_THRESHOLD;
}

void Scene::addPlayerJumpPower(float f)
{
	_playerJumpPower = BASE_PLAYER_JUMP_POWER + f;
}

void Scene::subPlayerJumpChargeTime(int dec)
{
	_timeToFullChargeJump = BASE_TIME_TO_FULL_JUMP - dec;
	_jumpPowerTimer = _timeToFullChargeJump - JUMPSTARTTIME;
}

Particle* Scene::getParticleAt(size_t i)
{
	return _particles[i].get();
}

size_t Scene::getNumberOfParticles()
{
	return _particles.size();
}

void Scene::addParticle(float x, float y, float vx, float vy)
{
	size_t i;
	Particle* p;
	for (i = 0; i < _particles.size(); i++)
	{
		p = getParticleAt(i);
		if (!p->isAlive())
		{
			p->setAlive(true);
			p->setPosition(x, y);
			p->setVelocity(vx, vy);
			return;
		}
	}

	p = getParticleAt(0);
	p->setPosition(x, y);
	p->setVelocity(vx, vy);
}

void Scene::handleVaultDoorClose(Door* door)
{
	size_t i;
	Entity* ent;
	for (i = 0; i < _currentMap->getNumberOfEnts(); i++)
	{
		ent = _currentMap->getEntAt(i);
		if (dynamic_cast<Enemy*>(ent) && check_collision(ent->getCollisionRect(), door->getCollisionVolume()->rect))
		{
			(static_cast<Enemy*>(ent))->changeState(KNOCKED_OUT);
		}
	}

	if (check_collision(_player->getCollisionRect(), door->getCollisionVolume()->rect))
	{
		_player->die();
		_stopTrajPoint = 0;
		_loadMenuVisible = true;
	}
}

void Scene::handleMouse(unsigned int mx, unsigned int my)
{
	_mousedOverObject = MO_Nothing;
	size_t i;
	Entity* ent;
	bool shouldHighlight = false;

	if (_player->isAimingGun())
	{
		bool right = _player->getDirection() == Right;
		float xDiff = _player->getCollisionRectCenterPosition().x - mx - _camera.x;
		float yDiff = _player->getCollisionRectCenterPosition().y - my - _camera.y;

		vec2f v2 = vec2f(xDiff, yDiff);

		float angle = acos(vec2f(_player->getDirection() == Right ? -1 : 1, 0).dot(vec2f_normalize(v2)));
		angle = ToDegree(angle);
		if ((yDiff > 0 && right) || (yDiff < 0 && !right))
		{
			angle = 0 - angle;
		}
		if ((xDiff > 0 && right) || (xDiff < 0 && !right))
		{
			_player->reverseDirection();
		}

		_player->setArmRotation(angle);
		traceBullet(_player.get(), vec2f(mx + _camera.x, my + _camera.y), Shot_FromPlayer, false);
	}

	for (i = 0; i < _currentMap->getNumberOfEnts(); i++)
	{
		ent = _currentMap->getEntAt(i);
		ent->setHighlighted(false);
		if (vec2InRect(vec2f(mx + _camera.x, my + _camera.y), ent->getCollisionRect()))
		{
			if (dynamic_cast<LightSwitch*>(ent))
			{
				_mousedOverObject = static_cast<LightSwitch*>(ent)->isHandScanner() ? MO_HandScanner : MO_Switch;
				shouldHighlight = true;
				break;
			}
			else if (dynamic_cast<MotionScanner*>(ent))
			{
				_mousedOverObject = MO_MotionScanner;
				shouldHighlight = true;
				break;
			}
			else if (dynamic_cast<ElevatorSwitch*>(ent))
			{
				_mousedOverObject = MO_Elevator;
				shouldHighlight = true;
				break;
			}
			else if (dynamic_cast<LightFixture*>(ent))
			{
				_mousedOverObject = MO_LightFixture;
				shouldHighlight = true;
				break;
			}
			else if (dynamic_cast<Door*>(ent))
			{
				switch(static_cast<Door*>(ent)->getType())
				{
				case Door_Normal:
					_mousedOverObject = MO_Door;
					break;
				case Door_Trap:
					_mousedOverObject = MO_TrapDoor;
					break;
				case Door_Vault:
					_mousedOverObject = MO_VaultDoor;
					break;
				}
				shouldHighlight = true;
				break;
			}
			else if (dynamic_cast<SoundDetector*>(ent))
			{
				_mousedOverObject = MO_SoundDetector;
				shouldHighlight = true;
				break;
			}
			else if (dynamic_cast<Alarm*>(ent))
			{
				_mousedOverObject = MO_Alarm;
				shouldHighlight = true;
				break;
			}
			else if (dynamic_cast<PowerSocket*>(ent))
			{
				_mousedOverObject = MO_PowerSocket;
				shouldHighlight = true;
				break;
			}
			else if (dynamic_cast<SecurityCamera*>(ent))
			{
				_mousedOverObject = MO_SecurityCamera;
				shouldHighlight = true;
				break;
			}
			else if (dynamic_cast<CircuitBox*>(ent))
			{
				_mousedOverObject = MO_CircuitBox;
				shouldHighlight = true;
				break;
			}
			else if (dynamic_cast<MainComputer*>(ent))
			{
				_mousedOverObject = MO_MainComputer;
				shouldHighlight = true;
				break;
			}
			else if (dynamic_cast<Enemy*>(ent))
			{
				switch(static_cast<Enemy*>(ent)->getType())
				{
				case Enemy_Guard:
					_mousedOverObject = MO_Guard;
					break;
				case Enemy_Enforcer:
					_mousedOverObject = MO_Enforcer;
					break;
				case Enemy_Professional:
					_mousedOverObject = MO_Professional;
					break;
				case Enemy_Sniper:
					_mousedOverObject = MO_Sniper;
					break;
				}
				shouldHighlight = true;
				break;
			}
		}
	}

	if (shouldHighlight)
	{
		_currentMap->getEntAt(i)->setHighlighted(true);
	}
}

MouseOverObject Scene::getObjectMousedOver()
{
	return _mousedOverObject;
}

float Scene::getLightEnteredAlpha()
{
	return _lightEnteredAlpha;
}

float Scene::getInputPopupAlpha()
{
	return _inputPopupAlpha;
}

vec2f Scene::getLaserEnd()
{
	return _laserEnd;
}

bool Scene::hasPlayerFiredShot()
{
	return _playerShotFired;
}

int Scene::getTimeToSniper()
{
	return _timeToSniper / 1000;
}

unsigned int Scene::getNumPlayerBullets()
{
	return _numPlayerBullets;
}

unsigned int Scene::getPlayerEnergy()
{
	return _playerEnergy;
}

void Scene::setTimeToSniper(int time)
{
	_timeToSniper = time;
}

void Scene::setNumPlayerBullets(unsigned int bullets)
{
	_numPlayerBullets = bullets;
}

void Scene::setPlayerEnergy(unsigned int energy)
{
	_playerEnergy = energy;
}

int Scene::getStringMessageTime()
{
	return _stringMessageTimer;
}

StringMessage Scene::getStringMessage()
{
	return _stringMessage;
}

UsableEnts Scene::getFirstOverlappedEnt()
{
	if (_playerOverlappingEnts[UEEnemy])
	{
		if (_player->getNumPunches() == 0)
		{
			return UEEnemy;
		}
		else
		{
			return UEEnemyKnockedOut;
		}
	}
	if (_playerOverlappingEnts[UESwitch])
	{
		return UESwitch;
	}
	if (_playerOverlappingEnts[UETerminal])
	{
		return UETerminal;
	}
	if (_playerOverlappingEnts[UEElevator])
	{
		return UEElevator;
	}
	if (_playerOverlappingEnts[UECircuitBox])
	{
		return UECircuitBox;
	}
	if (_playerOverlappingEnts[UEStairs])
	{
		return UEStairs;
	}
	return NumUsableEnts;
}