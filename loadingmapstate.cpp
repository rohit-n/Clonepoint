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

#include "loadingmapstate.h"

LoadingMapState::LoadingMapState(StateManager* sm) : BaseState(sm)
{
	_mapFilename = "";
	_mapMusicFilename = "";
	_timer = 0;
	_loaded = false;
}

LoadingMapState::~LoadingMapState()
{
}

void LoadingMapState::update(unsigned int dT)
{
	_timer += dT;

	if (_timer > 50 && ! _loaded)
	{
		_manager->initSceneAndMap(_mapFilename.c_str());
		_manager->makeStartSave();
		Locator::getAudio()->playMusic(_mapMusicFilename);
		_loaded = true;
	}

	if (_timer > 100)
	{
		//give a little extra time before displaying to avoid seeing the last
		//game (if you exited to the main menu and loaded another map).
		_manager->switchToState(GAME_SCREEN); 
	}
}

void LoadingMapState::handleKeyDown(SDL_Keycode){}
void LoadingMapState::handleKeyUp(SDL_Keycode){}
void LoadingMapState::handleMouseDown(SDL_MouseButtonEvent event){}
void LoadingMapState::handleMouseUp(SDL_MouseButtonEvent event){}

void LoadingMapState::setMap(std::string mapFilename, std::string musicFilename)
{
	_mapFilename = mapFilename;
	_mapMusicFilename = musicFilename;
	_timer = 0;
	_loaded = false;
}