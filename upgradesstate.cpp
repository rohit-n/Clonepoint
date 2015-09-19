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

#include <algorithm>
#include "upgradesstate.h"
#include "statemanager.h"
#include "global.h"

UpgradesState::UpgradesState(StateManager* sm) : MenuState(sm)
{
	int i;
	unsigned int left = Locator::getSpriteManager()->getIndex("./data/sprites/interface.sprites", "arrowleft");
	unsigned int left_selected = Locator::getSpriteManager()->getIndex("./data/sprites/interface.sprites", "arrowleft_selected");
	unsigned int right = Locator::getSpriteManager()->getIndex("./data/sprites/interface.sprites", "arrowright");
	unsigned int right_selected = Locator::getSpriteManager()->getIndex("./data/sprites/interface.sprites", "arrowright_selected");
	unsigned int empty = Locator::getSpriteManager()->getIndex("./data/sprites/interface.sprites", "box_empty");

	_titleLabel = new TextLabel(0, 0, "Select your upgrades.", 1, 1, 1);
	_jumpPowerLabel = new TextLabel(0, 0, "Jump Power", 1, 1, 1);
	_jumpTimeLabel = new TextLabel(0, 0, "Time to Charge Jump", 1, 1, 1);
	_upgradesLeft = new TextLabel(0, 0, "0", 1, 1, 1);

	_exitButton = new TextButton(0, 0, (strlen("Go Back") + 2) * 16, 32, "Go Back");
	_startMapButton = new TextButton(0, 0, (strlen("Start Map") + 2) * 16, 32, "Start Map");

	_jumpPowerDecr = new ImageButton(0, 0, 32, 32, left, left_selected);
	_jumpPowerIncr = new ImageButton(0, 0, 32, 32, right, right_selected);

	_jumpTimeDecr = new ImageButton(0, 0, 32, 32, left, left_selected);
	_jumpTimeIncr = new ImageButton(0, 0, 32, 32, right, right_selected);

	_buttons.push_back(_jumpPowerDecr);
	_buttons.push_back(_jumpPowerIncr);
	_buttons.push_back(_jumpTimeDecr);
	_buttons.push_back(_jumpTimeIncr);

	for (i = 0; i < NUM_JUMP_POWER_UPGRADES; i++)
	{
		_jumpPowerProgress[i] = new ImageButton(0, 0, 32, 32, false, empty);
		_buttons.push_back(_jumpPowerProgress[i]);
	}

	for (i = 0; i < NUM_JUMP_TIME_UPGRADES; i++)
	{
		_jumpTimeProgress[i] = new ImageButton(0, 0, 32, 32, false, empty);
		_buttons.push_back(_jumpTimeProgress[i]);
	}

	_buttons.push_back(_exitButton);
	_buttons.push_back(_startMapButton);

	_labels.push_back(_titleLabel);
	_labels.push_back(_jumpPowerLabel);
	_labels.push_back(_jumpTimeLabel);
	_labels.push_back(_upgradesLeft);

	_mapFilename = "";
	_jumpPower = 0;
	_jumpTime = 0;
	_availableMoney = 0;
	_availableUpgrades = 0;
	_availableTimeToSniper = 0;
	_availableBullets = 0;
	_availableEnergy = 0;
	updateProgressBars();
}

UpgradesState::~UpgradesState()
{
	if (_jumpTime == 0)
	{
		delete _jumpTimeDecr;
	}

	if (_jumpPower == 0)
	{
		delete _jumpPowerDecr;
	}

	if (_availableUpgrades == 0 || _jumpTime == NUM_JUMP_TIME_UPGRADES)
	{
		delete _jumpTimeIncr;
	}

	if (_availableUpgrades == 0 || _jumpPower == NUM_JUMP_POWER_UPGRADES)
	{
		delete _jumpPowerIncr;
	}
}

void UpgradesState::resetPositions(int w, int h)
{
	int x, y;
	int i;

	x = w * 0.5;

	_exitButton->setPosition(x, h * 0.75f);
	_titleLabel->setPosition(x - (_titleLabel->getText().length() * 8), h * 0.1f);
	_startMapButton->setPosition(x, h * 0.7f);
	_jumpPowerLabel->setPosition(w * 0.25f, h * 0.45f);
	_jumpTimeLabel->setPosition(w * 0.15f, h * 0.5f);
	_upgradesLeft->setPosition(w * 0.1f, h * 0.1f);

	x = w * 0.4f;
	y = h * 0.42f;

	for (i = 0; i < NUM_JUMP_POWER_UPGRADES; i++)
	{
		_jumpPowerProgress[i]->setPositionWithOffset(x, y, 16 * i, 0);
	}

	_jumpPowerDecr->setPositionWithOffset(x, y, (16 * i) + 32, 0);
	_jumpPowerIncr->setPositionWithOffset(x, y, (16 * i) + 64, 0);

	y = h * 0.47f;

	for (i = 0; i < NUM_JUMP_TIME_UPGRADES; i++)
	{
		_jumpTimeProgress[i]->setPositionWithOffset(x, y, 16 * i, 0);
	}

	_jumpTimeDecr->setPositionWithOffset(x, y, (16 * i) + 32, 0);
	_jumpTimeIncr->setPositionWithOffset(x, y, (16 * i) + 64, 0);
}

