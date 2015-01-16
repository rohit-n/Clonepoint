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

#include "levelendstate.h"
#include "statemanager.h"
#include "global.h"

LevelEndState::LevelEndState(StateManager* sm) : MenuState(sm)
{
	_lvlCompleteLabel.reset(new TextLabel(640, 64, "Level Complete!", 1, 1, 1));
	_OKButton.reset(new TextButton(320, 320, (strlen("OK") + 2) * 16, 32, "OK"));
	_buttons.push_back(_OKButton);
	_labels.push_back(_lvlCompleteLabel);
}

LevelEndState::~LevelEndState()
{
	LOGF((stdout, "running Level End destructor!\n"));
}

void LevelEndState::resetPositions(int w, int h)
{
	_lvlCompleteLabel->setPosition(w * 0.48f, h * 0.1f);
	_OKButton->setPosition(w * 0.48f, h * 0.8f);
}

void LevelEndState::update(unsigned int dT)
{
	MenuState::update(dT);
}

void LevelEndState::handleButton(Button* button)
{
	if (button == _OKButton.get())
	{
		_manager->switchToState(MAINMENU_SCREEN);
		_manager->destroyScene();
	}
}