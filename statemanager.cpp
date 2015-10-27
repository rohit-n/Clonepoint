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

#include "tinyxml/tinyxml.h"
#include "statemanager.h"
#include "gamestate.h"
#include "mainmenustate.h"
#include "creditsstate.h"
#include "levelendstate.h"
#include "pausestate.h"
#include "loadmapstate.h"
#include "optionsstate.h"
#include "upgradesstate.h"

StateManager::StateManager()
{
	_gameState = new GameState(this);
	_mainMenuState = new MainMenuState(this);
	_creditsState = new CreditsState(this);
	_levelEndState = new LevelEndState(this);
	_pauseState = new PauseState(this);
	_loadMapState = new LoadMapState(this);
	_optionsState = new OptionsState(this);
	_upgradesState = new UpgradesState(this);
	_activeState = _mainMenuState;
	_winX = 800;
	_winY = 600;
	_settingsChanged = false;
	_activeMapFilename = "";
}

StateManager::~StateManager()
{
	LOGF((stdout, "Running Statemanager destructor!\n"));
	_activeState = NULL;
	delete _gameState;
	delete _mainMenuState;
	delete _creditsState;
	delete _levelEndState;
	delete _pauseState;
	delete _loadMapState;
	delete _optionsState;
	delete _upgradesState;
}

BaseState* StateManager::getGameState()
{
	return _gameState;
}

BaseState* StateManager::getActiveState()
{
	return _activeState;
}

void StateManager::switchToState(eState state)
{
	GameState* gs = static_cast<GameState*>(_gameState);
	UpgradesState* us = static_cast<UpgradesState*>(_upgradesState);
	int jp, jt, timeToSniper, old_x, old_y;
	unsigned int ammo, energy;
	_activeState->getMousePosition(&old_x, &old_y);

	switch(state)
	{
	case MAINMENU_SCREEN:
		_activeState = _mainMenuState;
		break;
	case GAME_SCREEN:
		if (_activeState == _upgradesState)
		{
			us->getModifiers(&jp, &jt, &ammo, &timeToSniper, &energy);
			gs->getScene()->addPlayerJumpPower((float)jp / 50.0f);
			gs->getScene()->subPlayerJumpChargeTime(jt * 90);
			gs->getScene()->_numPlayerBullets = ammo;
			gs->getScene()->_timeToSniper = (timeToSniper * 1000);
			gs->getScene()->_playerEnergy = energy;
		}
		_activeState = _gameState;
		break;
	case CREDITS_SCREEN:
		_activeState = _creditsState;
		break;
	case LEVELEND_SCREEN:
		_activeState = _levelEndState;
		break;
	case PAUSE_SCREEN:
		_activeState = _pauseState;
		break;
	case LOADMAP_SCREEN:
		_activeState = _loadMapState;
		break;
	case OPTIONS_SCREEN:
		_activeState = _optionsState;
		static_cast<OptionsState*>(_optionsState)->setLabels();
		break;
	case UPGRADES_SCREEN:
		us->setMap(_activeMapFilename);
		_activeState = _upgradesState;
		break;
	}

	//done to prevent the mouse cursor sprite from being initially drawn in the old position.
	_activeState->setMousePosition(old_x, old_y);
}

void StateManager::initSceneAndMap(const char* filename)
{
	GameState* gs = static_cast<GameState*>(_gameState);
	_activeMapFilename = std::string(filename);

	if (!gs->getScene())
	{
		gs->initSceneAndMap(filename);
		gs->getScene()->setCameraDims(_winX, _winY);
	}
	else
	{
		LOGF((stderr, "ERROR: Game state currently has a scene!\n"));
	}
}

