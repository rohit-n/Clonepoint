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

#include "static_sprites.h"
#include "global.h"

StaticSpriteManager::StaticSpriteManager()
{
	loadSpriteIndicesFromFile("./data/sprites/linkable.sprites");
	loadSpriteIndicesFromFile("./data/sprites/objects.sprites");
	loadSpriteIndicesFromFile("./data/sprites/player.sprites");
	loadSpriteIndicesFromFile("./data/sprites/guard.sprites");
	loadSpriteIndicesFromFile("./data/sprites/interface.sprites");
	setSpritesIndices();
}

StaticSpriteManager::~StaticSpriteManager()
{
}

void StaticSpriteManager::setSpritesIndices()
{
	_guardStaticSprites[GUARD_IDLE_GROUND] = getIndex("./data/sprites/guard.sprites", "guard_idle_ground");
	_guardStaticSprites[GUARD_IDLE_CAUTION] = getIndex("./data/sprites/guard.sprites", "guard_idle_caution");
	_guardStaticSprites[GUARD_FALLEN] = getIndex("./data/sprites/guard.sprites", "guard_fallen");
	_guardStaticSprites[GUARD_FALLEN_PLAYER] = getIndex("./data/sprites/guard.sprites", "guard_fallen_player");
	_guardStaticSprites[GUARD_FALLING] = getIndex("./data/sprites/guard.sprites", "guard_falling");
	_guardStaticSprites[GUARD_PUNCHED] = getIndex("./data/sprites/guard.sprites", "guard_punched");

	_playerStaticSprites[PLAYER_IDLE_GROUND] = getIndex("./data/sprites/player.sprites", "player_idle_ground");
	_playerStaticSprites[PLAYER_IDLE_GROUND_AIMING] = getIndex("./data/sprites/player.sprites", "player_idle_aiming");
	_playerStaticSprites[PLAYER_IDLE_CEILING] = getIndex("./data/sprites/player.sprites", "player_idle_ceiling");
	_playerStaticSprites[PLAYER_IDLE_SIDE] = getIndex("./data/sprites/player.sprites", "player_idle_side");
	_playerStaticSprites[PLAYER_SLIDE_DOWN] = getIndex("./data/sprites/player.sprites", "player_slide_down");
	_playerStaticSprites[PLAYER_DEAD_GROUND] = getIndex("./data/sprites/player.sprites", "player_dead_ground");
	_playerStaticSprites[PLAYER_DEAD_AIR] = getIndex("./data/sprites/player.sprites", "player_dead_air");
	_playerStaticSprites[PLAYER_RISING] = getIndex("./data/sprites/player.sprites", "player_rising");
	_playerStaticSprites[PLAYER_DIVING] = getIndex("./data/sprites/player.sprites", "player_diving");
	_playerStaticSprites[PLAYER_FALLING] = getIndex("./data/sprites/player.sprites", "player_falling");
	_playerStaticSprites[PLAYER_IN_ELEVATOR] = getIndex("./data/sprites/player.sprites", "player_in_elevator");
}

unsigned int StaticSpriteManager::getGuardSpriteIndex(GuardStaticSprites gss)
{
	return _guardStaticSprites[gss];
}

unsigned int StaticSpriteManager::getPlayerSpriteIndex(PlayerStaticSprites pss)
{
	return _playerStaticSprites[pss];
}

void StaticSpriteManager::loadSpriteIndicesFromFile(std::string filename)
{
	char* text = file_read(filename.c_str());
	char* delim = (char*)" =\t\n\r";
	char* token = strtok(text, delim);
	std::string spriteName;
	std::map<std::string, unsigned int> indices;

	while (token)
	{
		spriteName = std::string(token);
		token = strtok(NULL, delim);
		LOGF((stdout, "Inserting sprite %s with index %i with filename %s.\n", spriteName.c_str(), atoi(token), filename.c_str()));
		indices.insert(std::pair<std::string, unsigned int>(spriteName, atoi(token)));

		if (token)
			token = strtok(NULL, delim);
	}
	delete [] text;
	delete [] token;
	text = NULL;
	token = NULL;
	delim = NULL;

	_spriteMap.insert(std::pair<std::string, std::map<std::string, unsigned int> >(filename, indices));
}

unsigned int StaticSpriteManager::getIndex(std::string filename, std::string name)
{
	std::map<std::string, std::map<std::string, unsigned int> >::iterator it = _spriteMap.find(filename);
	std::map<std::string, unsigned int> innerMap;
	std::map<std::string, unsigned int>::iterator it2;

	if (it != _spriteMap.end())
	{
		innerMap = it->second;
		it2 = innerMap.find(name);
		if (it2 != innerMap.end())
		{
			return it2->second;
		}
		LOGF((stderr, "ERROR: Found filename %s, but not animation %s!\n", filename.c_str(), name.c_str()));
	}

	return 0;
}