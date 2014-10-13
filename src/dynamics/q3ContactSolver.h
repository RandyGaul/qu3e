//--------------------------------------------------------------------------------------------------
/**
@file	q3ContactSolver.h

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

#ifndef Q3CONTACTSOLVER_H
#define Q3CONTACTSOLVER_H

#include "../math/q3Math.h"
#include "../common/q3Settings.h"

//--------------------------------------------------------------------------------------------------
// q3ContactSolver
//--------------------------------------------------------------------------------------------------
struct q3Island;
struct q3VelocityState;

struct q3ContactState
{
	q3Vec3 ra;					// Vector from C.O.M to contact position
	q3Vec3 rb;					// Vector from C.O.M to contact position
	r32 penetration;			// Depth of penetration from collision
	r32 normalImpulse;			// Accumulated normal impulse
	r32 tangentImpulse[ 2 ];	// Accumulated friction impulse
	r32 bias;					// Restitution + baumgarte
	r32 normalMass;				// Normal constraint mass
	r32 tangentMass[ 2 ];		// Tangent constraint mass
};

struct q3ContactConstraintState
{
	q3ContactState contacts[ 8 ];
	i32 contactCount;
	q3Vec3 tangentVectors[ 2 ];	// Tangent vectors
	q3Vec3 normal;				// From A to B
	q3Vec3 centerA;
	q3Vec3 centerB;
	q3Mat3 iA;
	q3Mat3 iB;
	r32 mA;
	r32 mB;
	r32 restitution;
	r32 friction;
	i32 indexA;
	i32 indexB;
};

struct q3ContactSolver
{
	void Initialize( q3Island *island );
	void ShutDown( void );

	void PreSolve( r32 dt );
	void Solve( void );

	q3Island *m_island;
	q3ContactConstraintState *m_contacts;
	i32 m_contactCount;
	q3VelocityState *m_velocities;

	bool m_enableFriction;
};

#endif // Q3CONTACTSOLVER_H
