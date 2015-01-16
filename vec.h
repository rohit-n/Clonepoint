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

#ifndef VEC_H
#define VEC_H
#include "global.h"
#include <cmath>
#include <cstdio>

struct vec2f
{
	float x;
	float y;

	vec2f(float x = 0, float y = 0)
		: x(x), y(y)
	{

	}

	vec2f& operator=(const vec2f& a)
	{
		x = a.x;
		y = a.y;
		return *this;
	}

	vec2f operator+(const vec2f& a) const
	{
		return vec2f(a.x + x, a.y + y);
	}

	vec2f operator-(const vec2f& a) const
	{
		return vec2f(x - a.x, y - y);
	}

	vec2f operator*(const float& f) const
	{
		return vec2f(x * f, y * f);
	}

	float dot(const vec2f& a)
	{
		return (a.x * x) + (a.y * y);
	}

	float length()
	{
		return sqrtf(x * x + y * y);
	}

	float length_squared()
	{
		return (x * x) + (y * y);
	}
};

struct vec3f
{
	float x;
	float y;
	float z;
};

struct quatf
{
	float q[4];
}; //quaternion

float vec2f_distance(vec2f a, vec2f b);
vec2f vec2f_normalize(vec2f v);

vec3f vec3f_init(float x, float y, float z);
float vec3f_dot(vec3f a, vec3f b);
float vec3f_length(vec3f v);
vec3f vec3f_cross(vec3f a, vec3f b);
vec3f vec3f_normalize(vec3f v);
vec3f vec3f_add(vec3f a, vec3f b);
vec3f vec3f_subtract(vec3f a, vec3f b);
vec3f vec3f_mult(vec3f v, float s);

quatf axis_angle_to_quat(vec3f axis, float angle);
quatf quatf_inverse(quatf q);
quatf quatf_mult(quatf a, quatf b);
vec3f quatf_mult_vec3f(quatf q, vec3f v);
void  quat_to_axis_angle();
vec3f vec3f_rotate_around_axis(vec3f v, vec3f axis, float angle);
#endif