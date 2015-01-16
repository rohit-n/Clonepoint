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

#ifndef AUDIO_H
#define AUDIO_H
#include <AL/al.h>
#include <AL/alc.h>
#include <SDL2/SDL_audio.h>
#include <cstdio>
#include <cstring>
#include <map>
#include <vector>
#include <string>
#define MAX_SOURCES 10

class AudioManager
{
public:
	AudioManager();
	~AudioManager();
	void playSound(std::string filename);
	void playSound3D(const char* filename, float x, float y, float z);
	void loadWAV(std::string filename);
	ALuint getBuffer(std::string filename);
	ALuint getNextAvailableSource();
	void setVolume(float volume);
private:
	std::map<std::string, ALuint> _loadedBuffers;
	std::vector<ALuint> _loadedSources;
	ALCdevice* _device;
	ALCcontext* _context;
};

#endif
