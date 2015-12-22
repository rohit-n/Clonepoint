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
#include "optionsstate.h"
#include "statemanager.h"
#include "global.h"

OptionsState::OptionsState(StateManager* sm) : MenuState(sm)
{
	unsigned int cbs = Locator::getSpriteManager()->getIndex("./data/sprites/interface.sprites", "checkbox_selected");
	unsigned int left = Locator::getSpriteManager()->getIndex("./data/sprites/interface.sprites", "arrowleft");
	unsigned int left_selected = Locator::getSpriteManager()->getIndex("./data/sprites/interface.sprites", "arrowleft_selected");
	unsigned int right = Locator::getSpriteManager()->getIndex("./data/sprites/interface.sprites", "arrowright");
	unsigned int right_selected = Locator::getSpriteManager()->getIndex("./data/sprites/interface.sprites", "arrowright_selected");
	unsigned int empty = Locator::getSpriteManager()->getIndex("./data/sprites/interface.sprites", "box_empty");
	_settingsChanged = false;
	int i;

	_currSoundVolume = 5;
	_currMusicVolume = 5;

	_bindingMode = false;
	_bindToChange = Bind_Nothing;

	_titleLabel = new TextLabel(0, 0, "Options", 1, 1, 1);
	_exitButton = new TextButton(0, 0, (strlen("Exit") + 2) * 16, 32, "Exit");

	_fullscreenLabel = new TextLabel(0, 0, "Fullscreen", 1, 1, 1);
	_resolutionLabel = new TextLabel(0, 0, "Resolution", 1, 1, 1);
	_resolutionText = new TextLabel(0, 0, "0x0", 1, 1, 1);

	_saveChangesButton = new TextButton(0, 0, (strlen("Save Changes") + 2) * 16, 32, "Save Changes");

	_resUp = new ImageButton(0, 0, 32, 32, Locator::getSpriteManager()->getIndex("./data/sprites/interface.sprites", "arrowup"),
	                             Locator::getSpriteManager()->getIndex("./data/sprites/interface.sprites", "arrowup_selected"));
	_resDown = new ImageButton(0, 0, 32, 32, Locator::getSpriteManager()->getIndex("./data/sprites/interface.sprites", "arrowdown"),
	                               Locator::getSpriteManager()->getIndex("./data/sprites/interface.sprites", "arrowdown_selected"));
	_fullscreenState = new ImageButton(0, 0, 32, 32, cbs, cbs);
	_lightEnteredAlphaState = new ImageButton(0, 0, 32, 32, cbs, cbs);
	_tutorialPopupsState = new ImageButton(0, 0, 32, 32, cbs, cbs);
	_inputPopupsState = new ImageButton(0, 0, 32, 32, cbs, cbs);
	_lightEnteredAlphaText = new TextLabel(0, 0, "Screen flash on entering light", 1, 1, 1);
	_tutorialPopupsText = new TextLabel(0, 0, "Show tutorial popups", 1, 1, 1);
	_inputPopupsText = new TextLabel(0, 0, "Show input popups", 1, 1, 1);

	_saveMessage = new FloatingMessage(0, 0, "", 0, 1, 0);
	_toBindingsPage = new TextButton(0, 0, (strlen("Bindings") + 2) * 16, 32, "Bindings");
	_soundVolumeDecr = new ImageButton(0, 0, 32, 32, left, left_selected);
	_soundVolumeIncr = new ImageButton(0, 0, 32, 32, right, right_selected);
	_musicVolumeDecr = new ImageButton(0, 0, 32, 32, left, left_selected);
	_musicVolumeIncr = new ImageButton(0, 0, 32, 32, right, right_selected);
	_gameplayPage.push_back(_soundVolumeDecr);
	_gameplayPage.push_back(_soundVolumeIncr);
	_gameplayPage.push_back(_musicVolumeDecr);
	_gameplayPage.push_back(_musicVolumeIncr);

	for (i = 0; i < NUM_VOLUME_BARS; i++)
	{
		_soundVolumeProgress[i] = new ImageButton(0, 0, 32, 32, false, empty);
		_gameplayPage.push_back(_soundVolumeProgress[i]);
		_musicVolumeProgress[i] = new ImageButton(0, 0, 32, 32, false, empty);
		_gameplayPage.push_back(_musicVolumeProgress[i]);
	}

	_soundVolumeLabel = new TextLabel(0, 0, "Sound Volume", 1, 1, 1);
	_musicVolumeLabel = new TextLabel(0, 0, "Music Volume", 1, 1, 1);
	_crosslinkBlurLabel = new TextLabel(0, 0, "Crosslink Blur", 1, 1, 1);
	_crossLinkBlurState = new ImageButton(0, 0, 32, 32, cbs, cbs);

	_labels.push_back(_titleLabel);
	_labels.push_back(_resolutionLabel);
	_labels.push_back(_resolutionText);
	_labels.push_back(_fullscreenLabel);
	_labels.push_back(_lightEnteredAlphaText);
	_labels.push_back(_tutorialPopupsText);
	_labels.push_back(_inputPopupsText);
	_labels.push_back(_saveMessage);
	_labels.push_back(_soundVolumeLabel);
	_labels.push_back(_musicVolumeLabel);
	_labels.push_back(_crosslinkBlurLabel);

	_gameplayPage.push_back(_exitButton);
	_gameplayPage.push_back(_saveChangesButton);
	_gameplayPage.push_back(_resDown);
	_gameplayPage.push_back(_resUp);
	_gameplayPage.push_back(_fullscreenState);
	_gameplayPage.push_back(_lightEnteredAlphaState);
	_gameplayPage.push_back(_tutorialPopupsState);
	_gameplayPage.push_back(_inputPopupsState);
	_gameplayPage.push_back(_crossLinkBlurState);
	_gameplayPage.push_back(_toBindingsPage);

	//Bindings page
	_toGameplayPage = new TextButton(0, 0, (strlen("Gameplay") + 2) * 16, 32, "Gameplay");
	_moveLeftLabel = new TextLabel(0, 0, "Move Left", 1, 1, 1);
	_moveRightLabel = new TextLabel(0, 0, "Move Right", 1, 1, 1);
	_moveUpLabel = new TextLabel(0, 0, "Move Up", 1, 1, 1);
	_moveDownLabel = new TextLabel(0, 0, "Move Down", 1, 1, 1);
	_pressAKeyLabel = new TextLabel(0, 0, "Press a key to bind this action.\nPress delete to clear this binding.\nPress ESC to cancel binding.", 1, 1, 1);
	_autoSaveMessage = new TextLabel(0, 0, "Key rebindings are automatically saved.", 0.5f, 0.5f, 0.5f);
	_moveLeftButton = new TextButton(0, 0, (strlen("None") + 2) * 16, 32, "None");
	_moveRightButton = new TextButton(0, 0, (strlen("None") + 2) * 16, 32, "None");
	_moveUpButton = new TextButton(0, 0, (strlen("None") + 2) * 16, 32, "None");
	_moveDownButton = new TextButton(0, 0, (strlen("None") + 2) * 16, 32, "None");

	_bindingsPage.push_back(_exitButton);
	// _bindingsPage.push_back(_saveChangesButton);
	_bindingsPage.push_back(_toGameplayPage);
	_bindingsPage.push_back(_moveLeftButton);
	_bindingsPage.push_back(_moveRightButton);
	_bindingsPage.push_back(_moveUpButton);
	_bindingsPage.push_back(_moveDownButton);
	_labels.push_back(_moveLeftLabel);
	_labels.push_back(_moveRightLabel);
	_labels.push_back(_moveUpLabel);
	_labels.push_back(_moveDownLabel);
	_labels.push_back(_pressAKeyLabel);
	_labels.push_back(_autoSaveMessage);

	_modeIndex = 0;

	int numModes = SDL_GetNumDisplayModes(0);
	SDL_DisplayMode mode;
	char res[20];

	for (i = 0; i < numModes; i++)
	{
		if (!SDL_GetDisplayMode(0, i, &mode))
		{
			sprintf(res, "%ix%i", mode.w, mode.h);
			addMode(std::string(res));
		}
	}

	updateProgressBars();
	changeToGameplayPage();
}

