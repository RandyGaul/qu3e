//--------------------------------------------------------------------------------------------------
/**
@file	Demo.h

@author	Randy Gaul
@date	11/25/2014

	Copyright (c) 2014 Randy Gaul http://www.randygaul.net

	This software is provided 'as-is', without any express or implied
	warranty. In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:
		1. The origin of this software must not be misrepresented; you must not
			 claim that you wrote the original software. If you use this software
			 in a product, an acknowledgment in the product documentation would be
			 appreciated but is not required.
		2. Altered source versions must be plainly marked as such, and must not
			 be misrepresented as being the original software.
		3. This notice may not be removed or altered from any source distribution.
*/
//--------------------------------------------------------------------------------------------------

#ifndef DEMO_H
#define DEMO_H

#include "../src/q3.h"

// Base class for running demos to show off q3
struct Demo
{
	virtual ~Demo( ) {}

	virtual void Init( ) {};
	virtual void Update( ) {};
	virtual void Shutdown( ) {};

	virtual void Render( q3Render *debugDrawer ) { (void)debugDrawer; }
	virtual void KeyDown( unsigned char key ) { (void)key; }
	virtual void KeyUp( unsigned char key ) { (void)key; }
	virtual void LeftClick( i32 x, i32 y ) { (void)x; (void)y; }
};

// Is frame by frame stepping enabled?
extern bool paused;

// Can the simulation take a step, while paused is enabled?
extern bool singleStep;

// Globals for running the scene
extern float dt;
extern q3Scene scene;
int InitApp( int argc, char** argv );

// Globals for maintaining a list of demos
extern i32 demoCount;
extern i32 currentDemo;
#define Q3_DEMO_MAX_COUNT 64
extern Demo* demos[ Q3_DEMO_MAX_COUNT ];

#endif // DEMO_H
