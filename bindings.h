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

#ifndef BINDINGS_H
#define BINDINGS_H
#include <map>
#include <vector>
#include <SDL2/SDL.h>
#include <string>
#include <cstring>
#include <cstdio>
#include "file.h"
#include "global.h"

enum eBinding
{
	Bind_Nothing = 0,
	Bind_MoveLeft,
	Bind_MoveRight,
	Bind_MoveUp,
	Bind_MoveDown,
	Bind_ToggleCrosslink
};

class BindingsManager
{
public:
	BindingsManager();
	~BindingsManager();
	void loadBindingsFromConfig(const char* filename);
	eBinding checkToken(std::string token);
	SDL_Keycode getKeyFromToken(char* token);
	std::string getKeyAsString(SDL_Keycode key);
	eBinding getBindingFromKey(SDL_Keycode key);
	std::string bindingToString(eBinding binding);
	std::vector<std::string> getBindingsToSave();
	void clearBinding(eBinding binding);
	std::string getKeysBound(eBinding binding);
	std::string getFirstKeyBound(eBinding binding);
	void addBinding(SDL_Keycode key, eBinding binding);
private:
	std::map<SDL_Keycode, eBinding> _bindings;
};

#endif