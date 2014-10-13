//--------------------------------------------------------------------------------------------------
/**
@file	q3Scene.h

@author	Randy Gaul
@date	10/10/2014

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

#ifndef Q3SCENE_H
#define Q3SCENE_H

#include "../common/q3Settings.h"
#include "../common/q3Memory.h"
#include "../dynamics/q3ContactManager.h"

//--------------------------------------------------------------------------------------------------
// q3Scene
//--------------------------------------------------------------------------------------------------
class q3Body;
struct q3BodyDef;
struct q3ContactConstraint;
class q3Render;
struct q3Island;

class q3ContactListener
{
public:
	virtual ~q3ContactListener( )
	{
	}

	virtual void BeginContact( const q3ContactConstraint *contact ) = 0;
	virtual void EndContact( const q3ContactConstraint *contact ) = 0;
};

class q3Scene
{
public:
	q3Scene( r32 dt, const q3Vec3& gravity = q3Vec3( r32( 0.0 ), r32( -9.8 ), r32( 0.0 ) ), i32 iterations = 20 );
	~q3Scene( );

	// Run the simulation forward in time by dt (fixed timestep). Variable
	// timestep is not supported.
	void Step( );

	// Construct a new rigid body. The BodyDef can be reused at the user's
	// discretion, as no reference to the BodyDef is kept.
	q3Body* CreateBody( const q3BodyDef& def );

	// Frees a body, removes all shapes associated with the body and frees
	// all shapes and contacts associated and attached to this body.
	void RemoveBody( q3Body* body );
	void RemoveAllBodies( );

	// Enables or disables rigid body sleeping. Sleeping is an effective CPU
	// optimization where bodies are put to sleep if they don't move much.
	// Sleeping bodies sit in memory without being updated, until the are
	// touched by something that wakes them up. The default is enabled.
	void SetAllowSleep( bool allowSleep );

	// Increasing the iteration count increases the CPU cost of simulating
	// Scene.Step(). Decreasing the iterations makes the simulation less
	// realistic (convergent). A good iteration number range is 5 to 20.
	// Only positive numbers are accepted. Non-positive and negative
	// inputs set the iteration count to 1.
	void SetIterations( i32 iterations );

	// Friction occurs when two rigid bodies have shapes that slide along one
	// another. The friction force resists this sliding motion.
	void SetEnableFriction( bool enabled );

	// Render the scene with an interpolated time between the last frame and
	// the current simulation step.
	void Render( q3Render* render ) const;

	// Gets and sets the global gravity vector used during integration
	const q3Vec3 GetGravity( ) const;
	void SetGravity( const q3Vec3& gravity );

	// Removes all bodies from the scene.
	void Shutdown( );

	// Sets the listener to report collision start/end. Provides the user
	// with a pointer to an q3ContactConstraint. The q3ContactConstraint
	// holds pointers to the two shapes involved in a collision, and the
	// two bodies connected to each shape. The q3ContactListener will be
	// called very often, so it is recommended for the funciton to be very
	// efficient. Provide a NULL pointer to remove the previously set
	// listener.
	void SetContactListener( q3ContactListener* listener );

private:
	q3ContactManager m_contactManager;
	q3PagedAllocator m_boxAllocator;

	q3Island* m_islands;
	i32 m_islandCount;
	i32 m_islandCapacity;

	i32 m_bodyCount;
	q3Body* m_bodyList;

	q3Stack m_stack;
	q3Heap m_heap;

	q3Vec3 m_gravity;
	r32 m_dt;
	i32 m_iterations;

	bool m_newBox;
	bool m_allowSleep;
	bool m_enableFriction;

	friend class q3Body;
};

#endif // Q3SCENE_H
