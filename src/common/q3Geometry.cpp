//--------------------------------------------------------------------------------------------------
/**
@file	q3Geometry.cpp

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

#include "q3Geometry.h"

//--------------------------------------------------------------------------------------------------
// q3HalfSpace
//--------------------------------------------------------------------------------------------------
q3HalfSpace::q3HalfSpace( )
{
}

//--------------------------------------------------------------------------------------------------
q3HalfSpace::q3HalfSpace( const q3Vec3& n, r32 d )
	: normal( n )
	, distance( d )
{
}

//--------------------------------------------------------------------------------------------------
void q3HalfSpace::Set( const q3Vec3& a, const q3Vec3& b, const q3Vec3& c )
{
	normal = q3Normalize( q3Cross( b - a, c - a ) );
	distance = q3Dot( normal, a );
}

//--------------------------------------------------------------------------------------------------
void q3HalfSpace::Set( const q3Vec3& n, const q3Vec3& p )
{
	normal = q3Normalize( n );
	distance = q3Dot( normal, p );
}

//--------------------------------------------------------------------------------------------------
const q3Vec3 q3HalfSpace::Origin( ) const
{
	return normal * distance;
}

//--------------------------------------------------------------------------------------------------
r32 q3HalfSpace::Distance( const q3Vec3& p ) const
{
	return q3Dot( normal, p ) - distance;
}

//--------------------------------------------------------------------------------------------------
const q3Vec3 q3HalfSpace::Projected( const q3Vec3& p ) const
{
	return p - normal * Distance( p );
}
