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
#include <dirent.h>
#include <string>
#include "loadmapstate.h"
#include "statemanager.h"
#include "global.h"
#define MAX_ITEMS_PER_PAGE 8

LoadMapState::LoadMapState(StateManager* sm) : MenuState(sm)
{
	_currPageIndex = 0;

	std::vector<Button*> currPage;

	_chooseLabel = new TextLabel(0, 0, "Select a map to load.", 1, 1, 1);
	_pageLabel = new TextLabel(0, 0, "1/1", 1, 1, 1);
	_labels.push_back(_chooseLabel);
	_labels.push_back(_pageLabel);

	_cancelButton = new TextButton(0, 0, (strlen("Go Back") + 2) * 16, 32, "Go Back");

	_prevButton = new ImageButton(0, 0, 32, 32, Locator::getSpriteManager()->getIndex("./data/sprites/interface.sprites", "arrowleft"),
	                                  Locator::getSpriteManager()->getIndex("./data/sprites/interface.sprites", "arrowleft_selected"));
	_nextButton = new ImageButton(0, 0, 32, 32, Locator::getSpriteManager()->getIndex("./data/sprites/interface.sprites", "arrowright"),
	                                  Locator::getSpriteManager()->getIndex("./data/sprites/interface.sprites", "arrowright_selected"));

	DIR *dir = opendir("./data");
	struct dirent *ent;
	std::vector<std::string> nameList;

	if (dir != NULL)
	{
		while ((ent = readdir (dir)) != NULL)
		{
			if (strlen(ent->d_name) > 4 &&
			        strcmp(ent->d_name, "template.tmx") &&
			        ent->d_name[strlen(ent->d_name) - 1] == 'x' &&
			        ent->d_name[strlen(ent->d_name) - 2] == 'm' &&
			        ent->d_name[strlen(ent->d_name) - 3] == 't' &&
			        ent->d_name[strlen(ent->d_name) - 4] == '.')
			{
				nameList.push_back(std::string(ent->d_name));
			}
		}
		closedir (dir);
	}
	else
	{
		LOGF((stderr, "ERROR: Could not load directory ./data."));
	}

	std::sort(nameList.begin(), nameList.end());
	std::vector<std::string>::iterator it;

	for (it = nameList.begin(); it != nameList.end(); ++it)
	{
		currPage.push_back(new TextButton(0, 0, ((*it).length() + 2) * 16, 32, *it));
		if (currPage.size() == MAX_ITEMS_PER_PAGE)
		{
			currPage.push_back(_cancelButton);
			currPage.push_back(_prevButton);
			currPage.push_back(_nextButton);
			_pages.push_back(currPage);
			currPage.clear();
		}
	}

	if (!currPage.empty())
	{
		currPage.push_back(_cancelButton);
		currPage.push_back(_prevButton);
		currPage.push_back(_nextButton);
		_pages.push_back(currPage);
		currPage.clear();
	}
	handlePageChange();
}

LoadMapState::~LoadMapState()
{
}

void LoadMapState::resetPositions(int w, int h)
{
	size_t i, j;
	int iy = 102;
	Button* button;
	_chooseLabel->setPosition((w * 0.5) - 168, h * 0.05f);
	_cancelButton->setPosition(w * 0.45f, h * 0.85f);
	_prevButton->setPosition(w * 0.4f, h * 0.07f);
	_pageLabel->setPositionWithOffset(w * 0.5, h * 0.07f, 0, 24);
	_nextButton->setPosition(w * 0.6f, h * 0.07f);

	for (i = 0; i < _pages.size(); i++)
	{
		for (j = 0; j < _pages[i].size(); j++)
		{
			button = _pages[i][j];
			if (button != _cancelButton && button != _prevButton && button != _nextButton)
			{
				button->setPositionWithOffset(w * 0.45f, 0, 0, iy);
				iy += 32;
			}
		}
		iy = 102;
	}
}

void LoadMapState::handleButton(Button* button)
{
	if (button == _cancelButton)
	{
		_manager->switchToState(MAINMENU_SCREEN);
	}
	else if (button == _prevButton)
	{
		prevPage();
	}
	else if (button == _nextButton)
	{
		nextPage();
	}
	else
	{
		std::string fullpath = std::string("data/") + std::string((static_cast<TextButton*>(button))->getText());
		_manager->setActiveMapFilename(fullpath);
		_manager->switchToState(UPGRADES_SCREEN);
	}
}

void LoadMapState::handleMouseWheel(int dir)
{
	if (dir < 0)
		nextPage();
	else
		prevPage();
}

void LoadMapState::nextPage()
{
	if (_currPageIndex < _pages.size() - 1)
	{
		_currPageIndex++;
	}
	handlePageChange();
}

void LoadMapState::prevPage()
{
	if (_currPageIndex > 0)
	{
		_currPageIndex--;
	}
	handlePageChange();
}

void LoadMapState::handlePageChange()
{
	_buttons = _pages[_currPageIndex];
	char str[16];
#ifdef _WIN32
	sprintf(str, "%i/%i", _currPageIndex + 1, _pages.size());
#else
	sprintf(str, "%lu/%lu", _currPageIndex + 1, _pages.size());
#endif
	_pageLabel->setText(std::string(str));
}