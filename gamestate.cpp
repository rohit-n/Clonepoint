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

#include "gamestate.h"
#include "statemanager.h"

GameState::GameState(StateManager* sm) : BaseState(sm)
{
	_playerMovingLeft = _playerMovingRight = _playerMovingDown = _playerMovingUp = false;
	_scene.reset();
	_movementLocked = false;
	_LMBHeldDown = false;
	_RMBHeldDown = false;
	_saveMessage.reset(new FloatingMessage(0, 0, "", 0, 1, 0));
	_labels.push_back(_saveMessage);
}

GameState::~GameState()
{
}

void GameState::deleteScene()
{
	_scene.reset();
}

void GameState::initSceneAndMap(const char* filename)
{
	_scene.reset(new Scene());
	_scene->loadMap(filename, false);
	_saveMessage->setText("");
}

void GameState::update(unsigned int dT)
{
	Player* player = _scene->getPlayer();
	if (_LMBHeldDown && player->isAlive() && !player->isPinning() &&  !player->isAimingGun() && !player->isHacking() && !player->isMovingThroughStairs() && !player->isAnimatingThroughStairs())
	{
		_scene->handleClick(_mouseX, _mouseY, dT);
		_movementLocked = true;
	}
	else
	{
		_movementLocked = false;
	}

	if (!_LMBHeldDown)
	{
		player->setAimingGun(_RMBHeldDown);
	}

	_scene->crosslinkPan(_mouseX, _mouseY);
	_scene->handleMouse(_mouseX, _mouseY);

	if (player->isAlive())
	{
		if (_movementLocked ||
		        (player->isPinning() && player->getNumPunches() == 0) ||
		        player->isHacking() ||
		        player->isGoingUpRoof() ||
		        player->isAnimatingThroughStairs() ||
		        player->isMovingThroughStairs() ||
		        (player->isInElevator() && player->getElevatorDoor()->getShaft()->isMoving()) ||
		        _scene->isLoadMenuVisible())
		{
			_playerMovingLeft = _playerMovingRight = _playerMovingDown = _playerMovingUp = false;
		}
		else
		{
			if ((_playerMovingLeft || _playerMovingRight) && player->isInElevator() && !player->getElevatorDoor()->getShaft()->isMoving())
			{
				player->leaveElevator();
			}
		}
		_scene->movePlayer(_playerMovingLeft, _playerMovingRight, _playerMovingDown, _playerMovingUp);
	}

	_scene->update(dT);

	if (_scene->isLevelOver())
	{
		_playerMovingLeft = _playerMovingRight = _playerMovingDown = _playerMovingUp = false;
		_manager->switchToState(LEVELEND_SCREEN);
	}

	_saveMessage->update(dT);
}

Scene* GameState::getScene()
{
	return _scene.get();
}

void GameState::handleMouseWheel(int dir)
{
	if (dir != 0)
		_scene->toggleCrosslinkMode();
}

void GameState::handleKeyDown(SDL_Keycode key)
{
	eBinding binding = Bind_Nothing;
	binding = Locator::getBindingsManager()->getBindingFromKey(key);
	switch (binding)
	{
	case Bind_MoveLeft:
		_playerMovingLeft = true;
		break;
	case Bind_MoveRight:
		_playerMovingRight = true;
		break;
	case Bind_MoveUp:
		_playerMovingUp = true;
		_scene->handlePlayerFrob(true);
		break;
	case Bind_MoveDown:
		_playerMovingDown = true;
		_scene->handlePlayerFrob(false);
		break;
	default:
		break;
	}
}

void GameState::handleKeyUp(SDL_Keycode key)
{
	eBinding binding = Bind_Nothing;
	switch(key)
	{
	case SDLK_ESCAPE:
		if (!_scene->isLoadMenuVisible())
		{
			_manager->switchToState(PAUSE_SCREEN);
		}
		break;
#ifdef DEBUG
	case SDLK_n:
		_scene->addNoise(_mouseX, _mouseY, 512, true, ALERT_RUN, NULL);
		break;
#endif
	case SDLK_PRINTSCREEN:
		_takeScreenshot();
		break;
	case SDLK_F5:
		_scene->saveGame("quick.sav");
		_saveMessage->init(_manager->getWindowWidth() - 128, 64, "Saved.", 3000);
		break;
	case SDLK_F9:
		_scene->loadGame("quick.sav");
		_saveMessage->setText("");
		break;
	case SDLK_F8:
		_scene->showLoadMenu(true);
		break;
	default:
		break;
	}

	binding = Locator::getBindingsManager()->getBindingFromKey(key);

	switch (binding)
	{
	case Bind_MoveLeft:
		_playerMovingLeft = false;
		break;
	case Bind_MoveRight:
		_playerMovingRight = false;
		break;
	case Bind_MoveUp:
		_playerMovingUp = false;
		break;
	case Bind_MoveDown:
		_playerMovingDown = false;
		break;
	case Bind_ToggleCrosslink:
		_scene->toggleCrosslinkMode();
		break;
	default:
		break;
	}

	if (_scene->isLoadMenuVisible())
	{
		switch(_event.key.keysym.sym)
		{
		case SDLK_1:
			_scene->loadAutosave(0);
			break;
		case SDLK_2:
			_scene->loadAutosave(1);
			break;
		case SDLK_3:
			_scene->loadAutosave(2);
			break;
		case SDLK_ESCAPE:
			_scene->showLoadMenu(false);
			break;
		case SDLK_r:
			_scene->reloadMap();
			break;
		default:
			break;
		}
	}

	if (_scene->isDetachmentDelayed() || _scene->isElevatorLocked())
	{
		switch (binding)
		{
		case Bind_MoveLeft:
		case Bind_MoveRight:
		case Bind_MoveUp:
		case Bind_MoveDown:
			_scene->resetDelayDetachment();
			_scene->resetElevatorLock();
		default:
			break;
		}
	}
}

void GameState::handleMouseDown(SDL_MouseButtonEvent event)
{
	if (event.button == SDL_BUTTON_LEFT)
	{
		_LMBHeldDown = true;
		if (_scene->getPlayer()->isAlive() && _scene->getPlayer()->isPinning())
		{
			_scene->getPlayer()->punchPinnedEnemy();
		}
		if (_scene->getPlayer()->isAlive() && _scene->getPlayer()->isAimingGun())
		{
			_scene->traceBullet(_scene->getPlayer(), vec2f(_mouseX + _scene->getCamera().x, _mouseY + _scene->getCamera().y), Shot_FromPlayer, true);
		}
	}
	else if (event.button == SDL_BUTTON_RIGHT)
	{
		_RMBHeldDown = true;
	}
}

void GameState::handleMouseUp(SDL_MouseButtonEvent event)
{
	if (event.button == SDL_BUTTON_LEFT)
	{
		_LMBHeldDown = false;
		if (_scene->getPlayer()->isAlive())
		{
			_scene->handleLeftClickRelease(_event.button.x, _event.button.y);
		}
	}
	else if (event.button == SDL_BUTTON_MIDDLE)
	{
#ifdef DEBUG
		_scene->warpPlayerTo(event.x, event.y);
#endif
	}
	else if (event.button == SDL_BUTTON_RIGHT)
	{
		_RMBHeldDown = false;
	}
}

bool GameState::isMouseCursorSeen()
{
	if (_scene)
	{
		return _scene->isMouseCursorSeen(_mouseX, _mouseY);
	}
	return false;
}