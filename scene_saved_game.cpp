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

#define SAVE_INTERVAL_MS 3000

void Scene::loadGame(const char* filename)
{
	FILE* input = fopen(filename, "rb");
	int i;
	SavedGameHeader header;
	Enemy* enemy;
	Door* door;
	LightFixture* lt;
	MainComputer* mc;
	CollisionVolume* cvol;
	CircuitBox* cb;
	Alarm* alarm;
	ElevatorDoor* ed;

	if (!input)
	{
		LOGF((stderr, "Failed to open save game %s!\n", filename));
		return;
	}

	fread(&header, sizeof(header), 1, input);

	_playerShotFired = header.playerFiredShot;
	_timeToSniper = header.timeToSniper;
	_numPlayerBullets = header.numPlayerBullets;
	_playerEnergy = header.playerEnergy;

	std::vector<SavedGameLink> links(header.numLinks);
	std::vector<SavedGameEntityState> states(header.numEnts);

	fread(links.data(), sizeof(SavedGameLink) * header.numLinks, 1, input);
	fread(states.data(), sizeof(SavedGameEntityState) * header.numEnts, 1, input);

	fclose(input);

	_currentMap->addMissingGuns();

	_currentMap->clearLinks();
	loadMap(header.filename, true);

	if (_timeToSniper > 0)
	{
		_currentMap->removeSniper();
	}
	else
	{
		_currentMap->addSniper();
	}

	for (i = 0; i < header.numEnts; i++)
	{
		switch (states[i].type)
		{
		case SGET_Enemy:
			Assert(dynamic_cast<Enemy*>(_currentMap->getEntAt(states[i].index3)));
			enemy = static_cast<Enemy*>(_currentMap->getEntAt(states[i].index3));
			enemy->setDirection(states[i].dir);
			enemy->setPosition(states[i].position.x, states[i].position.y);
			enemy->setVelocity(states[i].velocity.x, states[i].velocity.y);
			enemy->setAlive(states[i].alive);
			enemy->setCanSeePlayer(states[i].b2);

			enemy->setReactionTime(states[i].index4);
			enemy->setReadyToShoot(false);

			enemy->setWaitingForAlert(states[i].b1);

			if (states[i].index1 >= 0)
			{
				enemy->setLinkableTarget((LinkableEntity*)_currentMap->getEntAt(states[i].index1));
			}
			if (states[i].index2 >= 0)
			{
				enemy->setLightToActivate((FieldOfView*)_currentMap->getLightAt(states[i].index2));
			}
			if (states[i].targetType != TARGET_NONE)
			{
				enemy->alertToPosition(states[i].targetLocation.x, states[i].targetLocation.y, states[i].alertType, states[i].targetType);
			}
			else
			{
				enemy->forgetTarget();
			}
			if (checkIfEntOnGround(enemy))
			{
				enemy->setIgnoreFall(false);
			}
			enemy->setSecondaryTarget(states[i].index5 >= 0 ? _currentMap->getEntAt(states[i].index5) : nullptr);
			calculateStrongestLight(enemy); //placed here to prevent idle guard from patrolling if game was saved with activated light and was loaded while it was deactivated.
			enemy->changeState(states[i].gs);
			enemy->setResolve(states[i].esr.timeSinceShot, states[i].esr.shotType);
			enemy->update(0); //placed here to move the enemy's gun accordingly so that links with them will restore correctly.
			enemy->setSprite();
			break;
		case SGET_Player:
			_player->setDirection(states[i].dir);
			_player->setOnGround(states[i].onGround);
			_player->setPosition(states[i].position.x, states[i].position.y);
			_player->setVelocity(states[i].velocity.x, states[i].velocity.y);
			if (states[i].index1 >= 0)
			{
				_player->attachToVolume(_currentMap->getCollideVolPointerAt(states[i].index1), states[i].at);
			}
			else
			{
				_player->detach();
			}
			if (states[i].index2 >= 0)
			{
				_player->enterElevator((ElevatorDoor*)_currentMap->getEntAt(states[i].index2));
			}
			else
			{
				_player->leaveElevator();
			}
			_player->setNumTerminalsHacked(states[i].index3);
			break;
		case SGET_Objective:
			Assert(dynamic_cast<MainComputer*>(_currentMap->getEntAt(states[i].index1)));
			mc = static_cast<MainComputer*>(_currentMap->getEntAt(states[i].index1));
			mc->setActive(states[i].b1);
			break;
		case SGET_Door:
			Assert(dynamic_cast<Door*>(_currentMap->getEntAt(states[i].index1)));
			door = static_cast<Door*>(_currentMap->getEntAt(states[i].index1));
			if (states[i].b1 && !door->isOpened())
			{
				door->open();
			}
			if (!states[i].b1 && door->isOpened())
			{
				door->close();
			}
			door->updateCollisionVolume();
			break;
		case SGET_LightFixture:
			Assert(dynamic_cast<LightFixture*>(_currentMap->getEntAt(states[i].index1)));
			lt = static_cast<LightFixture*>(_currentMap->getEntAt(states[i].index1));
			if (states[i].b1 xor lt->isSwitchedOn())
			{
				lt->setSwitchedOn(!lt->isSwitchedOn());
			}
			break;
		case SGET_Glass:
			cvol = _currentMap->getCollideVolPointerAt(states[i].index1);
			Assert(cvol->glass());
			if (states[i].b1)
			{
				cvol->flags |= COLLISION_ACTIVE;
			}
			else
			{
				cvol->flags &= ~COLLISION_ACTIVE;
			}
			break;
		case SGET_CircuitBox:
			cb = static_cast<CircuitBox*>(_currentMap->getEntAt(states[i].index1));
			cb->setHacked(states[i].b1, false);
			_circuitUnlocked[cb->getCircuit()] = states[i].b1;
			break;
		case SGET_Alarm:
			alarm = static_cast<Alarm*>(_currentMap->getEntAt(states[i].index1));
			alarm->setAnimating(states[i].b1);
			break;
		case SGET_ElevatorDoor:
			ed = static_cast<ElevatorDoor*>(_currentMap->getEntAt(states[i].index1));
			if (states[i].b1)
			{
				ed->getShaft()->setOpenDoor(ed, false);
			}
			break;
		default:
			break;
		}
	}

	for (i = 0; i < header.numLinks; i++)
	{
		_currentMap->addSavedLink(links[i].start, links[i].end);
	}

	makeInitialLinks();
}

