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

#ifndef SPRITE_H
#define SPRITE_H
#include <SDL2/SDL.h>
#include <vector>
#include "texture.h"

class SpriteSheet
{
public:
	SpriteSheet(const char* filename, int tileDim, bool flip);
	~SpriteSheet();
	GLint getWidth();
	GLint getHeight();
	GLuint getTexId();
	int getTileDim();
	void getClipPosition(unsigned int index, float* x, float* y);
	unsigned int getNumberOfSprites();
	GLuint* getIndexBuffers();
	GLuint getVertexBuffer();
	void setVertexBuffer(GLuint vbo);
private:
	GLuint _tex;
	GLuint _vertexBuffer;
	GLuint* _indexBuffers;
	GLint _width;
	GLint _height;
	int _tileDim;
	std::vector<SDL_Rect> _clips;
	unsigned int _numberOfSprites;
	unsigned int _widthInSprites;
};

class AnimationSequence
{
public:
	AnimationSequence(bool looping);
	~AnimationSequence();
	void addSpriteNumber(unsigned int index);
	void setFramesPerSecond(int framesPerSecond);
	unsigned int getSpriteAt(size_t i);
	void setSpriteIndex(unsigned int index);
	bool readFromFile(const char* filename);
	bool isLooping();
	size_t getNumSprites();
	float getMsPerFrame();
private:
	bool _looping;
	bool _finished;
	float _millisecondsPerFrame;
	float _savedMsPerFrame;
	int _framesPerSecond;
	std::vector<unsigned int> sequence;
	unsigned int currentSpriteIndex;
};
#endif