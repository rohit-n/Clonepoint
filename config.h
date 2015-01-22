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

#ifndef CONFIG_H
#define CONFIG_H
#include <map>
#include <vector>
#include <cstring>
#include <string>
#include "file.h"

class ConfigManager
{
public:
	ConfigManager();
	~ConfigManager();
	void loadConfig(const char* filename);
	void saveConfig(const char* filename, std::vector<std::string> bindings);
	bool isKeyValid(const char* key);
	std::string getValue(std::string key);
	bool getBool(std::string key);
	void setValue(std::string key, std::string value);
	std::map<std::string, std::string>* getSettings();
private:
	std::map<std::string, std::string> _settings;
};
#endif