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

#include <fstream>
#include "font.h"

Font::Font(std::string filename, float size)
{
	_atlas = 0;
	_size = size;
	unsigned char temp_bitmap[512 * 512];
	unsigned char ttf_buffer[1 << 20];
	std::ifstream input(filename.c_str());
	if (input)
	{
		input.read((char*)ttf_buffer, 1<<20);
		stbtt_BakeFontBitmap(ttf_buffer, 0, size, temp_bitmap, 512, 512, 32, 96, _cdata);
		glGenTextures(1, &_atlas);
		glBindTexture(GL_TEXTURE_2D, _atlas);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, 512,512, 0, GL_ALPHA, GL_UNSIGNED_BYTE, temp_bitmap);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
}

GLuint Font::getTexture()
{
	return _atlas;
}

stbtt_bakedchar* Font::data()
{
	return _cdata;
}

float Font::getSize()
{
	return _size;
}

Font::~Font()
{
	glDeleteTextures(1, &_atlas);
}