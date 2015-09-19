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

#ifndef ANIMATIONS_H
#define ANIMATIONS_H

#include <memory>

#include "sprite.h"

enum eAnimations
{
	ANIM_PLAYER_RUNNING = 0,
	ANIM_PLAYER_WALKING_AIMING,
	ANIM_PLAYER_CLIMBING_UP,
	ANIM_PLAYER_CEILING,
	ANIM_PLAYER_HACKING,
	ANIM_PLAYER_ATTACH_DOWN,
	ANIM_PLAYER_REACH_ROOF,
	ANIM_PLAYER_ATTACH_TO_CEILING,
	ANIM_PLAYER_ATTACH_FROM_CEILING,
	ANIM_PLAYER_ENTER_STAIRS,
	ANIM_PLAYER_EXIT_STAIRS,
	ANIM_ENEMY_PATROLLING,
	ANIM_ENEMY_PLAYER_STRUGGLE,
	ANIM_ENEMY_KNOCK_OUT,
	ANIM_ENEMY_ENTER_STAIRS,
	ANIM_ENEMY_EXIT_STAIRS,
	ANIM_VAULT_OPEN,
	ANIM_VAULT_CLOSE,
	ANIM_ALARM_ACTIVE,
	ANIM_ELEVATOR_CLOSE,
	ANIM_ELEVATOR_OPEN,
	NUM_ANIMATIONS
};

class AnimationManager
{
public:
	AnimationManager();
	~AnimationManager();
	unsigned int getNextSprite(AnimationSequence* sequence, unsigned int* index, float* timeLeft, unsigned int dT, bool* finished);
	AnimationSequence* getSequence(eAnimations index);
private:
	AnimationSequence* _sequences[NUM_ANIMATIONS];
};

#endif