void Scene::saveGame(const char* filename)
{
	FILE* output = fopen(filename, "wb");
	SavedGameHeader header;
	std::vector<SavedGameLink> links;
	std::vector<SavedGameEntityState> states;

	strcpy(header.filename, _mapFilename);

	getLinks(&links);
	header.numLinks = links.size();

	//SGESS
	getSGESs(&states);
	SavedGameEntityState player_SGES;
	player_SGES.type = SGET_Player;
	player_SGES.position = _player->getPosition();
	player_SGES.velocity = _player->getVelocity();
	player_SGES.dir = _player->getDirection();
	player_SGES.at = _player->getAttachType();
	player_SGES.index1 = _currentMap->indexOfCollideVol(_player->getAttachedVolume());
	player_SGES.index2 = _currentMap->indexOfEntity(_player->getElevatorDoor());
	player_SGES.index3 = _player->getNumHackedTerminals();
	player_SGES.onGround = _player->isOnGround();
	states.push_back(player_SGES);

	header.numEnts = states.size();

	header.playerFiredShot = _playerShotFired;
	header.timeToSniper = _timeToSniper;
	header.numPlayerBullets = _numPlayerBullets;
	header.playerEnergy = _playerEnergy;

	//Write
	fwrite(&header, sizeof(header), 1, output);
	fwrite(links.data(), sizeof(SavedGameLink) * header.numLinks, 1, output);
	fwrite(states.data(), sizeof(SavedGameEntityState) * header.numEnts, 1, output);
	fclose(output);

	links.clear();
	states.clear();
}

