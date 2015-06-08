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

#include <stdlib.h>

#include "q3Scene.h"
#include "../dynamics/q3Body.h"
#include "../dynamics/q3Island.h"
#include "../dynamics/q3Contact.h"
#include "../dynamics/q3Island.h"
#include "../dynamics/q3ContactSolver.h"
#include "../collision/q3Box.h"

//--------------------------------------------------------------------------------------------------
// q3Scene
//--------------------------------------------------------------------------------------------------
q3Scene::q3Scene( r32 dt, const q3Vec3& gravity, i32 iterations )
	: m_contactManager( &m_stack )
	, m_boxAllocator( sizeof( q3Box ), 256 )
	, m_bodyCount( 0 )
	, m_bodyList( NULL )
	, m_gravity( gravity )
	, m_dt( dt )
	, m_iterations( iterations )
	, m_newBox( false )
	, m_allowSleep( true )
	, m_enableFriction( true )
{
}

//--------------------------------------------------------------------------------------------------
q3Scene::~q3Scene( )
{
	Shutdown( );
}

//--------------------------------------------------------------------------------------------------
void q3Scene::Step( )
{
	if ( m_newBox )
	{
		m_contactManager.m_broadphase.UpdatePairs( );
		m_newBox = false;
	}

	m_contactManager.TestCollisions( );

	for ( q3Body* body = m_bodyList; body; body = body->m_next )
		body->m_flags &= ~q3Body::eIsland;

	for ( q3ContactConstraint* c = m_contactManager.m_contactList; c; c = c->next )
		c->m_flags &= ~q3ContactConstraint::eIsland;

	// Size the stack island, pick worst case size
	q3Island island;
	island.m_bodyCapacity = m_bodyCount;
	island.m_contactCapacity = m_contactManager.m_contactCount;
	island.m_bodies = (q3Body**)m_stack.Allocate( sizeof( q3Body* ) * m_bodyCount );
	island.m_velocities = (q3VelocityState *)m_stack.Allocate( sizeof( q3VelocityState ) * m_bodyCount );
	island.m_contacts = (q3ContactConstraint **)m_stack.Allocate( sizeof( q3ContactConstraint* ) * island.m_contactCapacity );
	island.m_contactStates = (q3ContactConstraintState *)m_stack.Allocate( sizeof( q3ContactConstraintState ) * island.m_contactCapacity );
	island.m_allowSleep = m_allowSleep;
	island.m_enableFriction = m_enableFriction;
	island.m_bodyCount = 0;
	island.m_contactCount = 0;
	island.m_dt = m_dt;
	island.m_gravity = m_gravity;
	island.m_iterations = m_iterations;

	// Build each active island and then solve each built island
	i32 stackSize = m_bodyCount;
	q3Body** stack = (q3Body**)m_stack.Allocate( sizeof( q3Body* ) * stackSize );
	for ( q3Body* seed = m_bodyList; seed; seed = seed->m_next )
	{
		// Seed cannot be apart of an island already
		if ( seed->m_flags & q3Body::eIsland )
			continue;

		// Seed must be awake
		if ( !(seed->m_flags & q3Body::eAwake) )
			continue;

		// Seed cannot be a static body in order to keep islands
		// as small as possible
		if ( seed->m_flags & q3Body::eStatic )
			continue;

		i32 stackCount = 0;
		stack[ stackCount++ ] = seed;
		island.m_bodyCount = 0;
		island.m_contactCount = 0;

		// Mark seed as apart of island
		seed->m_flags |= q3Body::eIsland;

		// Perform DFS on constraint graph
		while( stackCount > 0 )
		{
			// Decrement stack to implement iterative backtracking
			q3Body *body = stack[ --stackCount ];
			island.Add( body );

			// Awaken all bodies connected to the island
			body->SetToAwake( );

			// Do not search across static bodies to keep island
			// formations as small as possible, however the static
			// body itself should be apart of the island in order
			// to properly represent a full contact
			if ( body->m_flags & q3Body::eStatic )
				continue;

			// Search all contacts connected to this body
			q3ContactEdge* contacts = body->m_contactList;
			for ( q3ContactEdge* edge = contacts; edge; edge = edge->next )
			{
				q3ContactConstraint *contact = edge->constraint;

				// Skip contacts that have been added to an island already
				if ( contact->m_flags & q3ContactConstraint::eIsland )
					continue;

				// Can safely skip this contact if it didn't actually collide with anything
				if ( !(contact->m_flags & q3ContactConstraint::eColliding) )
					continue;

				// Skip sensors
				if ( contact->A->sensor || contact->B->sensor )
					continue;

				// Mark island flag and add to island
				contact->m_flags |= q3ContactConstraint::eIsland;
				island.Add( contact );

				// Attempt to add the other body in the contact to the island
				// to simulate contact awakening propogation
				q3Body* other = edge->other;
				if ( other->m_flags & q3Body::eIsland )
					continue;

				assert( stackCount < stackSize );

				stack[ stackCount++ ] = other;
				other->m_flags |= q3Body::eIsland;
			}
		}

		assert( island.m_bodyCount != 0 );

		island.Initialize( );
		island.Solve( );

		// Reset all static island flags
		// This allows static bodies to participate in other island formations
		for ( i32 i = 0; i < island.m_bodyCount; i++ )
		{
			q3Body *body = island.m_bodies[ i ];

			if ( body->m_flags & q3Body::eStatic )
				body->m_flags &= ~q3Body::eIsland;
		}
	}

	m_stack.Free( stack );
	m_stack.Free( island.m_contactStates );
	m_stack.Free( island.m_contacts );
	m_stack.Free( island.m_velocities );
	m_stack.Free( island.m_bodies );

	// Update the broadphase AABBs
	for ( q3Body* body = m_bodyList; body; body = body->m_next )
	{
		if ( body->m_flags & q3Body::eStatic )
			continue;

		body->SynchronizeProxies( );
	}

	// Look for new contacts
	m_contactManager.FindNewContacts( );

	// Clear all forces
	for ( q3Body* body = m_bodyList; body; body = body->m_next )
	{
		q3Identity( body->m_force );
		q3Identity( body->m_torque );
	}
}

