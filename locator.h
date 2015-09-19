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

#ifndef LOCATOR_H
#define LOCATOR_H
#include "audio.h"
#include "animations.h"
#include "bindings.h"
#include "config.h"
#include "static_sprites.h"

class Locator
{
public:
	static AudioManager* getAudio();
	static void provide(AudioManager* service);

	static BindingsManager* getBindingsManager();
	static void provide(BindingsManager* service);

	static StaticSpriteManager* getSpriteManager();
	static void provide(StaticSpriteManager* service);

	static AnimationManager* getAnimationManager();
	static void provide(AnimationManager* service);

	static ConfigManager* getConfigManager();
	static void provide(ConfigManager* service);
private:
	static AudioManager* _audiomanager;
	static AnimationManager* _animmanager;
	static BindingsManager* _bindingsmanager;
	static ConfigManager* _configmanager;
	static StaticSpriteManager* _spritemanager;
};

#endif