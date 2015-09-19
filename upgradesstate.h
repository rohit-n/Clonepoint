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

#ifndef UPGRADESSTATE_H
#define UPGRADESSTATE_H
#include "menustate.h"

#define NUM_JUMP_POWER_UPGRADES 10
#define NUM_JUMP_TIME_UPGRADES 5

class UpgradesState : public MenuState
{
public:
	UpgradesState(StateManager* sm);
	~UpgradesState();
	void update(unsigned int dT);
	void handleButton(Button* button);
	void resetPositions(int w, int h);
	void setMap(std::string mapFilename);
	void getModifiers(int* jumpPower, int* jumpTime, unsigned int* bullets, int* timeToSniper, unsigned int* energy);
private:
	void updateProgressBars();
	void updateAvailableUpgrades();
	void handleDecrIncrVisibility(Button* decr, Button* incr, int value, int size);

	TextLabel* _titleLabel;
	TextLabel* _jumpTimeLabel;
	TextLabel* _jumpPowerLabel;
	TextLabel* _upgradesLeft;

	ImageButton* _jumpPowerDecr;
	ImageButton* _jumpPowerIncr;
	ImageButton* _jumpTimeDecr;
	ImageButton* _jumpTimeIncr;

	TextButton* _exitButton;
	TextButton* _startMapButton;

	ImageButton* _jumpPowerProgress[NUM_JUMP_POWER_UPGRADES];
	ImageButton* _jumpTimeProgress[NUM_JUMP_TIME_UPGRADES];

	std::string _mapFilename;
	int _jumpPower;
	int _jumpTime;
	int _availableMoney;
	int _availableUpgrades;
	int _availableTimeToSniper;
	unsigned int _availableBullets;
	unsigned int _availableEnergy;
};

#endif