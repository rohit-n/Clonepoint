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

#ifndef OPTIONSSTATE_H
#define OPTIONSSTATE_H
#include "menustate.h"
#include "config.h"

#define NUM_VOLUME_BARS 10

enum OptionsPage
{
	OPTION_PAGE_GAMEPLAY = 0,
	OPTION_PAGE_BINDINGS
};

class OptionsState : public MenuState
{
public:
	OptionsState(StateManager* sm);
	~OptionsState();
	void update(unsigned int dT);
	void handleButton(Button* button);
	void setLabels();
	void saveSettings();
	void addMode(std::string mode);
	void resetPositions(int w, int h);
	void changeToGameplayPage();
	void changeToBindingsPage();
	void enterBindingMode();
	void handleKeyUp(SDL_Keycode key);
	void rebindKey(SDL_Keycode key);
	void updateBindingButtons();
	void updateProgressBars();
	void handleDecrIncrVisibility(Button* decr, Button* incr, int value, int size);
	bool _settingsChanged;
private:
	TextLabel* _titleLabel;
	TextButton* _exitButton;
	TextButton* _saveChangesButton;

	FloatingMessage* _saveMessage;

	std::vector<std::string> _modes;
	int _modeIndex;
	bool _bindingMode;
	eBinding _bindToChange;
	int _currVolume;
	OptionsPage _page;

	//gameplay page
	TextButton* _toBindingsPage;
	ImageButton* _lightEnteredAlphaState;
	TextLabel* _lightEnteredAlphaText;
	TextLabel* _resolutionText;
	TextLabel* _resolutionLabel;
	TextLabel* _volumeLabel;
	ImageButton* _resUp;
	ImageButton* _resDown;
	TextLabel* _fullscreenLabel;
	ImageButton* _fullscreenState;
	ImageButton* _volumeDecr;
	ImageButton* _volumeIncr;
	ImageButton* _volumeProgress[NUM_VOLUME_BARS];
	TextLabel* _tutorialPopupsText;
	TextLabel* _inputPopupsText;
	ImageButton* _tutorialPopupsState;
	ImageButton* _inputPopupsState;

	//bindings page
	TextLabel* _pressAKeyLabel;
	TextLabel* _moveLeftLabel;
	TextLabel* _moveRightLabel;
	TextLabel* _moveUpLabel;
	TextLabel* _moveDownLabel;
	TextLabel* _autoSaveMessage;
	TextButton* _moveLeftButton;
	TextButton* _moveRightButton;
	TextButton* _moveUpButton;
	TextButton* _moveDownButton;
	TextButton* _toGameplayPage;

	std::vector<Button*> _bindingsPage;
	std::vector<Button*> _gameplayPage;
};

#endif