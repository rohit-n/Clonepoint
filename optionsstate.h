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
	void handleDecrIncrVisibility(std::shared_ptr<Button> decr, std::shared_ptr<Button> incr, int value, int size);
private:
	std::shared_ptr<TextLabel> _titleLabel;
	std::shared_ptr<TextButton> _exitButton;
	std::shared_ptr<TextButton> _saveChangesButton;

	std::shared_ptr<FloatingMessage> _saveMessage;

	std::vector<std::string> _modes;
	int _modeIndex;
	bool _bindingMode;
	eBinding _bindToChange;
	int _currVolume;

	//gameplay page
	std::shared_ptr<TextButton> _toBindingsPage;
	std::shared_ptr<ImageButton> _lightEnteredAlphaState;
	std::shared_ptr<TextLabel> _lightEnteredAlphaText;
	std::shared_ptr<TextLabel> _resolutionText;
	std::shared_ptr<TextLabel> _resolutionLabel;
	std::shared_ptr<TextLabel> _volumeLabel;
	std::shared_ptr<ImageButton> _resUp;
	std::shared_ptr<ImageButton> _resDown;
	std::shared_ptr<TextLabel> _fullscreenLabel;
	std::shared_ptr<ImageButton> _fullscreenState;
	std::shared_ptr<ImageButton> _volumeDecr;
	std::shared_ptr<ImageButton> _volumeIncr;
	std::shared_ptr<ImageButton> _volumeProgress[NUM_VOLUME_BARS];
	std::shared_ptr<TextLabel> _tutorialPopupsText;
	std::shared_ptr<TextLabel> _inputPopupsText;
	std::shared_ptr<ImageButton> _tutorialPopupsState;
	std::shared_ptr<ImageButton> _inputPopupsState;

	//bindings page
	std::shared_ptr<TextLabel> _pressAKeyLabel;
	std::shared_ptr<TextLabel> _moveLeftLabel;
	std::shared_ptr<TextLabel> _moveRightLabel;
	std::shared_ptr<TextLabel> _moveUpLabel;
	std::shared_ptr<TextLabel> _moveDownLabel;
	std::shared_ptr<TextLabel> _autoSaveMessage;
	std::shared_ptr<TextButton> _moveLeftButton;
	std::shared_ptr<TextButton> _moveRightButton;
	std::shared_ptr<TextButton> _moveUpButton;
	std::shared_ptr<TextButton> _moveDownButton;
	std::shared_ptr<TextButton> _toGameplayPage;

	std::vector<std::shared_ptr<Button> > _bindingsPage;
	std::vector<std::shared_ptr<Button> > _gameplayPage;
};

#endif