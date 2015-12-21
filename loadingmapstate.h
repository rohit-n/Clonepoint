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

#ifndef LOADINGMAPSTATE_H
#define LOADINGMAPSTATE_H
#include "state.h"
#include "statemanager.h"

class LoadingMapState : public BaseState
{
public:
	LoadingMapState(StateManager* sm);
	~LoadingMapState();
	void update(unsigned int dT);
	void handleKeyDown(SDL_Keycode);
	void handleKeyUp(SDL_Keycode);
	void handleMouseDown(SDL_MouseButtonEvent event);
	void handleMouseUp(SDL_MouseButtonEvent event);
	void setMap(std::string mapFilename);
private:
	std::string _mapFilename;
	unsigned int _timer;
	bool _loaded;
};

#endif