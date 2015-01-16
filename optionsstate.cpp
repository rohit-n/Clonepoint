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
	int i;

	_currVolume = 5;

	_bindingMode = false;
	_bindToChange = Bind_Nothing;

	_titleLabel.reset(new TextLabel(0, 0, "Options", 1, 1, 1));
	_exitButton.reset(new TextButton(0, 0, (strlen("Exit") + 2) * 16, 32, "Exit"));

	_fullscreenLabel.reset(new TextLabel(0, 0, "Fullscreen", 1, 1, 1));
	_resolutionLabel.reset(new TextLabel(0, 0, "Resolution", 1, 1, 1));
	_resolutionText.reset(new TextLabel(0, 0, "0x0", 1, 1, 1));

	_saveChangesButton.reset(new TextButton(0, 0, (strlen("Save Changes") + 2) * 16, 32, "Save Changes"));

	_resUp.reset(new ImageButton(0, 0, 32, 32, Locator::getSpriteManager()->getIndex("./data/sprites/interface.sprites", "arrowup"),
	                             Locator::getSpriteManager()->getIndex("./data/sprites/interface.sprites", "arrowup_selected")));
	_resDown.reset(new ImageButton(0, 0, 32, 32, Locator::getSpriteManager()->getIndex("./data/sprites/interface.sprites", "arrowdown"),
	                               Locator::getSpriteManager()->getIndex("./data/sprites/interface.sprites", "arrowdown_selected")));
	_fullscreenState.reset(new ImageButton(0, 0, 32, 32, cbs, cbs));
	_lightEnteredAlphaState.reset(new ImageButton(0, 0, 32, 32, cbs, cbs));
	_lightEnteredAlphaText.reset(new TextLabel(0, 0, "Screen flash on entering light", 1, 1, 1));

	_saveMessage.reset(new FloatingMessage(0, 0, "", 0, 1, 0));
	_toBindingsPage.reset(new TextButton(0, 0, (strlen("Bindings") + 2) * 16, 32, "Bindings"));
	_volumeDecr.reset(new ImageButton(0, 0, 32, 32, left, left_selected));
	_volumeIncr.reset(new ImageButton(0, 0, 32, 32, right, right_selected));
	_gameplayPage.push_back(_volumeDecr);
	_gameplayPage.push_back(_volumeIncr);

	for (i = 0; i < NUM_VOLUME_BARS; i++)
	{
		_volumeProgress[i].reset(new ImageButton(0, 0, 32, 32, false, empty));
		_gameplayPage.push_back(_volumeProgress[i]);
	}

	_volumeLabel.reset(new TextLabel(0, 0, "Volume", 1, 1, 1));

	_labels.push_back(_titleLabel);
	_labels.push_back(_resolutionLabel);
	_labels.push_back(_resolutionText);
	_labels.push_back(_fullscreenLabel);
	_labels.push_back(_lightEnteredAlphaText);
	_labels.push_back(_saveMessage);
	_labels.push_back(_volumeLabel);

	_gameplayPage.push_back(_exitButton);
	_gameplayPage.push_back(_saveChangesButton);
	_gameplayPage.push_back(_resDown);
	_gameplayPage.push_back(_resUp);
	_gameplayPage.push_back(_fullscreenState);
	_gameplayPage.push_back(_lightEnteredAlphaState);
	_gameplayPage.push_back(_toBindingsPage);

	//Bindings page
	_toGameplayPage.reset(new TextButton(0, 0, (strlen("Gameplay") + 2) * 16, 32, "Gameplay"));
	_moveLeftLabel.reset(new TextLabel(0, 0, "Move Left", 1, 1, 1));
	_moveRightLabel.reset(new TextLabel(0, 0, "Move Right", 1, 1, 1));
	_moveUpLabel.reset(new TextLabel(0, 0, "Move Up", 1, 1, 1));
	_moveDownLabel.reset(new TextLabel(0, 0, "Move Down", 1, 1, 1));
	_pressAKeyLabel.reset(new TextLabel(0, 0, "Press a key to bind this action.\nPress delete to clear this binding.\nPress ESC to cancel binding.", 1, 1, 1));
	_autoSaveMessage.reset(new TextLabel(0, 0, "Key rebindings are automatically saved.", 0.5f, 0.5f, 0.5f));
	_moveLeftButton.reset(new TextButton(0, 0, (strlen("None") + 2) * 16, 32, "None"));
	_moveRightButton.reset(new TextButton(0, 0, (strlen("None") + 2) * 16, 32, "None"));
	_moveUpButton.reset(new TextButton(0, 0, (strlen("None") + 2) * 16, 32, "None"));
	_moveDownButton.reset(new TextButton(0, 0, (strlen("None") + 2) * 16, 32, "None"));

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
}

