//--------------------------------------------------------------------------------------------------
/**
@file	q3Vec3.cpp

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

#include <cassert>
#include "q3Vec3.h"

//--------------------------------------------------------------------------------------------------
// q3Vec3
//--------------------------------------------------------------------------------------------------
q3Vec3::q3Vec3( )
{
}

//--------------------------------------------------------------------------------------------------
q3Vec3::q3Vec3( r32 _x, r32 _y, r32 _z )
	: x( _x )
	, y( _y )
	, z( _z )
{
}

//--------------------------------------------------------------------------------------------------
void q3Vec3::Set( r32 _x, r32 _y, r32 _z )
{
	x = _x;
	y = _y;
	z = _z;
}

//--------------------------------------------------------------------------------------------------
void q3Vec3::SetAll( r32 a )
{
	x = a;
	y = a;
	z = a;
}

//--------------------------------------------------------------------------------------------------
q3Vec3& q3Vec3::operator+=( const q3Vec3& rhs )
{
	x += rhs.x;
	y += rhs.y;
	z += rhs.z;

	return *this;
}

//--------------------------------------------------------------------------------------------------
q3Vec3& q3Vec3::operator-=( const q3Vec3& rhs )
{
	x -= rhs.x;
	y -= rhs.y;
	z -= rhs.z;

	return *this;
}

//--------------------------------------------------------------------------------------------------
q3Vec3& q3Vec3::operator*=( r32 f )
{
	x *= f;
	y *= f;
	z *= f;

	return *this;
}

//--------------------------------------------------------------------------------------------------
q3Vec3& q3Vec3::operator/=( r32 f )
{
	x /= f;
	y /= f;
	z /= f;

	return *this;
}

//--------------------------------------------------------------------------------------------------
r32& q3Vec3::operator[]( u32 i )
{
	assert( i >= 0 && i < 3 );

	return v[ i ];
}

//--------------------------------------------------------------------------------------------------
r32 q3Vec3::operator[]( u32 i ) const
{
	assert( i >= 0 && i < 3 );

	return v[i];
}

//--------------------------------------------------------------------------------------------------
q3Vec3 q3Vec3::operator-( void ) const
{
	return q3Vec3( -x, -y, -z );
}

//--------------------------------------------------------------------------------------------------
const q3Vec3 q3Vec3::operator+( const q3Vec3& rhs ) const
{
	return q3Vec3( x + rhs.x, y + rhs.y, z + rhs.z );
}

//--------------------------------------------------------------------------------------------------
const q3Vec3 q3Vec3::operator-( const q3Vec3& rhs ) const
{
	return q3Vec3( x - rhs.x, y - rhs.y, z - rhs.z );
}

//--------------------------------------------------------------------------------------------------
const q3Vec3 q3Vec3::operator*( r32 f ) const
{
	return q3Vec3( x * f, y * f, z * f );
}

//--------------------------------------------------------------------------------------------------
const q3Vec3 q3Vec3::operator/( r32 f ) const
{
	return q3Vec3( x / f, y / f, z / f );
}

//--------------------------------------------------------------------------------------------------
const q3Vec3 operator*( r32 f, const q3Vec3& rhs )
{
	return q3Vec3( rhs.x * f, rhs.y * f, rhs.z * f );
}
