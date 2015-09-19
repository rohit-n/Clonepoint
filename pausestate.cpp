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

#include "pausestate.h"
#include "statemanager.h"
#include "global.h"

PauseState::PauseState(StateManager* sm) : MenuState(sm)
{
	_resumeButton = new TextButton(320, 320, (strlen("Resume") + 2) * 16, 32, "Resume");
	_exitButton = new TextButton(320, 640, (strlen("Exit") + 2) * 16, 32, "Exit");
	_buttons.push_back(_resumeButton);
	_buttons.push_back(_exitButton);
}

PauseState::~PauseState()
{
	LOGF((stdout, "running pause destructor!\n"));
}

void PauseState::update(unsigned int dT)
{
	MenuState::update(dT);
}

void PauseState::resetPositions(int w, int h)
{
	_resumeButton->setPosition(w * 0.48f, h * 0.4f);
	_exitButton->setPosition(w * 0.48f, h * 0.45f);
}

void PauseState::handleButton(Button* button)
{
	if (button == _resumeButton)
	{
		_manager->switchToState(GAME_SCREEN);
	}

	if (button == _exitButton)
	{
		_manager->switchToState(MAINMENU_SCREEN);
		_manager->destroyScene();
	}
}