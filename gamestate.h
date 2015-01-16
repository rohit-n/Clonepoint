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

#ifndef GAMESTATE_H
#define GAMESTATE_H
#include "state.h"
#include "scene.h"
#include "global.h"
#include "bindings.h"

class GameState : public BaseState
{
public:
	GameState(StateManager* sm);
	~GameState();
	void update(unsigned int dT);
	void initSceneAndMap(const char* filename);
	void deleteScene();
	Scene* getScene();
	void handleMouseWheel(int dir);
	void handleKeyDown(SDL_Keycode);
	void handleKeyUp(SDL_Keycode);
	void handleMouseDown(SDL_MouseButtonEvent event);
	void handleMouseUp(SDL_MouseButtonEvent event);
	bool isMouseCursorSeen();
private:
	bool _playerMovingLeft, _playerMovingRight, _playerMovingDown, _playerMovingUp;
	bool _movementLocked;
	bool _LMBHeldDown, _RMBHeldDown;
	std::unique_ptr<Scene> _scene;
	std::shared_ptr<FloatingMessage> _saveMessage;
};

#endif