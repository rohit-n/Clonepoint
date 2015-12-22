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

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include <time.h>

#if BUILD_UNITY
#include "animations.cpp"
#include "audio.cpp"
#include "bindings.cpp"
#include "button.cpp"
#include "config.cpp"
#include "creditsstate.cpp"
#include "draw.cpp"
#include "elevators.cpp"
#include "enemy.cpp"
#include "entity.cpp"
#include "fieldofview.cpp"
#include "file.cpp"
#include "font.cpp"
#include "gamestate.cpp"
#include "intersect.cpp"
#include "levelendstate.cpp"
#include "linkableentity.cpp"
#include "livingentity.cpp"
#include "loadingmapstate.cpp"
#include "loadmapstate.cpp"
#include "locator.cpp"
#include "map.cpp"
#include "mainmenustate.cpp"
#include "matrix.cpp"
#include "menustate.cpp"
#include "optionsstate.cpp"
#include "pausestate.cpp"
#include "player.cpp"
#include "scene.cpp"
#include "scene_guards.cpp"
#include "scene_physics.cpp"
#include "scene_saved_game.cpp"
#include "scene_trace.cpp"
#include "sprite.cpp"
#include "stairs.cpp"
#include "state.cpp"
#include "statemanager.cpp"
#include "static_sprites.cpp"
#include "texture.cpp"
#include "upgradesstate.cpp"
#include "vec.cpp"
#else
#include "file.h"
#include "draw.h"
#include "matrix.h"
#include "scene.h"
#include "statemanager.h"
#include "optionsstate.h"
#include "locator.h"
#include "config.h"
#endif

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

SDL_Window* screen;
SDL_GLContext context;
Renderer renderer;

bool fullscreen;
int winX, winY;

std::string fontFilename;
StateManager* sm;

void init(void)
{
	winX = 800;
	winY = 600;
	fullscreen = false;

	AudioManager* manager = new AudioManager();
	Locator::provide(manager);

	StaticSpriteManager* spriteManager = new StaticSpriteManager();
	Locator::provide(spriteManager);

	AnimationManager* animationManager = new AnimationManager();
	Locator::provide(animationManager);

	ConfigManager* configManager = new ConfigManager();
	Locator::provide(configManager);

	BindingsManager* bindingsManager = new BindingsManager();
	Locator::provide(bindingsManager);

	srand(time(NULL));
}

void draw_frame(void)
{
	renderer.drawState(sm->getActiveState());
	SDL_GL_SwapWindow(screen);
}

void getSettings(void)
{
	ConfigManager* cm = Locator::getConfigManager();
	BindingsManager* bm = Locator::getBindingsManager();
	cm->loadConfig("config.cfg");
	bm->loadBindingsFromConfig("config.cfg");

	std::string val;
	val = cm->getValue("window_x");
	if (val != "")
	{
		winX = atoi(val.c_str());
	}
	val = cm->getValue("window_y");
	if (val != "")
	{
		winY = atoi(val.c_str());
	}
	val = cm->getValue("fullscreen");
	if (val != "")
	{
		fullscreen = atoi(val.c_str());
	}
	val = cm->getValue("font");
	if (val != "")
	{
		fontFilename = val;
	}
}

bool initSDL(void)
{
	bool success = true;
	int flags = SDL_WINDOW_OPENGL;
	if (SDL_Init(SDL_INIT_EVERYTHING))
	{
		success = false;
	}
	else
	{
		if (fullscreen)
		{
			flags |= SDL_WINDOW_FULLSCREEN;
		}
		screen = SDL_CreateWindow("Clonepoint", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, winX, winY, flags);
		if (!screen) success = false;
	}

	context = SDL_GL_CreateContext(screen);
	if (!context)
	{
		success = false;
	}

	if (!success)
	{
		LOGF((stderr, "%s\n", SDL_GetError()));
	}
	else
	{
		SDL_GL_MakeCurrent(screen, context);
	}

	return success;
}

void quit()
{
	char scrstr[4]; //hope you don't have more than 9999 screenshots :/
	sprintf(scrstr, "%i", renderer.getScreenshotIndex());
	Locator::getConfigManager()->setValue("screenshot_index", std::string(scrstr));
	Locator::getConfigManager()->saveConfig("config.cfg", Locator::getBindingsManager()->getBindingsToSave());
	delete Locator::getAudio();
	delete Locator::getBindingsManager();
	delete Locator::getSpriteManager();
	delete Locator::getAnimationManager();
	delete Locator::getConfigManager();
	delete sm;

	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(screen);
	SDL_Quit();
}

void handleSettingsChange()
{
	renderer.handleSettingsChange();
	bool change = false;
	int newX =  atoi(Locator::getConfigManager()->getValue("window_x").c_str());
	int newY =  atoi(Locator::getConfigManager()->getValue("window_y").c_str());

	if (winX != newX)
	{
		winX = newX;
		change = true;
	}

	if (winY != newY)
	{
		winY = newY;
		change = true;
	}

	if (change)
	{
		SDL_DestroyWindow(screen);
		screen = SDL_CreateWindow("Clonepoint", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, winX, winY, SDL_WINDOW_OPENGL);
		SDL_GL_MakeCurrent(screen, context);
		renderer.setResolution(winX, winY);
		sm->setWindowDims(winX, winY);
	}
	SDL_SetWindowFullscreen(screen, Locator::getConfigManager()->getBool("fullscreen"));
}

void loop1()
{
	Uint32 old_time;
	Uint32 curr_time;
	old_time = SDL_GetTicks();
	float delta = 0.0f;
	while (!sm->getActiveState()->isQuitting())
	{
		sm->getActiveState()->handleInput();

		curr_time = SDL_GetTicks();

		delta += (curr_time - old_time);
		old_time = curr_time;

		while (delta > 15)
		{
			sm->update(15);
			renderer.updateLinkProgress(15);
			delta -= 15;
		}

		draw_frame();

		if (sm->settingsChanged())
		{
			handleSettingsChange();
			sm->resetSettingsFlag();
		}

		if (sm->screenshotTaken())
		{
			renderer.takeScreenshot();
			sm->resetScreenShotFlag();
		}

		if (sm->stateChanged())
		{
			renderer.changeMenuBackground(sm->_activeStateName);
			sm->resetStateChangedFlag();
		}
	}
}

int main(int argc, char **argv)
{
	init();
	getSettings();

	if (!initSDL())
	{
		return 1;
	}

	if (!renderer.initShaders())
	{
		LOGF((stderr, "Shaders not supported.\n"));
		return 1;
	}

	if (!renderer.init(winX, winY))
	{
		fprintf(stderr, "Error! Some files were not able to be loaded.\n\
Did you download the Clonepoint data files from rohit.itch.io/clonepoint\n\
and place them in the data/ directory?\n\n");
		quit();
		return 1;
	}

	renderer.setScreenshotIndex(atoi(Locator::getConfigManager()->getValue("screenshot_index").c_str()));

	sm = new StateManager();
	sm->setWindowDims(winX, winY);
	Locator::getAudio()->setSoundVolume((float)atoi(Locator::getConfigManager()->getValue("sound_volume").c_str()) / 10);
	Locator::getAudio()->setMusicVolume((float)atoi(Locator::getConfigManager()->getValue("music_volume").c_str()) / 10);

	loop1();

	quit();
	return 0;
}