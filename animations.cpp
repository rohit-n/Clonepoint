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

#include "animations.h"

AnimationManager::AnimationManager()
{
	_sequences[ANIM_PLAYER_RUNNING].reset(new AnimationSequence(true));
	_sequences[ANIM_PLAYER_RUNNING]->readFromFile("./data/animations/player_run.anim");

	_sequences[ANIM_PLAYER_WALKING_AIMING].reset(new AnimationSequence(true));
	_sequences[ANIM_PLAYER_WALKING_AIMING]->readFromFile("./data/animations/player_walk_aiming.anim");

	_sequences[ANIM_PLAYER_CLIMBING_UP].reset(new AnimationSequence(true));
	_sequences[ANIM_PLAYER_CLIMBING_UP]->readFromFile("./data/animations/player_climb_up.anim");

	_sequences[ANIM_PLAYER_CEILING].reset(new AnimationSequence(true));
	_sequences[ANIM_PLAYER_CEILING]->readFromFile("./data/animations/player_ceiling.anim");

	_sequences[ANIM_PLAYER_HACKING].reset(new AnimationSequence(false));
	_sequences[ANIM_PLAYER_HACKING]->readFromFile("./data/animations/player_hack_terminal.anim");

	_sequences[ANIM_PLAYER_ATTACH_DOWN].reset(new AnimationSequence(false));
	_sequences[ANIM_PLAYER_ATTACH_DOWN]->readFromFile("./data/animations/player_attach_down.anim");

	_sequences[ANIM_PLAYER_REACH_ROOF].reset(new AnimationSequence(false));
	_sequences[ANIM_PLAYER_REACH_ROOF]->readFromFile("./data/animations/player_reach_roof.anim");

	_sequences[ANIM_PLAYER_ATTACH_TO_CEILING].reset(new AnimationSequence(false));
	_sequences[ANIM_PLAYER_ATTACH_TO_CEILING]->readFromFile("./data/animations/player_attach_to_ceiling.anim");

	_sequences[ANIM_PLAYER_ATTACH_FROM_CEILING].reset(new AnimationSequence(false));
	_sequences[ANIM_PLAYER_ATTACH_FROM_CEILING]->readFromFile("./data/animations/player_attach_from_ceiling.anim");

	_sequences[ANIM_PLAYER_ENTER_STAIRS].reset(new AnimationSequence(false));
	_sequences[ANIM_PLAYER_ENTER_STAIRS]->readFromFile("./data/animations/player_enter_stairs.anim");

	_sequences[ANIM_PLAYER_EXIT_STAIRS].reset(new AnimationSequence(false));
	_sequences[ANIM_PLAYER_EXIT_STAIRS]->readFromFile("./data/animations/player_exit_stairs.anim");

	_sequences[ANIM_ENEMY_PATROLLING].reset(new AnimationSequence(true));
	_sequences[ANIM_ENEMY_PATROLLING]->readFromFile("./data/animations/enemy_patrolling.anim");

	_sequences[ANIM_ENEMY_PLAYER_STRUGGLE].reset(new AnimationSequence(true));
	_sequences[ANIM_ENEMY_PLAYER_STRUGGLE]->readFromFile("./data/animations/enemy_player_struggle.anim");

	_sequences[ANIM_ENEMY_KNOCK_OUT].reset(new AnimationSequence(false));
	_sequences[ANIM_ENEMY_KNOCK_OUT]->readFromFile("./data/animations/enemy_punch_out.anim");

	_sequences[ANIM_ENEMY_ENTER_STAIRS].reset(new AnimationSequence(false));
	_sequences[ANIM_ENEMY_ENTER_STAIRS]->readFromFile("./data/animations/enemy_enter_stairs.anim");

	_sequences[ANIM_ENEMY_EXIT_STAIRS].reset(new AnimationSequence(false));
	_sequences[ANIM_ENEMY_EXIT_STAIRS]->readFromFile("./data/animations/enemy_exit_stairs.anim");

	_sequences[ANIM_VAULT_OPEN].reset(new AnimationSequence(false));
	_sequences[ANIM_VAULT_OPEN]->readFromFile("./data/animations/vault_open.anim");

	_sequences[ANIM_VAULT_CLOSE].reset(new AnimationSequence(false));
	_sequences[ANIM_VAULT_CLOSE]->readFromFile("./data/animations/vault_close.anim");

	_sequences[ANIM_ALARM_ACTIVE].reset(new AnimationSequence(true));
	_sequences[ANIM_ALARM_ACTIVE]->readFromFile("./data/animations/alarm_active.anim");

	_sequences[ANIM_ELEVATOR_CLOSE].reset(new AnimationSequence(false));
	_sequences[ANIM_ELEVATOR_CLOSE]->readFromFile("./data/animations/elevator_close.anim");

	_sequences[ANIM_ELEVATOR_OPEN].reset(new AnimationSequence(false));
	_sequences[ANIM_ELEVATOR_OPEN]->readFromFile("./data/animations/elevator_open.anim");
}

AnimationManager::~AnimationManager()
{
}

AnimationSequence* AnimationManager::getSequence(eAnimations index)
{
	return _sequences[index].get();
}

unsigned int AnimationManager::getNextSprite(AnimationSequence* sequence, unsigned int* index, float* timeLeft, unsigned int dT, bool* finished)
{
	*finished = false;
	*timeLeft -= dT;

	if (*timeLeft <= 0.0f)
	{
		*timeLeft = sequence->getMsPerFrame();
		*index = *index + 1;
		if (*index >= sequence->getNumSprites())
		{
			*index = 0;
			if (!sequence->isLooping())
			{
				*finished = true;
			}
		}
	}

	return sequence->getSpriteAt(*index);
}