void Scene::getLinks(std::vector<SavedGameLink>* container)
{
	LinkableEntity* curr;
	LinkableEntity* target;
	SavedGameLink sgl;
	std::vector<std::shared_ptr<LinkableEntity> >::iterator begin;
	std::vector<std::shared_ptr<LinkableEntity> >::iterator end;
	std::vector<std::shared_ptr<LinkableEntity> >::iterator i;
	_currentMap->getLinkableIters(&begin, &end);
	for (i = begin; i != end; ++i)
	{
		curr = (*i).get();
		target = curr->getTarget();
		if (target != nullptr)
		{
			sgl.start = curr->getCollisionRectPosition();
			sgl.end = target->getCollisionRectPosition();
			container->push_back(sgl);
		}
	}
}

void Scene::getSGESs(std::vector<SavedGameEntityState>* container)
{
	unsigned int i;
	SavedGameEntityState sges;
	Entity* ent;
	CollisionVolume* vol;
	LivingEntity* le;
	Door* door;
	MainComputer* mc;
	CircuitBox* cb;
	Enemy* enemy;
	LightFixture* lt;
	Alarm* alarm;
	ElevatorDoor* ed;

	for (i = 0; i < _currentMap->getNumberOfEnts(); i++)
	{
		ent = _currentMap->getEntAt(i);

		sges.type = SGET_Unknown;
		sges.position = ent->getPosition();
		sges.velocity = ent->getVelocity();

		if (dynamic_cast<LivingEntity*>(ent))
		{
			le = static_cast<LivingEntity*>(ent);
			sges.dir = le->getDirection();
			sges.stairTimer = le->getStairTimer();
			sges.onGround = le->isOnGround();
			sges.alive = le->isAlive();
		}

		if (dynamic_cast<Door*>(ent))
		{
			door = static_cast<Door*>(ent);
			sges.type = SGET_Door;
			sges.index1 = i;
			sges.index2 = 0;
			sges.index3 = 0;
			sges.index4 = 0;
			sges.b1 = door->isOpened();
			sges.timeToClose = door->getTimeToClose();
		}

		if (dynamic_cast<MainComputer*>(ent))
		{
			mc = static_cast<MainComputer*>(ent);
			sges.type = SGET_Objective;
			sges.b1 = mc->isActive();
			sges.index1 = i;
			sges.index2 = 0;
			sges.index3 = 0;
			sges.index4 = 0;
		}

		if (dynamic_cast<CircuitBox*>(ent))
		{
			cb = static_cast<CircuitBox*>(ent);
			sges.type = SGET_CircuitBox;
			sges.b1 = cb->isHacked();
			sges.index1 = i;
			sges.index2 = 0;
			sges.index3 = 0;
			sges.index4 = 0;
		}

		if (dynamic_cast<Enemy*>(ent))
		{
			enemy = static_cast<Enemy*>(ent);
			sges.type = SGET_Enemy;
			sges.gs = enemy->getState();
			sges.targetType = enemy->getTargetType();
			sges.alertType = enemy->getAlertType();
			sges.targetLocation = enemy->getTarget();
			sges.index1 = _currentMap->indexOfEntity(enemy->getTargetSwitch());
			sges.index2 = _currentMap->indexOfFOV(enemy->getLightToActivate());
			sges.index3 = i;
			sges.index4 = enemy->getReactionTime();
			sges.index5 = _currentMap->indexOfEntity(enemy->getSecondaryTarget());
			sges.b1 = enemy->isWaitingForAlert();
			sges.b2 = enemy->canSeePlayer();
			sges.esr = enemy->getResolve();
		}

		if (dynamic_cast<LightFixture*>(ent))
		{
			lt = static_cast<LightFixture*>(ent);
			sges.type = SGET_LightFixture;
			sges.b1 = lt->isSwitchedOn();
			sges.index1 = i;
			sges.index2 = 0;
			sges.index3 = 0;
			sges.index4 = 0;
		}

		if (dynamic_cast<Alarm*>(ent))
		{
			alarm = static_cast<Alarm*>(ent);
			sges.type = SGET_Alarm;
			sges.b1 = alarm->isActivated();
			sges.index1 = i;
			sges.index2 = 0;
			sges.index3 = 0;
			sges.index4 = 0;
		}

		if (dynamic_cast<ElevatorDoor*>(ent))
		{
			ed = static_cast<ElevatorDoor*>(ent);
			sges.type = SGET_ElevatorDoor;
			sges.b1 = ed->isOpen();
			sges.index1 = i;
		}

		container->push_back(sges);
	}

	for (i = 0; i < _currentMap->getNumberOfCollideVols(); i++)
	{
		vol = _currentMap->getCollideVolPointerAt(i);

		if (vol->glass())
		{
			sges.type = SGET_Glass;
			sges.b1 = vol->active();
			sges.index1 = i;
			container->push_back(sges);
		}
	}
}

