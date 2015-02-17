//--------------------------------------------------------------------------------------------------
/**
@file	q3BroadPhase.cpp

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

#include "q3BroadPhase.h"
#include "../collision/q3Box.h"
#include "../common/q3Geometry.h"
#include "../dynamics/q3ContactManager.h"

//--------------------------------------------------------------------------------------------------
// q3BroadPhase
//--------------------------------------------------------------------------------------------------
q3BroadPhase::q3BroadPhase( q3ContactManager *manager )
{
	m_manager = manager;

	m_pairCount = 0;
	m_pairCapacity = 64;
	m_pairBuffer = (q3ContactPair*)q3Alloc( m_pairCapacity * sizeof( q3ContactPair ) );

	m_moveCount = 0;
	m_moveCapacity = 64;
	m_moveBuffer = (i32*)q3Alloc( m_moveCapacity * sizeof( i32 ) );
}

//--------------------------------------------------------------------------------------------------
q3BroadPhase::~q3BroadPhase( )
{
	q3Free( m_moveBuffer );
	q3Free( m_pairBuffer );
}

//--------------------------------------------------------------------------------------------------
void q3BroadPhase::InsertBox( q3Box *box, const q3AABB& aabb )
{
	i32 id = m_tree.Insert( aabb, box );
	box->broadPhaseIndex = id;
	BufferMove( id );
}

//--------------------------------------------------------------------------------------------------
void q3BroadPhase::RemoveBox( const q3Box *box )
{
	m_tree.Remove( box->broadPhaseIndex );
}

//--------------------------------------------------------------------------------------------------
inline bool ContactPairSort( const q3ContactPair& lhs, const q3ContactPair& rhs )
{
	if ( lhs.A < rhs.A )
		return true;

	if ( lhs.A == rhs.A )
		return lhs.B < rhs.B;

	return false;
}

//--------------------------------------------------------------------------------------------------
void q3BroadPhase::UpdatePairs( )
{
	m_pairCount = 0;

	// Query the tree with all moving boxs
	for ( i32 i = 0; i < m_moveCount; ++i)
	{
		m_currentIndex = m_moveBuffer[ i ];
		q3AABB aabb = m_tree.GetFatAABB( m_currentIndex );

		// @TODO: Use a static and non-static tree and query one against the other.
		//        This will potentially prevent (gotta think about this more) time
		//        wasted with queries of static bodies against static bodies, and
		//        kinematic to kinematic.
		m_tree.Query( this, aabb );
	}

	// Reset the move buffer
	m_moveCount = 0;

	// Sort pairs to expose duplicates
	std::sort( m_pairBuffer, m_pairBuffer + m_pairCount, ContactPairSort );

	// Queue manifolds for solving
	{
		i32 i = 0;
		while ( i < m_pairCount )
		{
			// Add contact to manager
			q3ContactPair* pair = m_pairBuffer + i;
			q3Box *A = (q3Box*)m_tree.GetUserData( pair->A );
			q3Box *B = (q3Box*)m_tree.GetUserData( pair->B );
			m_manager->AddContact( A, B );

			++i;

			// Skip duplicate pairs by iterating i until we find a unique pair
			while ( i < m_pairCount )
			{
				q3ContactPair* potentialDup = m_pairBuffer + i;

				if ( pair->A != potentialDup->A || pair->B != potentialDup->B )
					break;

				++i;
			}
		}
	}

	m_tree.Validate( );
}

//--------------------------------------------------------------------------------------------------
void q3BroadPhase::Update( i32 id, const q3AABB& aabb )
{
	if ( m_tree.Update( id, aabb ) )
		BufferMove( id );
}

//--------------------------------------------------------------------------------------------------
bool q3BroadPhase::TestOverlap( i32 A, i32 B ) const
{
	return q3AABBtoAABB( m_tree.GetFatAABB( A ), m_tree.GetFatAABB( B ) );
}

//--------------------------------------------------------------------------------------------------
void q3BroadPhase::BufferMove( i32 id )
{
	if ( m_moveCount == m_moveCapacity )
	{
		i32* oldBuffer = m_moveBuffer;
		m_moveCapacity *= 2;
		m_moveBuffer = (i32*)q3Alloc( m_moveCapacity * sizeof( i32 ) );
		memcpy( m_moveBuffer, oldBuffer, m_moveCount * sizeof( i32 ) );
		q3Free( oldBuffer );
	}

	m_moveBuffer[ m_moveCount++ ] = id;
}
