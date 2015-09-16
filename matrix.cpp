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

#include "matrix.h"

mat4f mat4f_mult(mat4f a, mat4f b)
{
	mat4f ret;
	int i, j;

	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 4; j++)
		{
			ret.m[i][j] =	a.m[i][0] * b.m[0][j] +
			                a.m[i][1] * b.m[1][j] +
			                a.m[i][2] * b.m[2][j] +
			                a.m[i][3] * b.m[3][j];
		}
	}
	return ret;
}

mat4f mat4f_ident(float scale)
{
	mat4f ret;
	int i, j;
	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 4; j++)
		{
			if (i == j)
			{
				ret.m[i][j] = scale;
			}
			else
			{
				ret.m[i][j] = 0.0f;
			}
		}
	}
	return ret;
}

mat4f mat4f_zero()
{
	mat4f ret;
	int i, j;
	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 4; j++)
		{
			ret.m[i][j] = 0.0f;
		}
	}
	return ret;
}

mat4f mat4f_rotate(float xRot, float yRot, float zRot)
{
	mat4f ret;
	mat4f rx, ry, rz;
	rx = ry = rz = mat4f_zero();

	const float x = ToRadian(xRot);
	const float y = ToRadian(yRot);
	const float z = ToRadian(zRot);

	rx.m[0][0] = 1.0f;
	rx.m[1][1] = cosf(x);
	rx.m[1][2] = -sinf(x);
	rx.m[2][1] = sinf(x);
	rx.m[2][2] = cosf(x);
	rx.m[3][3] = 1.0f;

	ry.m[0][0] = cosf(y);
	ry.m[0][2] = -sinf(y);
	ry.m[1][1] = 1.0f;
	ry.m[2][0] = sinf(y);
	ry.m[2][2] = cosf(y);
	ry.m[3][3] = 1.0f;

	rz.m[0][0] = cosf(z);
	rz.m[0][1] = -sinf(z);
	rz.m[1][0] = sinf(z);
	rz.m[1][1] = cosf(z);
	rz.m[2][2] = 1.0f;
	rz.m[3][3] = 1.0f;

	ret = mat4f_mult(rz, ry);
	ret = mat4f_mult(ret, rx);

	return ret;
}

mat4f mat4f_rotate(float angle, float x, float y, float z)
{
	mat4f ret;
	float c = cosf(ToRadian(angle));
	float s = sinf(ToRadian(angle));

	ret.m[0][0] = (x * x) * (1.0f - c) + c;
	ret.m[0][1] = (x * y) * (1.0f - c) - (z * s);
	ret.m[0][2] = (x * z) * (1.0f - c) + (y * s);
	ret.m[0][3] = 0.0f;

	ret.m[1][0] = (y * x) * (1.0f - c) + (z * s);
	ret.m[1][1] = (y * y) * (1.0f - c) + c;
	ret.m[1][2] = (y * z) * (1.0f - c) - (x * s);
	ret.m[1][3] = 0.0f;

	ret.m[2][0] = (z * x) * (1.0f - c) - (y * s);
	ret.m[2][1] = (y * z) * (1.0f - c) + (x * s);
	ret.m[2][2] = (z * z) * (1.0f - c) + c;
	ret.m[2][3] = 0.0f;

	ret.m[3][0] = 0.0f;
	ret.m[3][1] = 0.0f;
	ret.m[3][2] = 0.0f;
	ret.m[3][3] = 1.0f;
	return ret;
}

mat4f mat4f_translate(float x, float y, float z)
{
	mat4f ret = mat4f_ident(1.0f);

	ret.m[0][3] = x;
	ret.m[1][3] = y;
	ret.m[2][3] = z;

	return ret;
}

mat4f mat4f_perspective(float fov, float aspect, float zNear, float zFar)
{
	mat4f ret = mat4f_zero();
	float f = 1.0f / tan(fov * MY_PI / 360.0f);
	ret.m[0][0] = f / aspect;
	ret.m[1][1] = f;
	ret.m[2][2] = -(zFar + zNear) / (zNear - zFar);
	ret.m[2][3] = 2.0f *(zFar * zNear) / (zNear - zFar);
	ret.m[3][2] = 1.0f;

	return ret;
}

mat4f mat4f_orthographic(float left, float right, float bottom, float top, float znear, float zfar)
{
	mat4f ret = mat4f_zero();

	ret.m[0][3] = -(right + left) / (right - left);
	ret.m[1][3] = -(top + bottom) / (top - bottom);
	ret.m[2][3] = -(zfar + znear) / (zfar - znear);
	ret.m[0][0] = 2.0f / (right - left);
	ret.m[1][1] = 2.0f / (top - bottom);
	ret.m[2][2] = -2.0f / (zfar - znear);
	ret.m[3][3] = 1.0f;

	return ret;
}

mat4f mat4f_camera_transform(vec3f target, vec3f up)
{
	mat4f ret = mat4f_zero();
	vec3f N = vec3f_normalize(target);
	vec3f U = vec3f_normalize(up);
	U = vec3f_cross(U, target);
	vec3f V = vec3f_cross(N, U);

	ret.m[0][0] = U.x;
	ret.m[0][1] = U.y;
	ret.m[0][2] = U.z;
	ret.m[0][3] = 0.0f;
	ret.m[1][0] = V.x;
	ret.m[1][1] = V.y;
	ret.m[1][2] = V.z;
	ret.m[1][3] = 0.0f;
	ret.m[2][0] = N.x;
	ret.m[2][1] = N.y;
	ret.m[2][2] = N.z;
	ret.m[2][3] = 0.0f;
	ret.m[3][0] = 0.0f;
	ret.m[3][1] = 0.0f;
	ret.m[3][2] = 0.0f;
	ret.m[3][3] = 1.0f;

	return ret;
}

mat4f quat2mat(quatf quat)
{
	mat4f ret;

	ret.m[0][0] = (1.0f - (2.0f * ((quat.q[1] * quat.q[1]) + (quat.q[2] * quat.q[2]))));
	ret.m[0][1] = (2.0f * ((quat.q[0] * quat.q[1]) + (quat.q[2] * quat.q[3])));
	ret.m[0][2] = (2.0f * ((quat.q[0] * quat.q[2]) - (quat.q[1] * quat.q[3])));
	ret.m[0][3] = 0.0f;
	ret.m[1][0] = (2.0f * ((quat.q[0] * quat.q[1]) - (quat.q[2] * quat.q[3])));
	ret.m[1][1] = (1.0f - (2.0f * ((quat.q[0] * quat.q[0]) + (quat.q[2] * quat.q[2]))));
	ret.m[1][2] = (2.0f * ((quat.q[1] * quat.q[2]) + (quat.q[0] * quat.q[3])));
	ret.m[1][3] = 0.0f;
	ret.m[2][0] = (2.0f * ((quat.q[0] * quat.q[2]) + (quat.q[1] * quat.q[3])));
	ret.m[2][1] = (2.0f * ((quat.q[1] * quat.q[2]) - (quat.q[0] * quat.q[3])));
	ret.m[2][2] = (1.0f - (2.0f * ((quat.q[0] * quat.q[0]) + (quat.q[1] * quat.q[1]))));
	ret.m[2][3] = 0.0f;
	ret.m[3][0] = 0.0f;
	ret.m[3][1] = 0.0f;
	ret.m[3][2] = 0.0f;
	ret.m[3][3] = 1.0f;

	return ret;
}