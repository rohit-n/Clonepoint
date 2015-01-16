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

#ifndef STATIC_SPRITES_H
#define STATIC_SPRITES_H
#include <map>
#include <cstring>
#include <cstdio>
#include <string>
#include "file.h"

enum PlayerStaticSprites
{
	PLAYER_IDLE_GROUND = 0,
	PLAYER_IDLE_GROUND_AIMING,
	PLAYER_IDLE_CEILING,
	PLAYER_IDLE_SIDE,
	PLAYER_SLIDE_DOWN,
	PLAYER_DEAD_GROUND,
	PLAYER_DEAD_AIR,
	PLAYER_RISING,
	PLAYER_DIVING,
	PLAYER_FALLING,
	PLAYER_IN_ELEVATOR,
	NUM_PLAYER_STATIC_SPRITES
};

enum GuardStaticSprites
{
	GUARD_IDLE_GROUND = 0,
	GUARD_IDLE_CAUTION,
	GUARD_FALLEN,
	GUARD_FALLEN_PLAYER,
	GUARD_FALLING,
	GUARD_PUNCHED, //only used for playing punch sound at appropriate time.
	NUM_GUARD_STATIC_SPRITES
};

class StaticSpriteManager
{
public:
	StaticSpriteManager();
	~StaticSpriteManager();
	void loadSpriteIndicesFromFile(std::string filename);
	unsigned int getIndex(std::string filename, std::string name);
	void setSpritesIndices();
	unsigned int getGuardSpriteIndex(GuardStaticSprites gss);
	unsigned int getPlayerSpriteIndex(PlayerStaticSprites pss);
private:
	std::map<std::string, std::map<std::string, unsigned int> > _spriteMap;
	unsigned int _guardStaticSprites[NUM_GUARD_STATIC_SPRITES];
	unsigned int _playerStaticSprites[NUM_PLAYER_STATIC_SPRITES];
};

#endif