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

#ifndef TEXTURE_H
#define TEXTURE_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

enum FlipType
{
	Flip_Horizontal = 0,
	Flip_Vertical
};

GLuint loadTexture(const char * filename);
SDL_Surface* loadSurfaceFromImage(const char * filename);
GLuint createTextureFromSurface(SDL_Surface* surface);
SDL_Surface* reverseSpriteSheet(SDL_Surface* surface, unsigned int tileDim);
SDL_Surface* flipSurface(SDL_Surface* surface, FlipType ft);

#endif