void OptionsState::resetPositions(int w, int h)
{
	int x, y, i;

	x = w * 0.5;
	y = h * 0.3;

	_titleLabel->setPosition(x - 56, h * 0.08f);
	_fullscreenLabel->setPosition(w * 0.25f, y);
	_lightEnteredAlphaText->setPositionWithOffset(w * 0.25f, y, 0, 64);
	_lightEnteredAlphaState->setPositionWithOffset(w * 0.4f, y, 440, 32);

	_resolutionLabel->setPositionWithOffset(w * 0.25f, y, 0, 128);
	_fullscreenState->setPositionWithOffset(w * 0.4f, y, 440, -24);

	_resUp->setPositionWithOffset(w * 0.4f, y, 440, 80);
	_resolutionText->setPositionWithOffset(w * 0.4f, y, 408, 132);
	_resDown->setPositionWithOffset(w * 0.4f, y, 440, 144);

	_saveChangesButton->setPosition(w * 0.45f, h * 0.75f);
	_exitButton->setPosition(x, h * 0.8f);

	x = w * 0.4f;
	y = h * 0.6f;

	for (i = 0; i < NUM_VOLUME_BARS; i++)
	{
		_volumeProgress[i]->setPositionWithOffset(x, y, 16 * i, 0);
	}

	_volumeDecr->setPositionWithOffset(x, y, (16 * i) + 32, 0);
	_volumeIncr->setPositionWithOffset(x, y, (16 * i) + 64, 0);

	_volumeLabel->setPositionWithOffset(w * 0.25f, h * 0.6f, 0, 24);

	//bindings page
	_toGameplayPage->setPosition(w * 0.2f, h * 0.1f);
	_toBindingsPage->setPosition(w * 0.2f, h * 0.1f);
	_moveLeftLabel->setPosition(w * 0.1f, h * 0.2f);
	_moveRightLabel->setPosition(w * 0.1f, h * 0.25f);
	_moveUpLabel->setPosition(w * 0.1f, h * 0.3f);
	_moveDownLabel->setPosition(w * 0.1f, h * 0.35f);
	_moveLeftButton->setPositionWithOffset(w * 0.25f, h * 0.2f, 0, -32);
	_moveRightButton->setPositionWithOffset(w * 0.25f, h * 0.25f, 0, -32);
	_moveUpButton->setPositionWithOffset(w * 0.25f, h * 0.3f, 0, -32);
	_moveDownButton->setPositionWithOffset(w * 0.25f, h * 0.35f, 0, -32);
	_pressAKeyLabel->setPosition(w * 0.4f, h * 0.4f);
	_autoSaveMessage->setPositionWithOffset(w * 0.35f, h * 0.75f, 0, 32);
}

void OptionsState::update(unsigned int dT)
{
	MenuState::update(dT);
	_saveMessage->update(dT);
}

