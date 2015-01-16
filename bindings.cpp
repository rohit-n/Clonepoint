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

#include "bindings.h"
#include "global.h"

BindingsManager::BindingsManager()
{

}

BindingsManager::~BindingsManager()
{
}

eBinding BindingsManager::checkToken(std::string token)
{
	if (token == "Bind_MoveRight") return Bind_MoveRight;
	if (token == "Bind_MoveLeft") return Bind_MoveLeft;
	if (token == "Bind_MoveUp") return Bind_MoveUp;
	if (token == "Bind_MoveDown") return Bind_MoveDown;
	if (token == "Bind_ToggleCrosslink") return Bind_ToggleCrosslink;
	return Bind_Nothing;
}

std::string BindingsManager::bindingToString(eBinding binding)
{
	switch (binding)
	{
	case Bind_MoveRight:
		return "Bind_MoveRight";
	case Bind_MoveLeft:
		return "Bind_MoveLeft";
	case Bind_MoveUp:
		return "Bind_MoveUp";
	case Bind_MoveDown:
		return "Bind_MoveDown";
	case Bind_ToggleCrosslink:
		return "Bind_ToggleCrosslink";
	default:
		return "";
	}
}

SDL_Keycode BindingsManager::getKeyFromToken(char* token)
{
	if (!strcmp(token, "A")) return SDLK_a;
	if (!strcmp(token, "B")) return SDLK_b;
	if (!strcmp(token, "C")) return SDLK_c;
	if (!strcmp(token, "D")) return SDLK_d;
	if (!strcmp(token, "E")) return SDLK_e;
	if (!strcmp(token, "F")) return SDLK_f;
	if (!strcmp(token, "G")) return SDLK_g;
	if (!strcmp(token, "H")) return SDLK_h;
	if (!strcmp(token, "I")) return SDLK_i;
	if (!strcmp(token, "J")) return SDLK_j;
	if (!strcmp(token, "K")) return SDLK_k;
	if (!strcmp(token, "L")) return SDLK_l;
	if (!strcmp(token, "M")) return SDLK_m;
	if (!strcmp(token, "N")) return SDLK_n;
	if (!strcmp(token, "O")) return SDLK_o;
	if (!strcmp(token, "P")) return SDLK_p;
	if (!strcmp(token, "Q")) return SDLK_q;
	if (!strcmp(token, "R")) return SDLK_r;
	if (!strcmp(token, "S")) return SDLK_s;
	if (!strcmp(token, "T")) return SDLK_t;
	if (!strcmp(token, "U")) return SDLK_u;
	if (!strcmp(token, "V")) return SDLK_v;
	if (!strcmp(token, "W")) return SDLK_w;
	if (!strcmp(token, "X")) return SDLK_x;
	if (!strcmp(token, "Y")) return SDLK_y;
	if (!strcmp(token, "Z")) return SDLK_z;
	if (!strcmp(token, "Left")) return SDLK_LEFT;
	if (!strcmp(token, "Right")) return SDLK_RIGHT;
	if (!strcmp(token, "Up")) return SDLK_UP;
	if (!strcmp(token, "Down")) return SDLK_DOWN;
	if (!strcmp(token, "RCtrl")) return SDLK_RCTRL;
	if (!strcmp(token, "RAlt")) return SDLK_RALT;
	if (!strcmp(token, "LCtrl")) return SDLK_LCTRL;
	if (!strcmp(token, "LAlt")) return SDLK_LALT;

	return SDLK_CLEAR;
}

std::string BindingsManager::getKeyAsString(SDL_Keycode key)
{
	switch(key)
	{
	case SDLK_a:
		return "A";
	case SDLK_b:
		return "B";
	case SDLK_c:
		return "C";
	case SDLK_d:
		return "D";
	case SDLK_e:
		return "E";
	case SDLK_f:
		return "F";
	case SDLK_g:
		return "G";
	case SDLK_h:
		return "H";
	case SDLK_i:
		return "I";
	case SDLK_j:
		return "J";
	case SDLK_k:
		return "K";
	case SDLK_l:
		return "L";
	case SDLK_m:
		return "M";
	case SDLK_n:
		return "N";
	case SDLK_o:
		return "O";
	case SDLK_p:
		return "P";
	case SDLK_q:
		return "Q";
	case SDLK_r:
		return "R";
	case SDLK_s:
		return "S";
	case SDLK_t:
		return "T";
	case SDLK_u:
		return "U";
	case SDLK_v:
		return "V";
	case SDLK_w:
		return "W";
	case SDLK_x:
		return "X";
	case SDLK_y:
		return "Y";
	case SDLK_z:
		return "Z";
	case SDLK_LEFT:
		return "Left";
	case SDLK_RIGHT:
		return "Right";
	case SDLK_UP:
		return "Up";
	case SDLK_DOWN:
		return "Down";
	case SDLK_RCTRL:
		return "RCtrl";
	case SDLK_RALT:
		return "RAlt";
	case SDLK_LCTRL:
		return "LCtrl";
	case SDLK_LALT:
		return "LAlt";
	default:
		return "NONE";
	}
}

eBinding BindingsManager::getBindingFromKey(SDL_Keycode key)
{
	auto it = _bindings.find(key);

	if (it != _bindings.end())
	{
		return it->second;
	}

	return Bind_Nothing;
}

void BindingsManager::loadBindingsFromConfig(const char* filename)
{
	char* text = file_read(filename);
	char* delim = (char*)" =\t\n\r";
	char* token = strtok(text, delim);
	eBinding binding;

	while (token)
	{
		binding = checkToken(token);
		token = strtok(nullptr, delim);
		if (binding != Bind_Nothing)
		{
			SDL_Keycode key = getKeyFromToken(token);
			addBinding(key, binding);
		}

		if (token)
			token = strtok(nullptr, delim);
	}

	delete [] text;
	delete [] token;
	text = nullptr;
	token = nullptr;
	delim = nullptr;
}

std::vector<std::string> BindingsManager::getBindingsToSave()
{
	std::vector<std::string> bindings;
	std::string line;
	std::map<SDL_Keycode, eBinding>::iterator it;

	for (it = _bindings.begin(); it != _bindings.end(); it++)
	{
		line = bindingToString(it->second) + " = " + getKeyAsString(it->first) + "\n";
		bindings.push_back(line);
	}

	return bindings;
}

void BindingsManager::clearBinding(eBinding binding)
{
	std::map<SDL_Keycode, eBinding>::iterator it;

	for (it = _bindings.begin(); it != _bindings.end(); it++)
	{
		if (it->second == binding)
		{
			_bindings.erase(it);
		}
	}
}

std::string BindingsManager::getKeysBound(eBinding binding)
{
	std::string keysBound = "";
	std::map<SDL_Keycode, eBinding>::iterator it;

	for (it = _bindings.begin(); it != _bindings.end(); it++)
	{
		if (it->second == binding)
		{
			if (keysBound != "")
			{
				keysBound += ";";
			}
			keysBound += getKeyAsString(it->first);
		}
	}
	if (keysBound == "")
	{
		keysBound = "NONE";
	}

	return keysBound;
}

void BindingsManager::addBinding(SDL_Keycode key, eBinding binding)
{
	//erase previous keys for this binding first.
	std::map<SDL_Keycode, eBinding>::iterator it;
	std::string keyAsString = getKeyAsString(key);

	if (keyAsString == "NONE")
	{
		return;
	}
	for (it = _bindings.begin(); it != _bindings.end(); it++)
	{
		if (it->first == key)
		{
			_bindings.erase(it);
		}
	}

	_bindings.insert(std::pair<SDL_Keycode, eBinding>(key, binding));
}