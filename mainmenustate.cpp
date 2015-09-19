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

#include "mainmenustate.h"
#include "statemanager.h"
#include "global.h"

MainMenuState::MainMenuState(StateManager* sm) : MenuState(sm)
{
	_titleLabel = new TextLabel(0, 0, "Clonepoint", 1, 1, 1);
	_quitButton = new TextButton(0, 0, (strlen("Quit Game") + 2) * 16, 32, "Quit Game");
	_creditsButton = new TextButton(640, 576, (strlen("View Credits") + 2) * 16, 32, "View Credits");
	_loadMapButton = new TextButton(0, 0, (strlen("Load a Map") + 2) * 16, 32, "Load a Map");
	_optionsButton = new TextButton(0, 0, (strlen("Options") + 2) * 16, 32, "Options");

	_labels.push_back(_titleLabel);
	_buttons.push_back(_quitButton);
	_buttons.push_back(_loadMapButton);
	_buttons.push_back(_optionsButton);
	_buttons.push_back(_creditsButton);
}

MainMenuState::~MainMenuState()
{
}

void MainMenuState::resetPositions(int w, int h)
{
	int mid = w * 0.48;
	_titleLabel->setPosition(mid, h * 0.1f);
	_loadMapButton->setPosition(mid, h * 0.3f);
	_optionsButton->setPosition(mid, h * 0.35f);
	_creditsButton->setPosition(mid, h * 0.4f);
	_quitButton->setPosition(mid, h * 0.45f);
}

void MainMenuState::update(unsigned int dT)
{
	MenuState::update(dT);
}

void MainMenuState::handleButton(Button* button)
{
	if (button == _quitButton)
	{
		_quitting = true;
	}
	else if (button == _creditsButton)
	{
		_manager->switchToState(CREDITS_SCREEN);
	}
	else if (button == _loadMapButton)
	{
		_manager->switchToState(LOADMAP_SCREEN);
	}
	else if (button == _optionsButton)
	{
		_manager->switchToState(OPTIONS_SCREEN);
	}
}