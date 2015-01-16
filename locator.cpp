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

std::unique_ptr<AudioManager> Locator::_audiomanager;
std::unique_ptr<AnimationManager> Locator::_animmanager;
std::unique_ptr<BindingsManager> Locator::_bindingsmanager;
std::unique_ptr<ConfigManager> Locator::_configmanager;
std::unique_ptr<StaticSpriteManager> Locator::_spritemanager;

AudioManager* Locator::getAudio()
{
	return _audiomanager.get();
}

void Locator::provide(std::unique_ptr<AudioManager> service)
{
	_audiomanager = std::move(service);
}

BindingsManager* Locator::getBindingsManager()
{
	return _bindingsmanager.get();
}

void Locator::provide(std::unique_ptr<BindingsManager> service)
{
	_bindingsmanager = std::move(service);
}

StaticSpriteManager* Locator::getSpriteManager()
{
	return _spritemanager.get();
}

void Locator::provide(std::unique_ptr<StaticSpriteManager> service)
{
	_spritemanager = std::move(service);
}

AnimationManager* Locator::getAnimationManager()
{
	return _animmanager.get();
}

void Locator::provide(std::unique_ptr<AnimationManager> service)
{
	_animmanager = std::move(service);
}

ConfigManager* Locator::getConfigManager()
{
	return _configmanager.get();
}

void Locator::provide(std::unique_ptr<ConfigManager> service)
{
	_configmanager = std::move(service);
}