OptionsState::~OptionsState()
{
	LOGF((stdout, "running Options Menu destructor!\n"));
	size_t i;
	if (_page != OPTION_PAGE_GAMEPLAY)
	{
		for (i = 0; i < NUM_VOLUME_BARS; i++)
		{
			delete _soundVolumeProgress[i];
			delete _musicVolumeProgress[i];
		}

		delete _saveChangesButton;
		delete _toBindingsPage;
		delete _soundVolumeDecr;
		delete _soundVolumeIncr;
		delete _musicVolumeDecr;
		delete _musicVolumeIncr;
		delete _inputPopupsState;
		delete _tutorialPopupsState;
		delete _lightEnteredAlphaState;
		delete _fullscreenState;
		delete _resUp;
		delete _resDown;
	}
	else //bindings page
	{
		delete _toGameplayPage;
		delete _moveLeftButton;
		delete _moveRightButton;
		delete _moveUpButton;
		delete _moveDownButton;
	}
}

void OptionsState::resetPositions(int w, int h)
{
	int x, y, i;

	x = w * 0.5;
	y = h * 0.2;

	_titleLabel->setPosition(x - 56, h * 0.08f);
	_fullscreenLabel->setPosition(w * 0.25f, y);
	_lightEnteredAlphaText->setPositionWithOffset(w * 0.25f, y, 0, 64);
	_lightEnteredAlphaState->setPositionWithOffset(w * 0.4f, y, 440, 32);

	_tutorialPopupsText->setPositionWithOffset(w * 0.25f, y, 0, 128);
	_tutorialPopupsState->setPositionWithOffset(w * 0.4f, y, 440, 96);
	_inputPopupsText->setPositionWithOffset(w * 0.25f, y, 0, 192);
	_inputPopupsState->setPositionWithOffset(w * 0.4f, y, 440, 160);

	_resolutionLabel->setPositionWithOffset(w * 0.25f, y, 0, 256);

	_fullscreenState->setPositionWithOffset(w * 0.4f, y, 440, -24);

	_resUp->setPositionWithOffset(w * 0.4f, y, 440, 208);
	_resolutionText->setPositionWithOffset(w * 0.4f, y, 408, 260);
	_resDown->setPositionWithOffset(w * 0.4f, y, 440, 272);

	_saveChangesButton->setPosition(w * 0.75f, h * 0.8f);
	_exitButton->setPosition(w * 0.75f, h * 0.85f);

	x = w * 0.4f;
	y = h * 0.6f;

	for (i = 0; i < NUM_VOLUME_BARS; i++)
	{
		_soundVolumeProgress[i]->setPositionWithOffset(x, y, 16 * i, 72);
		_musicVolumeProgress[i]->setPositionWithOffset(x, y, 16 * i, 108);
	}

	_soundVolumeDecr->setPositionWithOffset(x, y, (16 * i) + 32, 72);
	_soundVolumeIncr->setPositionWithOffset(x, y, (16 * i) + 64, 72);
	_musicVolumeDecr->setPositionWithOffset(x, y, (16 * i) + 32, 108);
	_musicVolumeIncr->setPositionWithOffset(x, y, (16 * i) + 64, 108);

	_soundVolumeLabel->setPositionWithOffset(w * 0.25f, h * 0.6f, 0, 88);
	_musicVolumeLabel->setPositionWithOffset(w * 0.25f, h * 0.6f, 0, 128);

	_crosslinkBlurLabel->setPosition(w * 0.25f, y + 32);
	_crossLinkBlurState->setPositionWithOffset(w * 0.4f, y, 440, 12);

	x = w * 0.3f;

	//bindings page
	_toGameplayPage->setPosition(w * 0.2f, h * 0.1f);
	_toBindingsPage->setPosition(w * 0.2f, h * 0.1f);
	_moveLeftLabel->setPosition(x, h * 0.2f);
	_moveRightLabel->setPosition(x, h * 0.25f);
	_moveUpLabel->setPosition(x, h * 0.3f);
	_moveDownLabel->setPosition(x, h * 0.35f);
	_moveLeftButton->setPositionWithOffset(w * 0.45f, h * 0.2f, 0, -32);
	_moveRightButton->setPositionWithOffset(w * 0.45f, h * 0.25f, 0, -32);
	_moveUpButton->setPositionWithOffset(w * 0.45f, h * 0.3f, 0, -32);
	_moveDownButton->setPositionWithOffset(w * 0.45f, h * 0.35f, 0, -32);
	_pressAKeyLabel->setPosition(w * 0.25f, h * 0.5f);
	_autoSaveMessage->setPositionWithOffset(w * 0.35f, h * 0.75f, 0, 32);
}

