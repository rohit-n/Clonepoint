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

#include "locator.h"

AudioManager* Locator::_audiomanager;
AnimationManager* Locator::_animmanager;
BindingsManager* Locator::_bindingsmanager;
ConfigManager* Locator::_configmanager;
StaticSpriteManager* Locator::_spritemanager;

AudioManager* Locator::getAudio()
{
	return _audiomanager;
}

void Locator::provide(AudioManager* service)
{
	_audiomanager = service;
}

BindingsManager* Locator::getBindingsManager()
{
	return _bindingsmanager;
}

void Locator::provide(BindingsManager* service)
{
	_bindingsmanager = service;
}

StaticSpriteManager* Locator::getSpriteManager()
{
	return _spritemanager;
}

void Locator::provide(StaticSpriteManager* service)
{
	_spritemanager = service;
}

AnimationManager* Locator::getAnimationManager()
{
	return _animmanager;
}

void Locator::provide(AnimationManager* service)
{
	_animmanager = service;
}

ConfigManager* Locator::getConfigManager()
{
	return _configmanager;
}

void Locator::provide(ConfigManager* service)
{
	_configmanager = service;
}