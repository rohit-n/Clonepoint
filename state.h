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

#ifndef STATE_H
#define STATE_H
#include <SDL2/SDL.h>
#include "button.h"
#include "locator.h"

class StateManager;

class BaseState
{
public:
	BaseState(StateManager* sm);
	virtual ~BaseState();
	bool isQuitting();
	virtual void update(unsigned int dT) = 0;
	virtual void handleInput();
	void registerScreenshotFunction(std::function<void()> func);
	size_t getLabelCount();
	std::shared_ptr<TextLabel> getLabelAt(size_t i);
	virtual void handleMouseWheel(int dir);
	virtual void handleKeyDown(SDL_Keycode key) = 0;
	virtual void handleKeyUp(SDL_Keycode key) = 0;
	virtual void handleMouseDown(SDL_MouseButtonEvent event) = 0;
	virtual void handleMouseUp(SDL_MouseButtonEvent event) = 0;
	void getMousePosition(int* mx, int* my);
	void setMousePosition(int mx, int my);
protected:
	SDL_Event _event;
	bool _quitting;
	int _mouseX;
	int _mouseY;
	StateManager* _manager;
	std::function<void()> _takeScreenshot;
	std::vector<std::shared_ptr<TextLabel> > _labels;
};

#endif