void OptionsState::update(unsigned int dT)
{
	MenuState::update(dT);
	_saveMessage->update(dT);
}

void OptionsState::handleButton(Button* button)
{
	if (button == _exitButton)
	{
		_manager->switchToState(MAINMENU_SCREEN);
	}
	else if (button == _fullscreenState)
	{
		toggleCheckbox(_fullscreenState);
	}
	else if (button == _crossLinkBlurState)
	{
		toggleCheckbox(_crossLinkBlurState);
	}
	else if (button == _lightEnteredAlphaState)
	{
		toggleCheckbox(_lightEnteredAlphaState);
	}
	else if (button == _tutorialPopupsState)
	{
		toggleCheckbox(_tutorialPopupsState);
	}
	else if (button == _inputPopupsState)
	{
		toggleCheckbox(_inputPopupsState);
	}
	else if (button == _resUp)
	{
		if (_modeIndex <= 0)
		{
			_modeIndex = _modes.size() - 1;
		}
		else
		{
			_modeIndex--;
		}
		_resolutionText->setText(_modes[_modeIndex]);
	}
	else if (button == _resDown)
	{
		_modeIndex++;
		if (_modeIndex >= (int)_modes.size())
		{
			_modeIndex = 0;
		}
		_resolutionText->setText(_modes[_modeIndex]);
	}
	else if (button == _soundVolumeDecr && _currSoundVolume > 0)
	{
		_currSoundVolume--;
	}
	else if (button == _soundVolumeIncr && _currSoundVolume < NUM_VOLUME_BARS)
	{
		_currSoundVolume++;
	}
	else if (button == _musicVolumeDecr && _currMusicVolume > 0)
	{
		_currMusicVolume--;
	}
	else if (button == _musicVolumeIncr && _currMusicVolume < NUM_VOLUME_BARS)
	{
		_currMusicVolume++;
	}
	else if (button == _saveChangesButton)
	{
		saveSettings();
		_settingsChanged = true;
		_saveMessage->init(_manager->getWindowWidth() - 256, 64, "Saved settings.", 2000);
	}
	else if (button == _toBindingsPage)
	{
		changeToBindingsPage();
	}
	else if (button == _toGameplayPage)
	{
		changeToGameplayPage();
	}
	else if (button == _moveLeftButton)
	{
		enterBindingMode();
		_bindToChange = Bind_MoveLeft;
	}
	else if (button == _moveRightButton)
	{
		enterBindingMode();
		_bindToChange = Bind_MoveRight;
	}
	else if (button == _moveUpButton)
	{
		enterBindingMode();
		_bindToChange = Bind_MoveUp;
	}
	else if (button == _moveDownButton)
	{
		enterBindingMode();
		_bindToChange = Bind_MoveDown;
	}

	if (button == _soundVolumeDecr ||
	    button == _soundVolumeIncr ||
		button == _musicVolumeDecr ||
		button == _musicVolumeIncr)
	{
		updateProgressBars();
	}
}