//--------------------------------------------------------------------------------------------------
q3Body* q3Scene::CreateBody( const q3BodyDef& def )
{
	q3Body* body = (q3Body*)m_heap.Allocate( sizeof( q3Body ) );
	new (body) q3Body( def, this );

	// Add body to scene bodyList
	body->m_prev = NULL;
	body->m_next = m_bodyList;

	if ( m_bodyList )
		m_bodyList->m_prev = body;

	m_bodyList = body;
	++m_bodyCount;

	return body;
}

//--------------------------------------------------------------------------------------------------
void q3Scene::RemoveBody( q3Body* body )
{
	assert( m_bodyCount > 0 );

	m_contactManager.RemoveContactsFromBody( body );

	body->RemoveAllBoxes( );

	// Remove body from scene bodyList
	if ( body->m_next )
		body->m_next->m_prev = body->m_prev;

	if ( body->m_prev )
		body->m_prev->m_next = body->m_next;

	if ( body == m_bodyList )
		m_bodyList = body->m_next;

	--m_bodyCount;

	m_heap.Free( body );
}

//--------------------------------------------------------------------------------------------------
void q3Scene::RemoveAllBodies( )
{
	q3Body* body = m_bodyList;

	while ( body )
	{
		q3Body* next = body->m_next;

		body->RemoveAllBoxes( );

		m_heap.Free( body );

		body = next;
	}

	m_bodyList = NULL;
}

//--------------------------------------------------------------------------------------------------
void q3Scene::SetAllowSleep( bool allowSleep )
{
	m_allowSleep = allowSleep;

	if ( !allowSleep )
	{
		for ( q3Body* body = m_bodyList; body; body = body->m_next )
			body->SetToAwake( );
	}
}

//--------------------------------------------------------------------------------------------------
void q3Scene::SetIterations( i32 iterations )
{
	m_iterations = q3Max( 1, iterations );
}

//--------------------------------------------------------------------------------------------------
void q3Scene::SetEnableFriction( bool enabled )
{
	m_enableFriction = enabled;
}

//--------------------------------------------------------------------------------------------------
void q3Scene::Render( q3Render* render ) const
{
	q3Body* body = m_bodyList;

	while ( body )
	{
		body->Render( render );
		body = body->m_next;
	}

	m_contactManager.RenderContacts( render );
	//m_contactManager.m_broadphase.m_tree.Render( render );
}

//--------------------------------------------------------------------------------------------------
const q3Vec3 q3Scene::GetGravity( ) const
{
	return m_gravity;
}

//--------------------------------------------------------------------------------------------------
void q3Scene::SetGravity( const q3Vec3& gravity )
{
	m_gravity = gravity;
}

