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

#ifndef MAP_H
#define MAP_H

#include <memory>
#include <vector>

class TiXmlElement;

struct Line
{
	vec2f p1;
	vec2f p2;
};

class Map
{
public:
	Map();
	~Map();
	bool loadFromFile(const char* filename, bool savegame);
	size_t getNumberOfCollideVols();
	size_t getNumberOfEnts();
	size_t getNumberOfStairs();
	size_t getNumberOfStairwells();
	size_t getNumberOfLights();
	size_t getNumberOfLines();
	size_t getNumberOfLightLinks();
	size_t getNumberOfShafts();
	size_t getNumberOfEnemies();
	int indexOfCollideVol(CollisionVolume* vol);
	int indexOfEntity(Entity* ent);
	int indexOfFOV(FieldOfView* fov);

	CollisionVolume getCollideVolAt(int i);
	CollisionVolume* getCollideVolPointerAt(int i);
	Entity* getEntAt(size_t i);
	Enemy* getEnemyAt(size_t i);
	Stairs* getStairsAt(size_t i);
	Stairwell* getStairwellAt(size_t i);
	FieldOfView* getLightAt(size_t i);
	ElevatorShaft* getShaftAt(size_t i);
	Line getLineAt(size_t i);
	Line getLightLinkAt(size_t i);
	unsigned int getMapWidth();
	unsigned int getMapHeight();
	vec2f getPlayerStartPos();
	void addSavedLink(vec2f start, vec2f end);
	void clearLinks();
	GLuint getMapTexture();
	GLuint getCrosslinkTexture();
	bool subwayFound();
	vec2f getSubwayPosition();
	void getLinkableIters(std::vector<LinkableEntity*>::iterator* begin,
	                      std::vector<LinkableEntity*>::iterator* end);

	void getTutorialIters(std::vector<TutorialMark*>::iterator* begin,
	                      std::vector<TutorialMark*>::iterator* end);
	void removeEnemyGun(EnemyGun* gun);
	void addMissingGuns();
	void addSniper();
	void removeSniper();
	Enemy* getSniper();
private:
	unsigned int _tilesHigh;
	unsigned int _tilesWide;
	unsigned int _mapWidth;
	unsigned int _mapHeight;
	vec2f _playerStartPos;
	vec2f _subwayPos;
	bool _subwayFound;
	std::vector<CollisionVolume*> _collideVols;
	std::vector<Entity*> _entities;
	std::vector<Stairs*> _stairDoors;
	std::vector<Stairwell*> _stairwells;
	std::vector<FieldOfView*> _lights;
	std::vector<ElevatorShaft*> _shafts;
	std::vector<LinkableEntity*> _linkableEnts;
	std::vector<Line> _lines;
	std::vector<Line> _lightLinks;
	std::vector<size_t> _enemyIndices;
	std::vector<TutorialMark*> _tutorials;
	Enemy* _sniper;

	void parseLinkableObject(TiXmlElement* element, Circuit c);
	void parseCollisionVolume(TiXmlElement* element);
	void parseEntity(TiXmlElement* element, bool savegame);
	void parseObjective(TiXmlElement* element);
	void parseLight(TiXmlElement* element);
	void parseProp(TiXmlElement* element);
	void parsePolyline(TiXmlElement* element, float ox, float oy, std::vector<Line>* container);
	void getPolylineComponent(std::string point, float* x, float* y);
	void makeElevatorShafts();
	int doesElevatorShaftExist(int x);
	int doesStairwellExist(int x);
	void setShaftOpenings();
	void makeStairwells();
	void calculateStairDirections();
	void calculateElevatorOrder();
	void parseTileLayer(char* data);
	SDL_Surface* _tilesetImage;
	GLuint _mapTex;
	GLuint _crosslinkTex;
};
#endif