void OptionsState::setLabels()
{
	std::string res = Locator::getConfigManager()->getValue("window_x") + "x" + Locator::getConfigManager()->getValue("window_y");
	size_t i;
	bool foundRes = false;
	unsigned int cb = Locator::getSpriteManager()->getIndex("./data/sprites/interface.sprites", "checkbox");
	unsigned int cbs = Locator::getSpriteManager()->getIndex("./data/sprites/interface.sprites", "checkbox_selected");

	_saveMessage->setText("");

	if (Locator::getConfigManager()->getBool("fullscreen"))
	{
		_fullscreenState->changeSprites(cbs, cbs);
	}
	else
	{
		_fullscreenState->changeSprites(cb, cb);
	}

	if (Locator::getConfigManager()->getBool("crosslink_blur"))
	{
		_crossLinkBlurState->changeSprites(cbs, cbs);
	}
	else
	{
		_crossLinkBlurState->changeSprites(cb, cb);
	}

	if (Locator::getConfigManager()->getBool("entered_light_flash"))
	{
		_lightEnteredAlphaState->changeSprites(cbs, cbs);
	}
	else
	{
		_lightEnteredAlphaState->changeSprites(cb, cb);
	}

	if (Locator::getConfigManager()->getBool("tutorial_popups"))
	{
		_tutorialPopupsState->changeSprites(cbs, cbs);
	}
	else
	{
		_tutorialPopupsState->changeSprites(cb, cb);
	}

	if (Locator::getConfigManager()->getBool("input_popups"))
	{
		_inputPopupsState->changeSprites(cbs, cbs);
	}
	else
	{
		_inputPopupsState->changeSprites(cb, cb);
	}

	for (i = 0; i < _modes.size(); i++)
	{
		if (_modes[i] == res)
		{
			foundRes = true;
			_modeIndex = i;
			break;
		}
	}

	if (foundRes)
	{
		_resolutionText->setText(_modes[_modeIndex]);
	}
	else
	{
		_resolutionText->setText(res);
		_modeIndex = -1;
	}

	updateBindingButtons();
	_currSoundVolume = atoi(Locator::getConfigManager()->getValue("sound_volume").c_str());
	_currMusicVolume = atoi(Locator::getConfigManager()->getValue("music_volume").c_str());
	updateProgressBars();
}

