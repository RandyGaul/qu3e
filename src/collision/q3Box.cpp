//--------------------------------------------------------------------------------------------------
/**
@file	q3Box.cpp

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

#include "q3Box.h"
#include "../math/q3Vec3.h"

//--------------------------------------------------------------------------------------------------
// q3Box
//--------------------------------------------------------------------------------------------------
bool q3Box::TestPoint( const q3Transform& tx, const q3Vec3& p ) const
{
	return false;
}

//--------------------------------------------------------------------------------------------------
bool q3Box::Raycast( const q3Transform& tx, q3RaycastData* raycast ) const
{
	return false;
}

//--------------------------------------------------------------------------------------------------
void q3Box::ComputeAABB( const q3Transform& tx, q3AABB* aabb ) const
{
	q3Vec3 v[ 8 ] = {
		q3Vec3( -e.x, -e.y, -e.z ),
		q3Vec3( -e.x, -e.y,  e.z ),
		q3Vec3( -e.x,  e.y, -e.z ),
		q3Vec3( -e.x,  e.y,  e.z ),
		q3Vec3(  e.x, -e.y, -e.z ),
		q3Vec3(  e.x, -e.y,  e.z ),
		q3Vec3(  e.x,  e.y, -e.z ),
		q3Vec3(  e.x,  e.y,  e.z )
	};

	for ( i32 i = 0; i < 8; ++i )
		v[ i ] = q3Mul( tx, v[ i ] );

	q3Vec3 min( Q3_R32_MAX, Q3_R32_MAX, Q3_R32_MAX );
	q3Vec3 max( -Q3_R32_MAX, -Q3_R32_MAX, -Q3_R32_MAX );

	for ( i32 i = 0; i < 8; ++i )
	{
		min = q3Min( min, v[ i ] );
		max = q3Max( max, v[ i ] );
	}

	aabb->min = min;
	aabb->max = max;
}

//--------------------------------------------------------------------------------------------------
void q3Box::ComputeMass( q3MassData* md ) const
{
	// Calculate inertia tensor
	r32 ex2 = r32( 4.0 ) * e.x * e.x;
	r32 ey2 = r32( 4.0 ) * e.y * e.y;
	r32 ez2 = r32( 4.0 ) * e.y * e.y;
	r32 mass = r32( 8.0 ) * e.x * e.y * e.z * density;
	r32 x = r32( 1.0 / 12.0 ) * mass * (ey2 + ez2);
	r32 y = r32( 1.0 / 12.0 ) * mass * (ex2 + ez2);
	r32 z = r32( 1.0 / 12.0 ) * mass * (ex2 + ey2);
	q3Mat3 I = q3Diagonal( x, y, z );

	// Transform tensor to local space
	I = local.rotation * I * q3Transpose( local.rotation );
	q3Mat3 identity;
	q3Identity( identity );
	I += (identity * q3Dot( local.position, local.position ) - q3OuterProduct( local.position, local.position )) * mass;

	md->center = local.position;
	md->inertia = I;
	md->mass = mass;
}

//--------------------------------------------------------------------------------------------------
const i32 kBoxIndices[ 36 ] = {
	1 - 1, 7 - 1, 5 - 1,
	1 - 1, 3 - 1, 7 - 1,
	1 - 1, 4 - 1, 3 - 1,
	1 - 1, 2 - 1, 4 - 1,
	3 - 1, 8 - 1, 7 - 1,
	3 - 1, 4 - 1, 8 - 1,
	5 - 1, 7 - 1, 8 - 1,
	5 - 1, 8 - 1, 6 - 1,
	1 - 1, 5 - 1, 6 - 1,
	1 - 1, 6 - 1, 2 - 1,
	2 - 1, 6 - 1, 8 - 1,
	2 - 1, 8 - 1, 4 - 1
};

//--------------------------------------------------------------------------------------------------
void q3Box::Render( const q3Transform& tx, bool awake, q3Render* render ) const
{
	q3Vec3 vertices[ 8 ] = {
		q3Vec3( -e.x, -e.y, -e.z ),
		q3Vec3( -e.x, -e.y,  e.z ),
		q3Vec3( -e.x,  e.y, -e.z ),
		q3Vec3( -e.x,  e.y,  e.z ),
		q3Vec3(  e.x, -e.y, -e.z ),
		q3Vec3(  e.x, -e.y,  e.z ),
		q3Vec3(  e.x,  e.y, -e.z ),
		q3Vec3(  e.x,  e.y,  e.z )
	};

	for ( i32 i = 0; i < 36; i += 3 )
	{
		q3Vec3 a = q3Mul( tx, vertices[ kBoxIndices[ i ] ] );
		q3Vec3 b = q3Mul( tx, vertices[ kBoxIndices[ i + 1 ] ] );
		q3Vec3 c = q3Mul( tx, vertices[ kBoxIndices[ i + 2 ] ] );

		q3Vec3 n = q3Normalize( q3Cross( b - a, c - a ) );

		//render->SetPenColor( 0.2f, 0.4f, 0.7f, 0.5f );
		//render->SetPenPosition( a.x, a.y, a.z );
		//render->Line( b.x, b.y, b.z );
		//render->Line( c.x, c.y, c.z );
		//render->Line( a.x, a.y, a.z );

		render->SetTriNormal( n.x, n.y, n.z );
		render->Triangle( a.x, a.y, a.z, b.x, b.y, b.z, c.x, c.y, c.z );
	}
}
