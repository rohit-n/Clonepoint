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

#include "file.h"
#include "draw.h"
#include "matrix.h"
#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"
#include "scene.h"
#include "statemanager.h"
#include "optionsstate.h"
#include "locator.h"
#include "config.h"

SDL_Window* screen;
SDL_GLContext context;
Renderer renderer;

bool fullscreen;
int winX, winY;

std::string fontFilename;
std::unique_ptr<StateManager> sm;

void init(void)
{
	winX = 800;
	winY = 600;
	fullscreen = false;

	std::unique_ptr<AudioManager> manager(new AudioManager());
	Locator::provide(std::move(manager));

	std::unique_ptr<StaticSpriteManager> spriteManager(new StaticSpriteManager());
	Locator::provide(std::move(spriteManager));

	std::unique_ptr<AnimationManager> animationManager(new AnimationManager());
	Locator::provide(std::move(animationManager));

	std::unique_ptr<ConfigManager> configManager(new ConfigManager());
	Locator::provide(std::move(configManager));

	std::unique_ptr<BindingsManager> bindingsManager(new BindingsManager());
	Locator::provide(std::move(bindingsManager));

	srand(time(nullptr));
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
	SDL_SetWindowFullscreen(screen, Locator::getConfigManager()->getValue("fullscreen") == "1");
}

void loop1()
{
	/*
	Uint32 fps_lasttime = SDL_GetTicks(); //the last recorded time.
	Uint32 fps_current; //the current FPS.
	Uint32 fps_frames = 0; //frames passed since the last recorded fps.
	*/

	Uint32 old_time;
	Uint32 curr_time;
	old_time = SDL_GetTicks();
	float delta = 0.0f;
	while (!sm->getActiveState()->isQuitting())
	{
		sm->getActiveState()->handleInput();

		curr_time = SDL_GetTicks();

		// while ((curr_time - old_time) < 15)
		// {
		// curr_time = SDL_GetTicks();
		// SDL_Delay(curr_time - old_time);
		// }

		delta += (curr_time - old_time);
		old_time = curr_time;

		while (delta > 15)
		{
			sm->update(15);
			renderer.updateLinkProgress(15);
			delta -= 15;
		}

		draw_frame();
		/*
		fps_frames++;
		if (fps_lasttime < SDL_GetTicks() - 1000)
		{
			fps_lasttime = SDL_GetTicks();
			fps_current = fps_frames;
			LOGF((stdout, "FPS = %i\n", fps_current));
			fps_frames = 0;
		}
		*/
	}
}

void loop2()
{
	Uint32 old_time;

	old_time = SDL_GetTicks();

	unsigned int dT;
	unsigned int accumulator = 0;

	while (!sm->getActiveState()->isQuitting())
	{
		sm->getActiveState()->handleInput();
		dT = SDL_GetTicks() - old_time;
		accumulator += dT;
		if (accumulator >= 15)
		{
			sm->update(accumulator);
			accumulator = 0;
			draw_frame();
		}

		old_time = SDL_GetTicks();
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

	renderer.init(winX, winY);

	renderer.setScreenshotIndex(atoi(Locator::getConfigManager()->getValue("screenshot_index").c_str()));

	sm.reset(new StateManager());
	sm->setWindowDims(winX, winY);
	Locator::getAudio()->setVolume((float)atoi(Locator::getConfigManager()->getValue("volume").c_str()) / 10);
	sm->registerScreenshotFunctions(renderer.getScreenshotFunc());
	sm->registerSettingsChange(handleSettingsChange);

	loop1();

	quit();
	return 0;
}