void OptionsState::saveSettings()
{
	unsigned int cbs = Locator::getSpriteManager()->getIndex("./data/sprites/interface.sprites", "checkbox_selected");
	char sndvolstr[4];
	char musvolstr[4];
	sprintf(sndvolstr, "%i", _currSoundVolume);
	sprintf(musvolstr, "%i", _currMusicVolume);

	std::string resolution = _resolutionText->getText();
	std::string tokenX = resolution.substr(0, resolution.find("x"));
	std::string tokenY = resolution.substr(resolution.find("x") + 1);

	Locator::getConfigManager()->setValue("window_x", tokenX);
	Locator::getConfigManager()->setValue("window_y", tokenY);
	Locator::getConfigManager()->setValue("fullscreen", _fullscreenState->getSpriteIndex() == cbs ?  "1" : "0");
	Locator::getConfigManager()->setValue("entered_light_flash", _lightEnteredAlphaState->getSpriteIndex() == cbs ? "1" : "0");
	Locator::getConfigManager()->setValue("tutorial_popups", _tutorialPopupsState->getSpriteIndex() == cbs ? "1" : "0");
	Locator::getConfigManager()->setValue("input_popups", _inputPopupsState->getSpriteIndex() == cbs ? "1" : "0");
	Locator::getConfigManager()->setValue("sound_volume", std::string(sndvolstr));
	Locator::getConfigManager()->setValue("music_volume", std::string(musvolstr));
	Locator::getAudio()->setSoundVolume((float)_currSoundVolume / 10);
	Locator::getAudio()->setMusicVolume((float)_currMusicVolume / 10);
	Locator::getConfigManager()->setValue("crosslink_blur", _crossLinkBlurState->getSpriteIndex() == cbs ?  "1" : "0");
}

void OptionsState::addMode(std::string mode)
{
	if (std::find(_modes.begin(), _modes.end(), mode) ==  _modes.end())
	{
		_modes.push_back(mode);
	}
}

void OptionsState::changeToGameplayPage()
{
	_page = OPTION_PAGE_GAMEPLAY;
	_buttons = _gameplayPage;
	_fullscreenLabel->setVisible(true);
	_resolutionText->setVisible(true);
	_resolutionLabel->setVisible(true);
	_lightEnteredAlphaText->setVisible(true);
	_soundVolumeLabel->setVisible(true);
	_musicVolumeLabel->setVisible(true);
	_tutorialPopupsText->setVisible(true);
	_inputPopupsText->setVisible(true);
	_crosslinkBlurLabel->setVisible(true);
	//bindings
	_moveLeftLabel->setVisible(false);
	_moveRightLabel->setVisible(false);
	_moveUpLabel->setVisible(false);
	_moveDownLabel->setVisible(false);
	_pressAKeyLabel->setVisible(false);
	_autoSaveMessage->setVisible(false);
}

