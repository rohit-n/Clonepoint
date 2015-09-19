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

#include "creditsstate.h"
#include "statemanager.h"

CreditsState::CreditsState(StateManager* sm) : MenuState(sm)
{
	_cancelButton = new TextButton(128, 600, (strlen("Go Back") + 2) * 16, 32, "Go Back");
	_titleLabel = new TextLabel(0, 0, "Credits", 1, 1, 1);
	_credit1 = new TextLabel(0, 0,
	                             "Clonepoint created by Rohit Nirmal\n\
	Gunpoint created by Tom Francis (www.pentadact.com)\n\
	\n\n\
	Libraries Used:\n\
	stb_image and stb_ttf by Sean Barrett (www.nothings.org)\n\
	TinyXML (www.grinninglizard.com/tinyxml/)\n\
	SDL2 (www.libsdl.org)\n\
	OpenAL Soft (kcat.strangesoft.net/openal.html)\n\
	\n\n\
	Special Thanks:\n\
	/agdg/ <3\
	", 1, 1, 1);
	_buttons.push_back(_cancelButton);
	_labels.push_back(_titleLabel);
	_labels.push_back(_credit1);
}

CreditsState::~CreditsState()
{
}

void CreditsState::resetPositions(int w, int h)
{
	_titleLabel->setPosition(w * 0.45f, h * 0.1f);
	_cancelButton->setPosition(w * 0.45f, h * 0.85f);
	_credit1->setPosition(w * 0.1f, h * 0.2f);
}

void CreditsState::handleButton(Button* button)
{
	if (button == _cancelButton)
	{
		_manager->switchToState(MAINMENU_SCREEN);
	}
}

void CreditsState::handleKeyUp(SDL_Keycode key)
{
	switch(key)
	{
	case SDLK_PRINTSCREEN:
		tookScreenshot = 1;
		break;
	default:
		break;
	}
}