//--------------------------------------------------------------------------------------------------
void q3Scene::Shutdown( )
{
	RemoveAllBodies( );

	m_boxAllocator.Clear( );
}

//--------------------------------------------------------------------------------------------------
void q3Scene::SetContactListener( q3ContactListener* listener )
{
	m_contactManager.m_contactListener = listener;
}

//--------------------------------------------------------------------------------------------------
void q3Scene::QueryAABB( q3QueryCallback *cb, const q3AABB& aabb ) const
{
	struct SceneQueryWrapper
	{
		bool TreeCallBack( i32 id )
		{
			q3AABB aabb;
			q3Box *box = (q3Box *)broadPhase->m_tree.GetUserData( id );

			box->ComputeAABB( box->body->GetTransform( ), &aabb );

			if ( q3AABBtoAABB( m_aabb, aabb ) )
			{
				return cb->ReportShape( box );
			}

			return true;
		}

		q3QueryCallback *cb;
		const q3BroadPhase *broadPhase;
		q3AABB m_aabb;
	};

	SceneQueryWrapper wrapper;
	wrapper.m_aabb = aabb;
	wrapper.broadPhase = &m_contactManager.m_broadphase;
	wrapper.cb = cb;
	m_contactManager.m_broadphase.m_tree.Query( &wrapper, aabb );
}

//--------------------------------------------------------------------------------------------------
void q3Scene::QueryPoint( q3QueryCallback *cb, const q3Vec3& point ) const
{
	struct SceneQueryWrapper
	{
		bool TreeCallBack( i32 id )
		{
			q3Box *box = (q3Box *)broadPhase->m_tree.GetUserData( id );

			if ( box->TestPoint( box->body->GetTransform( ), m_point ) )
			{
				cb->ReportShape( box );
			}

			return true;
		}

		q3QueryCallback *cb;
		const q3BroadPhase *broadPhase;
		q3Vec3 m_point;
	};

	SceneQueryWrapper wrapper;
	wrapper.m_point = point;
	wrapper.broadPhase = &m_contactManager.m_broadphase;
	wrapper.cb = cb;
	const r32 k_fattener = r32( 0.5 );
	q3Vec3 v( k_fattener, k_fattener, k_fattener );
	q3AABB aabb;
	aabb.min = point - v;
	aabb.max = point + v;
	m_contactManager.m_broadphase.m_tree.Query( &wrapper, aabb );
}

//--------------------------------------------------------------------------------------------------
void q3Scene::RayCast( q3QueryCallback *cb, q3RaycastData& rayCast ) const
{
	struct SceneQueryWrapper
	{
		bool TreeCallBack( i32 id )
		{
			q3Box *box = (q3Box *)broadPhase->m_tree.GetUserData( id );

			if ( box->Raycast( box->body->GetTransform( ), m_rayCast ) )
			{
				return cb->ReportShape( box );
			}

			return true;
		}

		q3QueryCallback *cb;
		const q3BroadPhase *broadPhase;
		q3RaycastData *m_rayCast;
	};
	
	SceneQueryWrapper wrapper;
	wrapper.m_rayCast = &rayCast;
	wrapper.broadPhase = &m_contactManager.m_broadphase;
	wrapper.cb = cb;
	m_contactManager.m_broadphase.m_tree.Query( &wrapper, rayCast );
}

//--------------------------------------------------------------------------------------------------
void q3Scene::Dump( FILE* file ) const
{
	fprintf( file, "// Ensure 64/32-bit memory compatability with the dump contents\n" );
	fprintf( file, "assert( sizeof( int* ) == %d );\n", sizeof( int* ) );
	fprintf( file, "scene.SetGravity( q3Vec3( %.15lf, %.15lf, %.15lf ) );\n", m_gravity.x, m_gravity.y, m_gravity.z );
	fprintf( file, "scene.SetAllowSleep( %s );\n", m_allowSleep ? "true" : "false" );
	fprintf( file, "scene.SetEnableFriction( %s );\n", m_enableFriction ? "true" : "false" );

	fprintf( file, "q3Body** bodies = (q3Body**)q3Alloc( sizeof( q3Body* ) * %d );\n", m_bodyCount );

	i32 i = 0;
	for ( q3Body* body = m_bodyList; body; body = body->m_next, ++i )
	{
		body->Dump( file, i );
	}

	fprintf( file, "q3Free( bodies );\n" );
}
