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

#include "texture.h"
#include "stb_image.c"
#include "global.h"

//getpixel and putpixel taken from http://sdl.beuc.net/sdl.wiki/Pixel_Access
Uint32 getpixel(SDL_Surface* surface, int x, int y)
{
	int bpp = surface->format->BytesPerPixel;

	Uint8* p = (Uint8*)surface->pixels + y * surface->pitch + x * bpp;

	switch(bpp)
	{
	case 1:
		return *p;
		break;

	case 2:
		return *(Uint16*)p;
		break;

	case 3:
		if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
			return p[0] << 16 | p[1] << 8 | p[2];
		else
			return p[0] | p[1] << 8 | p[2] << 16;
		break;

	case 4:
		return *(Uint32*)p;
		break;

	default:
		return 0;
	}
}

void putpixel(SDL_Surface* surface, int x, int y, Uint32 pixel)
{
	int bpp = surface->format->BytesPerPixel;
	Uint8* p = (Uint8*)surface->pixels + y * surface->pitch + x * bpp;

	switch(bpp)
	{
	case 1:
		*p = pixel;
		break;

	case 2:
		*(Uint16 *)p = pixel;
		break;

	case 3:
		if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
		{
			p[0] = (pixel >> 16) & 0xff;
			p[1] = (pixel >> 8) & 0xff;
			p[2] = pixel & 0xff;
		}
		else
		{
			p[0] = pixel & 0xff;
			p[1] = (pixel >> 8) & 0xff;
			p[2] = (pixel >> 16) & 0xff;
		}
		break;

	case 4:
		*(Uint32 *)p = pixel;
		break;
	}
}

GLuint loadTexture(const char* filename)
{
	GLuint id;

	int width, height, bpp;
	unsigned char* data = stbi_load(filename, &width, &height, &bpp, 0);

	if (!data)
	{
		LOGF((stderr, "Error loading texture %s!\n", filename));
		return 0;
	}

	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);

	switch (bpp)
	{
	case 3:
		glTexImage2D(GL_TEXTURE_2D, 0, bpp, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		break;
	case 4:
		glTexImage2D(GL_TEXTURE_2D, 0, bpp, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		break;
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	stbi_image_free(data);
	LOGF((stdout, "Loaded texture %s with id = %i\n", filename, id));
	return id;
}

SDL_Surface* loadSurfaceFromImage(const char* filename)
{
	int width, height, bpp;
	unsigned char* data = stbi_load(filename, &width, &height, &bpp, 0);

	if (!data)
	{
		LOGF((stderr, "Error loading image %s!\n", filename));
		return nullptr;
	}

	SDL_Surface* loadedImage = SDL_CreateRGBSurfaceFrom(data, width, height, 32, width * bpp,
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	                           0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
#else
	                           0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
#endif

	//need to copy loadedImage into another surface as SDL_CreateRGBSurfaceFrom does not copy the pixel data.
	//without copying, stbi_image_free will cause valgrind read errors when blitting from the new surface.
	SDL_Surface* copied = SDL_ConvertSurface(loadedImage, loadedImage->format, loadedImage->flags);
	SDL_FreeSurface(loadedImage);
	stbi_image_free(data);

	if(!copied)
	{
		LOGF((stderr, "Error creating surface from %s!\n", filename));
		return nullptr;
	}

	return copied;
}

GLuint createTextureFromSurface(SDL_Surface* surface)
{
	GLuint id;
	GLenum texture_format = GL_RGBA;
	Uint8 bpp = surface->format->BytesPerPixel;

	if (bpp == 4)
	{
		texture_format = surface->format->Rmask == 0x000000ff ? GL_RGBA : GL_BGRA;
	}
	else if (bpp == 3)
	{
		texture_format = surface->format->Rmask == 0x000000ff ? GL_RGB : GL_BGR;
	}
	else
	{
		Assert(false);
	}

	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);

	glTexImage2D(GL_TEXTURE_2D, 0, bpp, surface->w, surface->h, 0, texture_format, GL_UNSIGNED_BYTE, surface->pixels);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	LOGF((stdout, "Created texture with id = %i from surface.\n", id));
	return id;
}

SDL_Surface* reverseSpriteSheet(SDL_Surface* surface, unsigned int tileDim)
{
	SDL_Surface* ret, *subSurf;
	unsigned int height = surface->h / tileDim;
	unsigned int width = surface->w / tileDim;
	unsigned int i, j;
	SDL_Rect srcrect;

	srcrect.w = srcrect.h = tileDim;

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	ret = SDL_CreateRGBSurface(SDL_SWSURFACE, surface->w, surface->h, 32, 0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
	subSurf = SDL_CreateRGBSurface(SDL_SWSURFACE, tileDim, tileDim, 32, 0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
#else
	ret = SDL_CreateRGBSurface(SDL_SWSURFACE, surface->w, surface->h, 32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
	subSurf = SDL_CreateRGBSurface(SDL_SWSURFACE, tileDim, tileDim, 32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
#endif

	for (i = 0; i < width; i++)
	{
		for (j = 0; j < height; j++)
		{
			srcrect.x = i * tileDim;
			srcrect.y = j * tileDim;
			SDL_BlitSurface(surface, &srcrect, subSurf, nullptr);
			subSurf = flipSurface(subSurf, Flip_Horizontal);
			SDL_BlitSurface(subSurf, nullptr, ret, &srcrect);
		}
	}

	SDL_FreeSurface(surface);
	SDL_FreeSurface(subSurf);
	return ret;
}

SDL_Surface* flipSurface(SDL_Surface* surface, FlipType ft)
{
	int x, y;
	SDL_Surface* flipped_surface = SDL_CreateRGBSurface(SDL_SWSURFACE,
	                               surface->w,surface->h,
	                               surface->format->BitsPerPixel,
	                               surface->format->Rmask,
	                               surface->format->Gmask,
	                               surface->format->Bmask,
	                               surface->format->Amask);

	SDL_LockSurface(surface);
	SDL_LockSurface(flipped_surface);

	if (ft == Flip_Vertical)
	{
		for(y = 0; y < surface->h; y++)
		{
			for(x = 0; x < surface->w; x++)
			{
				putpixel(flipped_surface, x, y, getpixel(surface, x, surface->h - y - 1));
			}
		}
	}
	else
	{
		for(y = 0; y < surface->h; y++)
		{
			for(x = 0; x < surface->w; x++)
			{
				putpixel(flipped_surface, x, y, getpixel(surface, surface->w - x - 1, y));
			}
		}
	}

	SDL_UnlockSurface(flipped_surface);
	SDL_UnlockSurface(surface);
	SDL_FreeSurface(surface);
	return flipped_surface;
}