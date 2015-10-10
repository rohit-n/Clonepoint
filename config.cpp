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

#include <fstream>
#include "config.h"

ConfigManager::ConfigManager()
{

}

ConfigManager::~ConfigManager()
{

}

//bindings are loaded in BindingsManager.
bool ConfigManager::isKeyValid(const char* key)
{
	if (!strcmp(key, "fullscreen")) return true;
	if (!strcmp(key, "window_x")) return true;
	if (!strcmp(key, "window_y")) return true;
	if (!strcmp(key, "screenshot_index")) return true;
	if (!strcmp(key, "entered_light_flash")) return true;
	if (!strcmp(key, "volume")) return true;
	if (!strcmp(key, "tutorial_popups")) return true;
	if (!strcmp(key, "input_popups")) return true;

	return false;
}

std::string ConfigManager::getValue(std::string key)
{
	std::map<std::string, std::string>::iterator it = _settings.find(key);

	return it != _settings.end() ? it->second : "";
}

bool ConfigManager::getBool(std::string key)
{
	std::map<std::string, std::string>::iterator it = _settings.find(key);

	return it != _settings.end() ? (it->second == "1") : false;
}

void ConfigManager::setValue(std::string key, std::string value)
{
	if (getValue(key) != "")
	{
		_settings[key] = value;
	}
	else
	{
		if (isKeyValid(key.c_str()))
		{
			_settings.insert(std::pair<std::string, std::string>(key, value));
		}
	}
}

void ConfigManager::loadConfig(const char* filename)
{
	char* text = file_read(filename, NULL);
	char* delim = (char*)" =\t\n\r";
	char* token = strtok(text, delim);
	char* key;

	while (token)
	{
		key = token;
		token = strtok(NULL, delim);

		if (isKeyValid(key))
		{
			_settings.insert(std::pair<std::string, std::string>(std::string(key), std::string(token)));
		}

		if (token)
			token = strtok(NULL, delim);
	}
	delete [] text;
	delete [] token;
	text = NULL;
	token = NULL;
	delim = NULL;
}

void ConfigManager::saveConfig(const char* filename, std::vector<std::string> bindings)
{
	std::ofstream output(filename);
	size_t i;
	std::map<std::string, std::string>::iterator it;

	if (!output)
	{
		return;
	}

	for (it = _settings.begin(); it != _settings.end(); it++)
	{
		output << it->first << " = " << it->second << "\n";
	}

	for (i = 0; i < bindings.size(); i++)
	{
		output << bindings[i];
	}
}

std::map<std::string, std::string>* ConfigManager::getSettings()
{
	return &_settings;
}