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

#ifndef MATRIX_H
#define MATRIX_H
#include <cmath>
#include "vec.h"

struct mat4f
{
	float m[4][4];
};

struct mat34f
{
	float m[3][4];
};

mat4f mat4f_ident(float scale);
mat4f mat4f_zero();
mat4f mat4f_mult(mat4f a, mat4f b);
mat4f mat4f_rotate(float xRot, float yRot, float zRot);
mat4f mat4f_rotate(float angle, float x, float y, float z);
mat4f mat4f_translate(float x, float y, float z);
mat4f mat4f_orthographic(float left, float right, float bottom, float top, float znear, float zfar);
mat4f mat4f_perspective(float fov, float aspect, float zNear, float zFar);
mat4f mat4f_camera_transform(vec3f target, vec3f up);
mat4f quat2mat(quatf quat);

#endif