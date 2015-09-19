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

#include "state.h"

BaseState::BaseState(StateManager* sm)
{
	_quitting = false;
	tookScreenshot = false;
	_manager = sm;
	_mouseX = 0;
	_mouseY = 0;
}

BaseState::~BaseState()
{
	size_t i;
	for (i = 0; i < _labels.size(); i++)
	{
		delete _labels[i];
	}
	_labels.clear();
}

bool BaseState::isQuitting()
{
	return _quitting;
}

size_t BaseState::getLabelCount()
{
	return _labels.size();
}

TextLabel* BaseState::getLabelAt(size_t i)
{
	return _labels[i];
}

void BaseState::handleMouseWheel(int dir)
{

}

void BaseState::handleInput()
{
	while (SDL_PollEvent(&_event))
	{
		if (_event.type == SDL_QUIT)
		{
			_quitting = true;
		}
		else if (_event.type == SDL_MOUSEWHEEL)
		{
			handleMouseWheel(_event.wheel.y);
		}
		else if (_event.type == SDL_KEYUP)
		{
			handleKeyUp(_event.key.keysym.sym);
		}
		else if (_event.type == SDL_KEYDOWN)
		{
			handleKeyDown(_event.key.keysym.sym);
		}
		else if (_event.type == SDL_MOUSEBUTTONDOWN)
		{
			handleMouseDown(_event.button);
		}
		else if (_event.type == SDL_MOUSEBUTTONUP)
		{
			handleMouseUp(_event.button);
		}

		if (_event.type == SDL_MOUSEBUTTONUP
		        || _event.type == SDL_MOUSEBUTTONDOWN
		        || _event.type == SDL_MOUSEMOTION)
		{
			setMousePosition(_event.button.x, _event.button.y);
		}
	}
}

void BaseState::getMousePosition(int* mx, int* my)
{
	*mx = _mouseX;
	*my = _mouseY;
}

void BaseState::setMousePosition(int mx, int my)
{
	_mouseX = mx;
	_mouseY = my;
}