void OptionsState::handleButton(Button* button)
{
	unsigned int cb = Locator::getSpriteManager()->getIndex("./data/sprites/interface.sprites", "checkbox");
	unsigned int cbs = Locator::getSpriteManager()->getIndex("./data/sprites/interface.sprites", "checkbox_selected");

	if (button == _exitButton.get())
	{
		_manager->switchToState(MAINMENU_SCREEN);
	}
	else if (button == _fullscreenState.get())
	{
		if (_fullscreenState->getSpriteIndex() == cb)
		{
			_fullscreenState->changeSprites(cbs, cbs);
		}
		else
		{
			_fullscreenState->changeSprites(cb, cb);
		}
	}
	else if (button == _lightEnteredAlphaState.get())
	{
		if (_lightEnteredAlphaState->getSpriteIndex() == cb)
		{
			_lightEnteredAlphaState->changeSprites(cbs, cbs);
		}
		else
		{
			_lightEnteredAlphaState->changeSprites(cb, cb);
		}
	}
	else if (button == _resUp.get())
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
	else if (button == _resDown.get())
	{
		_modeIndex++;
		if (_modeIndex >= (int)_modes.size())
		{
			_modeIndex = 0;
		}
		_resolutionText->setText(_modes[_modeIndex]);
	}
	else if (button == _volumeDecr.get() && _currVolume > 0)
	{
		_currVolume--;
	}
	else if (button == _volumeIncr.get() && _currVolume < NUM_VOLUME_BARS)
	{
		_currVolume++;
	}
	else if (button == _saveChangesButton.get())
	{
		saveSettings();
		_manager->changeSettings();
		_saveMessage->init(_manager->getWindowWidth() - 256, 64, "Saved settings.", 2000);
	}
	else if (button == _saveChangesButton.get())
	{
		saveSettings();
		_manager->changeSettings();
		_saveMessage->init(_manager->getWindowWidth() - 256, 64, "Saved settings.", 2000);
	}
	else if (button == _saveChangesButton.get())
	{
		saveSettings();
		_manager->changeSettings();
		_saveMessage->init(_manager->getWindowWidth() - 256, 64, "Saved settings.", 2000);
	}
	else if (button == _toBindingsPage.get())
	{
		changeToBindingsPage();
	}
	else if (button == _toGameplayPage.get())
	{
		changeToGameplayPage();
	}
	else if (button == _moveLeftButton.get())
	{
		enterBindingMode();
		_bindToChange = Bind_MoveLeft;
	}
	else if (button == _moveRightButton.get())
	{
		enterBindingMode();
		_bindToChange = Bind_MoveRight;
	}
	else if (button == _moveUpButton.get())
	{
		enterBindingMode();
		_bindToChange = Bind_MoveUp;
	}
	else if (button == _moveDownButton.get())
	{
		enterBindingMode();
		_bindToChange = Bind_MoveDown;
	}

	if (button == _volumeDecr.get() ||
	        button == _volumeIncr.get())
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

	if (Locator::getConfigManager()->getValue("fullscreen") == "1")
	{
		_fullscreenState->changeSprites(cbs, cbs);
	}
	else
	{
		_fullscreenState->changeSprites(cb, cb);
	}

	if (Locator::getConfigManager()->getValue("entered_light_flash") == "1")
	{
		_lightEnteredAlphaState->changeSprites(cbs, cbs);
	}
	else
	{
		_lightEnteredAlphaState->changeSprites(cb, cb);
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
	_currVolume = atoi(Locator::getConfigManager()->getValue("volume").c_str());
	updateProgressBars();
}

void OptionsState::saveSettings()
{
	unsigned int cbs = Locator::getSpriteManager()->getIndex("./data/sprites/interface.sprites", "checkbox_selected");
	char volstr[4];
	sprintf(volstr, "%i", _currVolume);

	std::string resolution = _resolutionText->getText();
	std::string tokenX = resolution.substr(0, resolution.find("x"));
	std::string tokenY = resolution.substr(resolution.find("x") + 1);

	Locator::getConfigManager()->setValue("window_x", tokenX);
	Locator::getConfigManager()->setValue("window_y", tokenY);
	Locator::getConfigManager()->setValue("fullscreen", _fullscreenState->getSpriteIndex() == cbs ?  "1" : "0");
	Locator::getConfigManager()->setValue("entered_light_flash", _lightEnteredAlphaState->getSpriteIndex() == cbs ? "1" : "0");
	Locator::getConfigManager()->setValue("volume", std::string(volstr));
	Locator::getAudio()->setVolume((float)_currVolume / 10);
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
	_buttons = _gameplayPage;
	_fullscreenLabel->setVisible(true);
	_resolutionText->setVisible(true);
	_resolutionLabel->setVisible(true);
	_lightEnteredAlphaText->setVisible(true);
	_volumeLabel->setVisible(true);
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
	_buttons = _bindingsPage;
	_fullscreenLabel->setVisible(false);
	_resolutionText->setVisible(false);
	_resolutionLabel->setVisible(false);
	_lightEnteredAlphaText->setVisible(false);
	_volumeLabel->setVisible(false);
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
		if (i < _currVolume)
		{
			_volumeProgress[i]->changeSprites(full, full);
		}
		else
		{
			_volumeProgress[i]->changeSprites(empty, empty);
		}
	}

	handleDecrIncrVisibility(_volumeDecr, _volumeIncr, _currVolume, NUM_VOLUME_BARS);
}

void OptionsState::handleDecrIncrVisibility(std::shared_ptr<Button> decr, std::shared_ptr<Button> incr, int value, int size)
{
	std::vector<std::shared_ptr<Button> >::iterator it;

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