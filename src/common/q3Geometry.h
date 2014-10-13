//--------------------------------------------------------------------------------------------------
/**
@file	q3Geometry.h

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

#ifndef Q3GEOMETRY_H
#define Q3GEOMETRY_H

#include "../math/q3Math.h"

//--------------------------------------------------------------------------------------------------
// q3AABB
//--------------------------------------------------------------------------------------------------
struct q3AABB
{
	q3Vec3 min;
	q3Vec3 max;

	bool Contains( const q3AABB& other ) const;
	bool Contains( const q3Vec3& point ) const;
	r32 SurfaceArea( ) const;
};

const q3AABB q3Combine( const q3AABB& a, const q3AABB& b );

//--------------------------------------------------------------------------------------------------
// q3HalfSpace
//--------------------------------------------------------------------------------------------------
struct q3HalfSpace
{
	q3HalfSpace( );
	q3HalfSpace( const q3Vec3& normal, r32 distance );

	void Set( const q3Vec3& a, const q3Vec3& b, const q3Vec3& c );
	void Set( const q3Vec3& n, const q3Vec3& p );
	const q3Vec3 Origin( ) const;
	r32 Distance( const q3Vec3& p ) const;
	const q3Vec3 Projected( const q3Vec3& p ) const;

	q3Vec3 normal;
	r32 distance;
};

//--------------------------------------------------------------------------------------------------
// q3RaycastData
//--------------------------------------------------------------------------------------------------
struct q3RaycastData
{
	q3Vec3 start;	// Beginning point of the ray
	q3Vec3 dir;		// Direction of the ray (normalized)
	r32 t;			// Time specifying ray endpoint

	r32 toi;		// Solved time of impact
	q3Vec3 normal;	// Surface normal at impact

	void Set( const q3Vec3& startPoint, const q3Vec3& direction, r32 endPointTime );

	// Uses toi, start and dir to compute the point at toi. Should
	// only be called after a raycast has been conducted with a
	// return value of true.
	const q3Vec3 GetImpactPoint( ) const;
};

#include "q3Geometry.inl"

#endif // Q3GEOMETRY_H
