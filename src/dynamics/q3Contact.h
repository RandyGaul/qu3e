//--------------------------------------------------------------------------------------------------
/**
@file	q3Contact.h

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

#ifndef Q3CONTACT_H
#define Q3CONTACT_H

#include "../math/q3Math.h"
#include "../common/q3Settings.h"
#include "../collision/q3Box.h"
#include "../collision/q3Collide.h"

//--------------------------------------------------------------------------------------------------
// q3Contact
//--------------------------------------------------------------------------------------------------
class q3Body;
struct q3Box;
struct q3ContactConstraint;

// Restitution mixing. The idea is to use the maximum bounciness, so bouncy
// objects will never not bounce during collisions.
inline r32 q3MixRestitution( const q3Box* A, const q3Box* B )
{
	return q3Max( A->restitution, B->restitution );
}

// Friction mixing. The idea is to allow a very low friction value to
// drive down the mixing result. Example: anything slides on ice.
inline r32 q3MixFriction( const q3Box* A, const q3Box* B )
{
	return std::sqrt( A->friction * B->friction );
}

// in stands for "incoming"
// out stands for "outgoing"
// I stands for "incident"
// R stands for "reference"
// See D. Gregorius GDC 2015 on creating contacts for more details
// Each feature pair is used to cache solutions from one physics tick to another. This is
// called warmstarting, and lets boxes stack and stay stable. Feature pairs identify points
// of contact over multiple physics ticks. Each feature pair is the junction of an incoming
// feature and an outgoing feature, usually a result of clipping routines. The exact info
// stored in the feature pair can be arbitrary as long as the result is a unique ID for a
// given intersecting configuration.
union q3FeaturePair
{
	struct
	{
		u8 inR;
		u8 outR;
		u8 inI;
		u8 outI;
	};

	i32 key;
};

struct q3Contact
{
	q3Vec3 position;			// World coordinate of contact
	r32 penetration;			// Depth of penetration from collision
	r32 normalImpulse;			// Accumulated normal impulse
	r32 tangentImpulse[ 2 ];	// Accumulated friction impulse
	r32 bias;					// Restitution + baumgarte
	r32 normalMass;				// Normal constraint mass
	r32 tangentMass[ 2 ];		// Tangent constraint mass
	q3FeaturePair fp;			// Features on A and B for this contact
	u8 warmStarted;				// Used for debug rendering
};

struct q3Manifold
{
	void SetPair( q3Box *a, q3Box *b );

	q3Box *A;
	q3Box *B;

	q3Vec3 normal;				// From A to B
	q3Vec3 tangentVectors[ 2 ];	// Tangent vectors
	q3Contact contacts[ 8 ];
	i32 contactCount;

	q3Manifold* next;
	q3Manifold* prev;

	bool sensor;
};

struct q3ContactEdge
{
	q3Body *other;
	q3ContactConstraint *constraint;
	q3ContactEdge* next;
	q3ContactEdge* prev;
};

struct q3ContactConstraint
{
	void SolveCollision( void );

	q3Box *A, *B;
	q3Body *bodyA, *bodyB;

	q3ContactEdge edgeA;
	q3ContactEdge edgeB;
	q3ContactConstraint* next;
	q3ContactConstraint* prev;

	r32 friction;
	r32 restitution;

	q3Manifold manifold;

	enum
	{
		eColliding    = 0x00000001, // Set when contact collides during a step
		eWasColliding = 0x00000002, // Set when two objects stop colliding
		eIsland       = 0x00000004, // For internal marking during island forming
	};

	i32 m_flags;

	friend class q3ContactManager;
	friend class q3Scene;
	friend struct q3Island;
	friend struct q3ContactSolver;
};

#endif // Q3CONTACT_H