void Scene::updateSaves(unsigned int dT)
{
	if (_loadMenuVisible ||
	        (!_player->isOnGround() && _player->getAttachType() == NotAttached) ||
	        !_player->isAlive())
		return;

	_saveTimer += dT;

	if (_saveTimer > SAVE_INTERVAL_MS)
	{
		setTimeAt(_currentSave, _saveTimer);

		_saveTimer = 0;

		sprintf(_saveFilename, "auto%i.sav", _currentSave);
		saveGame(_saveFilename);

		_currentSave++;

		if (_currentSave >= MAX_SAVES)
		{
			_currentSave = 0;
		}
	}
}

bool Scene::isLoadMenuVisible()
{
	return _loadMenuVisible;
}

void Scene::showLoadMenu(bool b)
{
	_loadMenuVisible = b;
}

int Scene::getSaveTimeAt(unsigned int index)
{
	return _saveTimeSince[index];
}

void Scene::loadAutosave(unsigned int index)
{
	unsigned int currIndex = index;
	unsigned int i;
	if (_saveTimeSince[currIndex] == -1)
	{
		LOGF((stdout, "Can't load game in future!\n"));
		return;
	}

	sprintf(_saveFilename, "auto%i.sav", currIndex);

	//invalidate any saves made in the "future".
	for (i = 0; i < MAX_SAVES - 1; i++)
	{
		currIndex = currIndex == MAX_SAVES - 1 ? 0 : currIndex + 1;
		if (_saveTimeSince[currIndex] < _saveTimeSince[index])
		{
			_saveTimeSince[currIndex] = -1;
		}
		if (_saveTimeSince[currIndex] > _saveTimeSince[index])
		{
			_saveTimeSince[currIndex] -= _saveTimeSince[index];
		}
	}

	_saveTimeSince[index] = 0; //this auto save was just loaded, so set saved time to 0.

	loadGame(_saveFilename);
}

void Scene::setTimeAt(unsigned int index, unsigned int time)
{
	unsigned int currIndex = index;
	unsigned int i;

	_saveTimeSince[currIndex] = 0;
	for (i = 0; i < MAX_SAVES - 1; i++)
	{
		currIndex = currIndex == 0 ? MAX_SAVES - 1 : currIndex - 1;
		if (_saveTimeSince[currIndex] != -1)
		{
			_saveTimeSince[currIndex] += time;
		}
	}
}

int Scene::getSecondsSince(unsigned int index)
{
	int milliseconds = _saveTimer + _saveTimeSince[index];
	return milliseconds / 1000;
}