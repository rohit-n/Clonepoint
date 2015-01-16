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

#include <memory>
#include <string>

enum eState
{
	MAINMENU_SCREEN = 0,
	GAME_SCREEN,
	CREDITS_SCREEN,
	LEVELEND_SCREEN,
	PAUSE_SCREEN,
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
	std::shared_ptr<BaseState> getGameState();
	std::shared_ptr<BaseState> getActiveState();
	void switchToState(eState state);
	void initSceneAndMap(const char* filename);
	void destroyScene();
	void setWindowDims(int w, int h);
	void changeSettings();
	void setActiveMapFilename(std::string filename);
	void getMapVariables(std::string filename, int* money, int* upgrades, unsigned int* bullets, int* timeToSniper, unsigned int* energy);
	void registerScreenshotFunctions(std::function<void()> func);
	void registerSettingsChange(std::function<void()> func);
	int getWindowWidth();
	int getWindowHeight();
	void makeStartSave();
private:
	std::shared_ptr<BaseState> _activeState;
	std::shared_ptr<BaseState> _gameState;
	std::shared_ptr<BaseState> _mainMenuState;
	std::shared_ptr<BaseState> _creditsState;
	std::shared_ptr<BaseState> _levelEndState;
	std::shared_ptr<BaseState> _pauseState;
	std::shared_ptr<BaseState> _loadMapState;
	std::shared_ptr<BaseState> _optionsState;
	std::shared_ptr<BaseState> _upgradesState;
	bool _settingsChanged;
	std::string _activeMapFilename;
	std::function<void()> _changeSettings;
	int _winX;
	int _winY;
};