void OptionsState::changeToBindingsPage()
{
	_page = OPTION_PAGE_BINDINGS;
	_buttons = _bindingsPage;
	_fullscreenLabel->setVisible(false);
	_resolutionText->setVisible(false);
	_resolutionLabel->setVisible(false);
	_lightEnteredAlphaText->setVisible(false);
	_soundVolumeLabel->setVisible(false);
	_musicVolumeLabel->setVisible(false);
	_tutorialPopupsText->setVisible(false);
	_inputPopupsText->setVisible(false);
	_crosslinkBlurLabel->setVisible(false);
	//bindings
	_moveLeftLabel->setVisible(true);
	_moveRightLabel->setVisible(true);
	_moveUpLabel->setVisible(true);
	_moveDownLabel->setVisible(true);
	_autoSaveMessage->setVisible(true);
	_pressAKeyLabel->setVisible(false);
}

void OptionsState::enterBindingMode()
{
	_bindingMode = true;
	_pressAKeyLabel->setVisible(true);
}

void OptionsState::handleKeyUp(SDL_Keycode key)
{
	if (_bindingMode)
	{
		_pressAKeyLabel->setVisible(false);
		rebindKey(key);
	}
}

void OptionsState::rebindKey(SDL_Keycode key)
{
	Assert(_bindToChange != Bind_Nothing);

	if (key == SDLK_DELETE)
	{
		Locator::getBindingsManager()->clearBinding(_bindToChange);
	}
	else if (key != SDLK_ESCAPE)
	{
		Locator::getBindingsManager()->addBinding(key, _bindToChange);
	}

	_settingsChanged = true;
	updateBindingButtons();

	_bindToChange = Bind_Nothing;
	_bindingMode = false;
}

void OptionsState::updateBindingButtons()
{
	_moveLeftButton->setText(Locator::getBindingsManager()->getKeysBound(Bind_MoveLeft));
	_moveRightButton->setText(Locator::getBindingsManager()->getKeysBound(Bind_MoveRight));
	_moveUpButton->setText(Locator::getBindingsManager()->getKeysBound(Bind_MoveUp));
	_moveDownButton->setText(Locator::getBindingsManager()->getKeysBound(Bind_MoveDown));
}

void OptionsState::updateProgressBars()
{
	int i;
	unsigned int empty = Locator::getSpriteManager()->getIndex("./data/sprites/interface.sprites", "box_empty");
	unsigned int full = Locator::getSpriteManager()->getIndex("./data/sprites/interface.sprites", "box_full");

	for (i = 0; i < NUM_VOLUME_BARS; i++)
	{
		if (i < _currSoundVolume)
		{
			_soundVolumeProgress[i]->changeSprites(full, full);
		}
		else
		{
			_soundVolumeProgress[i]->changeSprites(empty, empty);
		}

		if (i < _currMusicVolume)
		{
			_musicVolumeProgress[i]->changeSprites(full, full);
		}
		else
		{
			_musicVolumeProgress[i]->changeSprites(empty, empty);
		}
	}

	if (_page == OPTION_PAGE_GAMEPLAY)
	{
		handleDecrIncrVisibility(_soundVolumeDecr,
			_soundVolumeIncr, _currSoundVolume, NUM_VOLUME_BARS);
		handleDecrIncrVisibility(_musicVolumeDecr,
			_musicVolumeIncr, _currMusicVolume, NUM_VOLUME_BARS);
	}
}

void OptionsState::handleDecrIncrVisibility(Button* decr, Button* incr, int value, int size)
{
	std::vector<Button* >::iterator it;

	if (value == size)
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
		if (it == _buttons.end())
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
		if (it == _buttons.end() && value != size)
		{
			_buttons.push_back(incr);
		}
	}
}

void OptionsState::toggleCheckbox(ImageButton* ib)
{
	unsigned int cb = Locator::getSpriteManager()->getIndex("./data/sprites/interface.sprites", "checkbox");
	unsigned int cbs = Locator::getSpriteManager()->getIndex("./data/sprites/interface.sprites", "checkbox_selected");

	if (ib->getSpriteIndex() == cb)
	{
		ib->changeSprites(cbs, cbs);
	}
	else
	{
		ib->changeSprites(cb, cb);
	}
}