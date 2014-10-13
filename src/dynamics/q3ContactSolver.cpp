//--------------------------------------------------------------------------------------------------
/**
@file	q3ContactSolver.cpp

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

#include "q3ContactSolver.h"
#include "q3Contact.h"
#include "q3Island.h"
#include "../common/q3Memory.h"
#include "q3Body.h"
#include "../common/q3Geometry.h"
#include "../common/q3Settings.h"

//--------------------------------------------------------------------------------------------------
// q3ContactSolver
//--------------------------------------------------------------------------------------------------
void q3ContactSolver::Initialize( q3Island *island )
{
	m_island = island;
	m_contactCount = island->m_contactCount;
	m_contacts = island->m_contactStates;
	m_velocities = m_island->m_velocities;
	m_enableFriction = island->m_enableFriction;
}

//--------------------------------------------------------------------------------------------------
void q3ContactSolver::ShutDown( void )
{
	for ( i32 i = 0; i < m_contactCount; ++i )
	{
		q3ContactConstraintState *c = m_contacts + i;
		q3ContactConstraint *cc = m_island->m_contacts[ i ];

		for ( i32 j = 0; j < c->contactCount; ++j )
		{
			q3Contact *oc = cc->manifold.contacts + j;
			q3ContactState *cs = c->contacts + j;
			oc->normalImpulse = cs->normalImpulse;
			oc->tangentImpulse[ 0 ] = cs->tangentImpulse[ 0 ];
			oc->tangentImpulse[ 1 ] = cs->tangentImpulse[ 1 ];
		}
	}
}

//--------------------------------------------------------------------------------------------------
void q3ContactSolver::PreSolve( r32 dt )
{
	for ( i32 i = 0; i < m_contactCount; ++i )
	{
		q3ContactConstraintState *cs = m_contacts + i;

		q3Vec3 vA = m_velocities[ cs->indexA ].v;
		q3Vec3 wA = m_velocities[ cs->indexA ].w;
		q3Vec3 vB = m_velocities[ cs->indexB ].v;
		q3Vec3 wB = m_velocities[ cs->indexB ].w;

		for ( i32 j = 0; j < cs->contactCount; ++j )
		{
			q3ContactState *c = cs->contacts + j;

			// Precalculate JM^-1JT for contact and friction constraints
			q3Vec3 raCn = q3Cross( c->ra, cs->normal );
			q3Vec3 rbCn = q3Cross( c->rb, cs->normal );
			r32 nm = cs->mA + cs->mB;
			r32 tm[ 2 ];
			tm[ 0 ] = nm;
			tm[ 1 ] = nm;

			nm += q3Dot( raCn, cs->iA * raCn ) + q3Dot( rbCn, cs->iB * rbCn );
			c->normalMass = q3Invert( nm );

			for ( i32 i = 0; i < 2; ++i )
			{
				q3Vec3 raCt = q3Cross( cs->tangentVectors[ i ], c->ra );
				q3Vec3 rbCt = q3Cross( cs->tangentVectors[ i ], c->rb );
				tm[ i ] += q3Dot( raCt, cs->iA * raCt ) + q3Dot( rbCt, cs->iB * rbCt );
				c->tangentMass[ i ] = q3Invert( tm[ i ] );
			}

			// Precalculate bias factor
			c->bias = -Q3_BAUMGARTE * (r32( 1.0 ) / dt) * q3Min( r32( 0.0 ), c->penetration + Q3_PENETRATION_SLOP );

			// Warm start contact
			q3Vec3 P = cs->normal * c->normalImpulse;

			if ( m_enableFriction )
			{
				P += cs->tangentVectors[ 0 ] * c->tangentImpulse[ 0 ];
				P += cs->tangentVectors[ 1 ] * c->tangentImpulse[ 1 ];
			}

			vA -= P * cs->mA;
			wA -= cs->iA * q3Cross( c->ra, P );

			vB += P * cs->mB;
			wB += cs->iB * q3Cross( c->rb, P );

			// Add in restitution bias
			r32 dv = q3Dot( vB + q3Cross( wB, c->rb ) - vA - q3Cross( wA, c->ra ), cs->normal );

			if ( dv < -r32( 1.0 ) )
				c->bias += -(cs->restitution) * dv;
		}

		m_velocities[ cs->indexA ].v = vA;
		m_velocities[ cs->indexA ].w = wA;
		m_velocities[ cs->indexB ].v = vB;
		m_velocities[ cs->indexB ].w = wB;
	}
}

//--------------------------------------------------------------------------------------------------
void q3ContactSolver::Solve( )
{
	for ( i32 i = 0; i < m_contactCount; ++i )
	{
		q3ContactConstraintState *cs = m_contacts + i;

		q3Vec3 vA = m_velocities[ cs->indexA ].v;
		q3Vec3 wA = m_velocities[ cs->indexA ].w;
		q3Vec3 vB = m_velocities[ cs->indexB ].v;
		q3Vec3 wB = m_velocities[ cs->indexB ].w;

		for ( i32 j = 0; j < cs->contactCount; ++j )
		{
			q3ContactState *c = cs->contacts + j;

			// relative velocity at contact
			q3Vec3 dv = vB + q3Cross( wB, c->rb ) - vA - q3Cross( wA, c->ra );

			// Friction
			if ( m_enableFriction )
			{
				for ( i32 i = 0; i < 2; ++i )
				{
					r32 lambda = -q3Dot( dv, cs->tangentVectors[ i ] ) * c->tangentMass[ i ];

					// Calculate frictional impulse
					r32 maxLambda = cs->friction * c->normalImpulse;

					// Clamp frictional impulse
					r32 oldPT = c->tangentImpulse[ i ];
					c->tangentImpulse[ i ] = q3Clamp( -maxLambda, maxLambda, oldPT + lambda );
					lambda = c->tangentImpulse[ i ] - oldPT;

					// Apply friction impulse
					q3Vec3 impulse = cs->tangentVectors[ i ] * lambda;
					vA -= impulse * cs->mA;
					wA -= cs->iA * q3Cross( c->ra, impulse );

					vB += impulse * cs->mB;
					wB += cs->iB * q3Cross( c->rb, impulse );
				}
			}

			// Normal
			{
				dv = vB + q3Cross( wB, c->rb ) - vA - q3Cross( wA, c->ra );

				// Normal impulse
				r32 vn = q3Dot( dv, cs->normal );

				// Factor in positional bias to calculate impulse scalar j
				r32 lambda = c->normalMass * (-vn + c->bias);

				// Clamp impulse
				r32 tempPN = c->normalImpulse;
				c->normalImpulse = q3Max( tempPN + lambda, r32( 0.0 ) );
				lambda = c->normalImpulse - tempPN;

				// Apply impulse
				q3Vec3 impulse = cs->normal * lambda;
				vA -= impulse * cs->mA;
				wA -= cs->iA * q3Cross( c->ra, impulse );

				vB += impulse * cs->mB;
				wB += cs->iB * q3Cross( c->rb, impulse );
			}
		}

		m_velocities[ cs->indexA ].v = vA;
		m_velocities[ cs->indexA ].w = wA;
		m_velocities[ cs->indexB ].v = vB;
		m_velocities[ cs->indexB ].w = wB;
	}
}
