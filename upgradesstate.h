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
	void handleDecrIncrVisibility(std::shared_ptr<Button> decr, std::shared_ptr<Button> incr, int value, int size);

	std::shared_ptr<TextLabel> _titleLabel;
	std::shared_ptr<TextLabel> _jumpTimeLabel;
	std::shared_ptr<TextLabel> _jumpPowerLabel;
	std::shared_ptr<TextLabel> _upgradesLeft;

	std::shared_ptr<ImageButton> _jumpPowerDecr;
	std::shared_ptr<ImageButton> _jumpPowerIncr;
	std::shared_ptr<ImageButton> _jumpTimeDecr;
	std::shared_ptr<ImageButton> _jumpTimeIncr;

	std::shared_ptr<TextButton> _exitButton;
	std::shared_ptr<TextButton> _startMapButton;

	std::shared_ptr<ImageButton> _jumpPowerProgress[NUM_JUMP_POWER_UPGRADES];
	std::shared_ptr<ImageButton> _jumpTimeProgress[NUM_JUMP_TIME_UPGRADES];

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