void UpgradesState::update(unsigned int dT)
{
	MenuState::update(dT);
}

void UpgradesState::handleButton(Button* button)
{
	if (button == _exitButton)
	{
		_manager->switchToState(LOADMAP_SCREEN);
	}
	else if (button == _startMapButton)
	{
		_manager->initSceneAndMap(_mapFilename.c_str());
		_manager->switchToState(GAME_SCREEN);
		_manager->makeStartSave();
	}
	else if (button == _jumpPowerDecr && _jumpPower > 0)
	{
		_jumpPower--;
		_availableUpgrades++;
	}
	else if (button == _jumpPowerIncr && _jumpPower < NUM_JUMP_POWER_UPGRADES && _availableUpgrades > 0)
	{
		_jumpPower++;
		_availableUpgrades--;
	}
	else if (button == _jumpTimeDecr && _jumpTime > 0)
	{
		_jumpTime--;
		_availableUpgrades++;
	}
	else if (button == _jumpTimeIncr && _jumpTime < NUM_JUMP_TIME_UPGRADES && _availableUpgrades > 0)
	{
		_jumpTime++;
		_availableUpgrades--;
	}

	if (button == _jumpPowerDecr ||
	        button == _jumpPowerIncr ||
	        button == _jumpTimeDecr ||
	        button == _jumpTimeIncr)
	{
		updateProgressBars();
		updateAvailableUpgrades();
	}
}

void UpgradesState::updateProgressBars()
{
	int i;
	unsigned int empty = Locator::getSpriteManager()->getIndex("./data/sprites/interface.sprites", "box_empty");
	unsigned int full = Locator::getSpriteManager()->getIndex("./data/sprites/interface.sprites", "box_full");

	for (i = 0; i < NUM_JUMP_POWER_UPGRADES; i++)
	{
		if (i < _jumpPower)
		{
			_jumpPowerProgress[i]->changeSprites(full, full);
		}
		else
		{
			_jumpPowerProgress[i]->changeSprites(empty, empty);
		}
	}

	for (i = 0; i < NUM_JUMP_TIME_UPGRADES; i++)
	{
		if (i < _jumpTime)
		{
			_jumpTimeProgress[i]->changeSprites(full, full);
		}
		else
		{
			_jumpTimeProgress[i]->changeSprites(empty, empty);
		}
	}

	handleDecrIncrVisibility(_jumpPowerDecr, _jumpPowerIncr, _jumpPower, NUM_JUMP_POWER_UPGRADES);
	handleDecrIncrVisibility(_jumpTimeDecr, _jumpTimeIncr, _jumpTime, NUM_JUMP_TIME_UPGRADES);
}

void UpgradesState::handleDecrIncrVisibility(Button* decr, Button* incr, int value, int size)
{
	std::vector<Button* >::iterator it;

	if (value == size || _availableUpgrades == 0)
	{
		it = std::find(_buttons.begin(), _buttons.end(), incr);
		if (it != _buttons.end())
		{
			_buttons.erase(it);
		}
	}

	if (value == 0)
	{
		it = std::find(_buttons.begin(), _buttons.end(), decr);
		if (it != _buttons.end())
		{
			_buttons.erase(it);
		}

		it = std::find(_buttons.begin(), _buttons.end(), incr);
		if (it == _buttons.end() && _availableUpgrades > 0)
		{
			_buttons.push_back(incr);
		}
	}
	else
	{
		it = std::find(_buttons.begin(), _buttons.end(), decr);
		if (it == _buttons.end())
		{
			_buttons.push_back(decr);
		}

		it = std::find(_buttons.begin(), _buttons.end(), incr);
		if (it == _buttons.end() && _availableUpgrades > 0 && value != size)
		{
			_buttons.push_back(incr);
		}
	}
}

void UpgradesState::updateAvailableUpgrades()
{
	char num[16];
	sprintf(num, "Upgrades: %i", _availableUpgrades);
	_upgradesLeft->setText(std::string(num));
}

void UpgradesState::getModifiers(int* jumpPower, int* jumpTime, unsigned int* bullets, int* timeToSniper, unsigned int* energy)
{
	*jumpPower = _jumpPower;
	*jumpTime = _jumpTime;
	*bullets = _availableBullets;
	*timeToSniper = _availableTimeToSniper;
	*energy = _availableEnergy;
}

void UpgradesState::setMap(std::string mapFilename)
{
	_mapFilename = mapFilename;
	_manager->getMapVariables(_mapFilename, &_availableMoney, &_availableUpgrades, &_availableBullets, &_availableTimeToSniper, &_availableEnergy);
	updateAvailableUpgrades();
	updateProgressBars();
}