void StateManager::setWindowDims(int w, int h)
{
	_winX = w;
	_winY = h;

	static_cast<MenuState*>(_mainMenuState)->resetPositions(w, h);
	static_cast<MenuState*>(_creditsState)->resetPositions(w, h);
	static_cast<MenuState*>(_levelEndState)->resetPositions(w, h);
	static_cast<MenuState*>(_pauseState)->resetPositions(w, h);
	static_cast<MenuState*>(_loadMapState)->resetPositions(w, h);
	static_cast<MenuState*>(_optionsState)->resetPositions(w, h);
	static_cast<MenuState*>(_upgradesState)->resetPositions(w, h);
}

void StateManager::destroyScene()
{
	static_cast<GameState*>(_gameState)->deleteScene();
	_activeMapFilename = "";
}

void StateManager::update(unsigned int dT)
{
	_activeState->update(dT);
}

void StateManager::setActiveMapFilename(std::string filename)
{
	_activeMapFilename = filename;
}

void StateManager::getMapVariables(std::string filename, int* money, int* upgrades, unsigned int* bullets, int* timeToSniper, unsigned int* energy)
{
	*money = 0;
	*upgrades = 0;
	*energy = 0;
	*bullets = 0;
	*timeToSniper = 1;
	TiXmlDocument doc(filename.c_str());

	if (!doc.LoadFile())
	{
		LOGF((stderr, "Failed to load map file %s.\n", filename.c_str()));
		return;
	}

	TiXmlHandle hDoc(&doc);
	TiXmlElement *root, *lvl1, *lvl2;
	root = doc.FirstChildElement("map");

	if(!root)
	{
		LOGF((stderr, "Failed to parse map file %s.\n", filename.c_str()));
		return;
	}

	lvl1 = root->FirstChildElement("properties");
	while(lvl1)
	{
		lvl2 = lvl1->FirstChildElement("property");
		while (lvl2)
		{
			if (!strcmp(lvl2->Attribute("name"), "startingmoney"))
			{
				*money = atoi(lvl2->Attribute("value"));
			}

			if (!strcmp(lvl2->Attribute("name"), "startingupgrades"))
			{
				*upgrades = atoi(lvl2->Attribute("value"));
			}

			if (!strcmp(lvl2->Attribute("name"), "startingenergy"))
			{
				*energy = atoi(lvl2->Attribute("value"));
			}

			if (!strcmp(lvl2->Attribute("name"), "startingammo"))
			{
				*bullets = atoi(lvl2->Attribute("value"));
			}

			if (!strcmp(lvl2->Attribute("name"), "timetosniper"))
			{
				*timeToSniper = atoi(lvl2->Attribute("value"));
			}

			lvl2 = lvl2->NextSiblingElement("property");
		}
		lvl1 = lvl1->NextSiblingElement("properties");
	}
}

int StateManager::getWindowWidth()
{
	return _winX;
}
int StateManager::getWindowHeight()
{
	return _winY;
}

void StateManager::makeStartSave()
{
	GameState* gs = static_cast<GameState*>(_gameState);
	gs->getScene()->saveGame("start.sav");
}

bool StateManager::settingsChanged()
{
	OptionsState* os = static_cast<OptionsState*>(_optionsState);
	return os->_settingsChanged;
}

void StateManager::resetSettingsFlag()
{
	OptionsState* os = static_cast<OptionsState*>(_optionsState);
	os->_settingsChanged = false;
}

bool StateManager::screenshotTaken()
{
	return _gameState->tookScreenshot ||
	_mainMenuState->tookScreenshot ||
	_creditsState->tookScreenshot ||
	_levelEndState->tookScreenshot ||
	_pauseState->tookScreenshot ||
	_loadMapState->tookScreenshot ||
	_optionsState->tookScreenshot ||
	_upgradesState->tookScreenshot;
}

void StateManager::resetScreenShotFlag()
{
	_gameState->tookScreenshot = false;
	_mainMenuState->tookScreenshot = false;
	_creditsState->tookScreenshot = false;
	_levelEndState->tookScreenshot = false;
	_pauseState->tookScreenshot = false;
	_loadMapState->tookScreenshot = false;
	_optionsState->tookScreenshot = false;
	_upgradesState->tookScreenshot = false;
}