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

#include "menustate.h"

MenuState::MenuState(StateManager* sm) : BaseState(sm)
{
	_quitting = false;
	_mouseX = 0;
	_mouseY = 0;
}

MenuState::~MenuState()
{
	_buttons.clear();
	_labels.clear();
}

size_t MenuState::getButtonCount()
{
	return _buttons.size();
}

std::shared_ptr<Button> MenuState::getButtonAt(int i)
{
	return _buttons[i];
}

void MenuState::update(unsigned int dT)
{
	for (auto& elem : _buttons)
	{
		elem->handleMouseIntersection(_mouseX, _mouseY);
	}
}

void MenuState::handleKeyDown(SDL_Keycode key)
{

}

void MenuState::handleKeyUp(SDL_Keycode key)
{

}

void MenuState::handleMouseDown(SDL_MouseButtonEvent event)
{

}

void MenuState::handleMouseUp(SDL_MouseButtonEvent event)
{
	if (event.button == SDL_BUTTON_LEFT)
	{
		for (auto& elem : _buttons)
		{
			if (elem->isMouseIntersecting(_mouseX, _mouseY))
			{
				if (elem->isClickable())
				{
					Locator::getAudio()->playSound("menu_click");
				}
				handleButton(elem.get());
				break;
			}
		}
	}
}