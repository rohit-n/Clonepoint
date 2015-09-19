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

#ifndef LOADMAPSTATE_H
#define LOADMAPSTATE_H
#include "menustate.h"

class LoadMapState : public MenuState
{
public:
	LoadMapState(StateManager* sm);
	~LoadMapState();
	void handleButton(Button* button);
	void handleMouseWheel(int dir);
	void nextPage();
	void prevPage();
	void resetPositions(int w, int h);
private:
	TextLabel* _chooseLabel;
	TextLabel* _pageLabel;
	TextButton* _cancelButton;
	ImageButton* _prevButton;
	ImageButton* _nextButton;
	std::vector<std::vector<Button*> > _pages;
	size_t _currPageIndex;
	void handlePageChange();
};

#endif