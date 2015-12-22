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

#ifndef STATEMANAGER_H
#define STATEMANAGER_H

#include <memory>
#include <string>

enum eState
{
	MAINMENU_SCREEN = 0,
	GAME_SCREEN,
	CREDITS_SCREEN,
	LEVELEND_SCREEN,
	PAUSE_SCREEN,
	LOADINGMAP_SCREEN,
	LOADMAP_SCREEN,
	OPTIONS_SCREEN,
	UPGRADES_SCREEN
};

class BaseState;

class StateManager
{
public:
	StateManager();
	~StateManager();
	void update(unsigned int dT);
	BaseState* getGameState();
	BaseState* getActiveState();
	void switchToState(eState state);
	void initSceneAndMap(const char* filename);
	void destroyScene();
	void setWindowDims(int w, int h);
	void setActiveMapFilename(std::string filename);
	void getMapVariables(std::string filename, int* money, int* upgrades, unsigned int* bullets, int* timeToSniper, unsigned int* energy);
	void getMusicFilename(std::string mapFilename);
	int getWindowWidth();
	int getWindowHeight();
	void makeStartSave();
	bool settingsChanged();
	void resetSettingsFlag();
	bool screenshotTaken();
	void resetScreenShotFlag();
private:
	BaseState* _activeState;
	BaseState* _gameState;
	BaseState* _mainMenuState;
	BaseState* _creditsState;
	BaseState* _levelEndState;
	BaseState* _pauseState;
	BaseState* _loadingMapState;
	BaseState* _loadMapState;
	BaseState* _optionsState;
	BaseState* _upgradesState;
	bool _settingsChanged;
	std::string _activeMapFilename;
	std::string _mapMusicFilename;
	int _winX;
	int _winY;
};

#endif