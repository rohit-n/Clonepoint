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

#include "entity.h"

FieldOfView::FieldOfView(float x, float y, float radius, int direction, int halfSize, bool active, FOVType type)
{
	_position.x = x;
	_position.y = y;
	_radius = radius;
	_direction = direction;
	_halfSize = halfSize;
	_active = active;
	clearVerts();

	_red = 1.0f;
	_green = 1.0f;
	_blue = 1.0f;

	_fixture = nullptr;

	_type = type;
}

FieldOfView::~FieldOfView()
{
	_verts.clear();
}

vec2f FieldOfView::getPosition()
{
	return _position;
}

float FieldOfView::getRadius()
{
	return _radius;
}

int FieldOfView::getDirection()
{
	return _direction;
}

int FieldOfView::getSize()
{
	return _halfSize;
}

bool FieldOfView::isActive()
{
	return _active;
}

void FieldOfView::addVertex(float x, float y)
{
	_verts.push_back(x);
	_verts.push_back(y);
}

void FieldOfView::changeVertex(unsigned int i, float x, float y)
{
	if (i >= _verts.size())
	{
		return;
	}
	_verts[i] = x;
	_verts[i + 1] = y;
}

float* FieldOfView::getVertData()
{
	return _verts.data();
}

int FieldOfView::getNumberOfVerts()
{
	return _verts.size() / 2;
}

void FieldOfView::rotate(int deg)
{
	_direction += deg;
	clearVerts();
}

void FieldOfView::moveTo(float x, float y)
{
	_position.x = x;
	_position.y = y;
	clearVerts();
}

void FieldOfView::clearVerts()
{
	_verts.clear();
	addVertex(_position.x, _position.y);
}

void FieldOfView::setActive(bool b)
{
	_active = b;
}

void FieldOfView::getColors(float* r, float* g, float* b)
{
	*r = _red;
	*g = _green;
	*b = _blue;
}

void FieldOfView::setColors(float r, float g, float b)
{
	_red = r;
	_green = g;
	_blue = b;
}

Rect FieldOfView::getCollisionRect()
{
	Rect rect;
	rect.x = _position.x - 16;
	rect.y = _position.y - 16;
	rect.w = 32;
	rect.h = 32;
	return rect;
}

LightFixture* FieldOfView::getLightFixture()
{
	return _fixture;
}

void FieldOfView::registerLightFixture(LightFixture* fixture)
{
	_fixture = fixture;
}

FOVType FieldOfView::getType()
{
	return _type;
}