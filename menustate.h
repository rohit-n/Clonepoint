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

#ifndef MENUSTATE_H
#define MENUSTATE_H
#include <vector>
#include "state.h"

class MenuState : public BaseState
{
public:
	MenuState(StateManager* sm);
	~MenuState();
	void update(unsigned int dT);
	size_t getButtonCount();
	std::shared_ptr<Button> getButtonAt(int i);
	virtual void resetPositions(int w, int h) = 0;
	void handleKeyDown(SDL_Keycode key);
	void handleKeyUp(SDL_Keycode key);
	void handleMouseDown(SDL_MouseButtonEvent event);
	void handleMouseUp(SDL_MouseButtonEvent event);
protected:
	std::vector<std::shared_ptr<Button> > _buttons;
	virtual void handleButton(Button* button) = 0;
};

#endif