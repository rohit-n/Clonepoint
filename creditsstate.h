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

#ifndef CREDITSTATE_H
#define CREDITSTATE_H
#include "menustate.h"

class CreditsState : public MenuState
{
public:
	CreditsState(StateManager* sm);
	~CreditsState();
	void handleButton(Button* button);
	void resetPositions(int w, int h);
	void handleKeyUp(SDL_Keycode key);
private:
	std::shared_ptr<TextButton> _cancelButton;
	std::shared_ptr<TextLabel> _titleLabel;
	std::shared_ptr<TextLabel> _credit1;
};

#endif