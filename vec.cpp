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

#include "vec.h"

float vec2f_distance(vec2f a, vec2f b)
{
	float xdiff = b.x - a.x;
	float ydiff = b.y - a.y;
	return sqrtf(xdiff * xdiff + ydiff * ydiff);
}

vec2f vec2f_normalize(vec2f v)
{
	vec2f ret;
	float length = v.length();

	if (length == 0.0f)
	{
		length = 1.0f;
	}

	ret.x = v.x / length;
	ret.y = v.y / length;

	return ret;
}

//vec3f
vec3f vec3f_init(float x, float y, float z)
{
	vec3f ret;

	ret.x = x;
	ret.y = y;
	ret.z = z;

	return ret;
}

float vec3f_dot(vec3f a, vec3f b)
{
	return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
}

float vec3f_length(vec3f v)
{
	return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}

vec3f vec3f_cross(vec3f a, vec3f b)
{
	vec3f ret;
	ret.x = (a.y * b.z) - (a.z * b.y);
	ret.y = (a.z * b.x) - (a.x * b.z);
	ret.z = (a.x * b.y) - (a.y * b.x);

	return ret;
}

vec3f vec3f_normalize(vec3f v)
{
	vec3f ret;
	float length = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);

	if (length == 0.0f)
	{
		length = 1.0f;
	}

	ret.x = v.x / length;
	ret.y = v.y / length;
	ret.z = v.z / length;

	return ret;
}

vec3f vec3f_add(vec3f a, vec3f b)
{
	vec3f ret;

	ret.x = a.x + b.x;
	ret.y = a.y + b.y;
	ret.z = a.z + b.z;

	return ret;
}

vec3f vec3f_subtract(vec3f a, vec3f b)
{
	vec3f ret;

	ret.x = a.x - b.x;
	ret.y = a.y - b.y;
	ret.z = a.z - b.z;

	return ret;
}

vec3f vec3f_mult(vec3f v, float s)
{
	vec3f ret;

	ret.x = v.x * s;
	ret.y = v.y * s;
	ret.z = v.z * s;

	return ret;
}

quatf axis_angle_to_quat(vec3f axis, float angle)
{
	quatf ret;
	float sin_angle = sinf(angle / 2);

	ret.q[0] = axis.x * sin_angle;
	ret.q[1] = axis.y * sin_angle;
	ret.q[2] = axis.z * sin_angle;


	ret.q[3] = cosf(angle / 2);

	return ret;
}

quatf quatf_inverse(quatf q)
{
	float length;
	quatf ret;

	length = 1.0f / ((q.q[0] * q.q[0]) +
	                 (q.q[1] * q.q[1]) +
	                 (q.q[2] * q.q[2]) +
	                 (q.q[3] * q.q[3]));

	ret.q[0] = q.q[0] * -length;
	ret.q[1] = q.q[1] * -length;
	ret.q[2] = q.q[2] * -length;
	ret.q[3] = q.q[3] * length;

	return ret;
}

quatf quatf_mult(quatf a, quatf b)
{
	vec3f v1, v2, cross;
	quatf ret;

	v1 = vec3f_init(a.q[0], a.q[1], a.q[2]);
	v2 = vec3f_init(b.q[0], b.q[1], b.q[2]);

	cross = vec3f_cross(v1, v2);

	v1.x *= b.q[3];
	v2.x *= a.q[3];

	v1.y *= b.q[3];
	v2.y *= a.q[3];

	v1.z *= b.q[3];
	v2.z *= a.q[3];

	ret.q[0] = v1.x + v2.x + cross.x;
	ret.q[1] = v1.y + v2.y + cross.y;
	ret.q[2] = v1.z + v2.z + cross.z;
	ret.q[3] = (a.q[3] * b.q[3]) - vec3f_dot(v1, v2);

	return ret;
}

vec3f quatf_mult_vec3f(quatf q, vec3f v)
{
	vec3f ret;
	quatf vec2quat, quat_inverse, res;

	vec2quat.q[0] = v.x;
	vec2quat.q[1] = v.y;
	vec2quat.q[2] = v.z;
	vec2quat.q[3] = 0.0f;

	quat_inverse = quatf_inverse(q);
	res = quatf_mult(vec2quat, quat_inverse);
	res = quatf_mult(q, res);

	ret.x = res.q[0];
	ret.y = res.q[1];
	ret.z = res.q[2];

	return ret;
}

vec3f vec3f_rotate_around_axis(vec3f v, vec3f axis, float angle)
{
	vec3f ret;
	angle = ToRadian(angle);
	quatf q = axis_angle_to_quat(axis, angle);
	ret = quatf_mult_vec3f(q, v);
	return ret;
}