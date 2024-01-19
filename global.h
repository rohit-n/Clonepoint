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

#ifndef GLOBAL_H
#define GLOBAL_H

#if 0
#define LOGF(a) fprintf a
#else
#define LOGF(a) (void)0
#endif

# define MY_PI           3.14159265358979323846

#define ToRadian(x) ((x) * MY_PI / 180.0f)
#define ToDegree(x) ((x) * 180.0f / MY_PI)

#ifdef DEBUG
#define Assert(x) if (!x) { printf("Assertion failed!\n"); *(int*)0 = 0; }
#else
#define Assert(x)
#endif
#define ENTDIM 128

#define COLLISION_ACTIVE (1 << 0)
#define COLLISION_GLASS (1 << 1)
#define COLLISION_DOOR (1 << 2)
#define COLLISION_GUARDBLOCK (1 << 3) //invisible volume that makes guards turn around.

struct Rect
{
	float x;
	float y;
	float w;
	float h;
};

struct CollisionVolume
{
	Rect rect;
	unsigned int flags;
	bool active() { return flags & COLLISION_ACTIVE; }
	bool glass() { return flags & COLLISION_GLASS; }
	bool door() { return flags & COLLISION_DOOR; }
	bool guardblock() { return flags & COLLISION_GUARDBLOCK; }
};

enum Circuit
{
	BLUE = 0,
	GREEN,
	YELLOW,
	VIOLET,
	RED
};

enum Direction
{
	Left = 0,
	Right
};

enum MouseOverObject
{
	MO_CircuitBox = 0,
	MO_MainComputer,
	MO_LightFixture,
	MO_Switch,
	MO_HandScanner,
	MO_Elevator,
	MO_MotionScanner,
	MO_Door,
	MO_TrapDoor,
	MO_VaultDoor,
	MO_SoundDetector,
	MO_Alarm,
	MO_PowerSocket,
	MO_SecurityCamera,
	MO_Guard,
	MO_Enforcer,
	MO_Professional,
	MO_Sniper,
	NUMBER_OF_MOUSEOVER_OBJECTS
};

enum StringMessage
{
	SM_Start = 0,
	SM_Jumping,
	SM_Falling,
	SM_Guards1,
	SM_Guards2,
	SM_Crosslink1,
	SM_Crosslink2,
	SM_Objectives,
	SM_Elevators,
	SM_Optional,
	SM_Circuits,
	SM_ObjectivesIncomplete,
	NUMBER_OF_STRING_MESSAGES
};

#endif