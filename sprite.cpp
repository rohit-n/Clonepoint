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

#include <cstdio>
#include <cstdlib>
#include <fstream>
#include "sprite.h"
#include "global.h"

SpriteSheet::SpriteSheet(const char* filename, int tileDim, bool flip)
{
	_numberOfSprites = 0;
	_indexBuffers = nullptr;
	if (flip)
	{
		SDL_Surface* surf = loadSurfaceFromImage(filename);
		if (!surf)
		{
			fprintf(stderr, "Fatal: Failed to load %s\n", filename);
			_tex = 0;
			return;
		}
		surf = reverseSpriteSheet(surf, tileDim);
		_tex = createTextureFromSurface(surf);
		SDL_FreeSurface(surf);
	}
	else
	{
		_tex = loadTexture(filename);
		if (_tex == 0)
		{
			fprintf(stderr, "Fatal: Failed to load %s\n", filename);
			return;
		}
	}
	_tileDim = tileDim;
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &_width);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &_height);

	_widthInSprites = _width / tileDim;
	unsigned int heightInSprites = _height / tileDim;

	_numberOfSprites = _widthInSprites * heightInSprites;

	_indexBuffers = new GLuint[_numberOfSprites];

	_clips.resize(_numberOfSprites);

	unsigned int i, j;

	for (i = 0; i < _widthInSprites; i++)
	{
		for (j = 0; j < heightInSprites; j++)
		{
			_clips[j * _widthInSprites + i].w = tileDim;
			_clips[j * _widthInSprites + i].h = tileDim;

			_clips[j * _widthInSprites + i].x = i * tileDim;
			_clips[j * _widthInSprites + i].y = j * tileDim;
		}
	}
	LOGF((stdout, "%s is %ix%i pixels and has %i %ix%i sprites.\n", filename, _width, _height, numberOfSprites, tileDim, tileDim));
}

SpriteSheet::~SpriteSheet()
{
	LOGF((stdout, "Deleting a Sprite Resource with texture ID %i.\n", _tex));
	glDeleteTextures(1, &_tex);
	_width = 0;
	_height = 0;
	if (_indexBuffers)
	{
		delete [] _indexBuffers;
	}
}

GLint SpriteSheet::getWidth()
{
	return _width;
}

GLint SpriteSheet::getHeight()
{
	return _height;
}

GLuint SpriteSheet::getTexId()
{
	return _tex;
}

int SpriteSheet::getTileDim()
{
	return _tileDim;
}

void SpriteSheet::getClipPosition(unsigned int index, float* x, float* y)
{
	*x = _clips[index].x;
	*y = _clips[index].y;
}

unsigned int SpriteSheet::getNumberOfSprites()
{
	return _numberOfSprites;
}

GLuint* SpriteSheet::getIndexBuffers()
{
	return _indexBuffers;
}

GLuint SpriteSheet::getVertexBuffer()
{
	return _vertexBuffer;
}

void SpriteSheet::setVertexBuffer(GLuint vbo)
{
	_vertexBuffer = vbo;
}

AnimationSequence::AnimationSequence(bool looping)
{
	_looping = looping;
	_finished = false;
	_framesPerSecond = 1;
	_millisecondsPerFrame = (float)(1.0f / _framesPerSecond) * 1000;
	_savedMsPerFrame = _millisecondsPerFrame;
	currentSpriteIndex = 0;
}

AnimationSequence::~AnimationSequence()
{
	sequence.clear();
}

void AnimationSequence::addSpriteNumber(unsigned int index)
{
	sequence.push_back(index);
}

unsigned int AnimationSequence::getSpriteAt(size_t i)
{
	return sequence[i];
}

void AnimationSequence::setSpriteIndex(unsigned int index)
{
	currentSpriteIndex = index;
}

bool AnimationSequence::readFromFile(const char* filename)
{
	std::ifstream input(filename);

	if (!input)
	{
		LOGF((stderr, "Failed to read animation file %s.\n", filename));
		return false;
	}

	int anim = 0;

	int numFrames = 0;
	char string[32];
	input.getline(string, 32);
	_framesPerSecond = atoi(string);
	_millisecondsPerFrame = (float)(1.0f / _framesPerSecond) * 1000;
	_savedMsPerFrame = _millisecondsPerFrame;
	input.getline(string, 32);

	numFrames = atoi(string);

	for (int i = 0; i < numFrames; i++)
	{
		input.getline(string, 32);
		anim = atoi(string);
		addSpriteNumber(anim);
	}
	return true;
}

size_t AnimationSequence::getNumSprites()
{
	return sequence.size();
}

float AnimationSequence::getMsPerFrame()
{
	return _savedMsPerFrame;
}

bool AnimationSequence::isLooping()
{
	return _looping;
}