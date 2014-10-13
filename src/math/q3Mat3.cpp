//--------------------------------------------------------------------------------------------------
/**
@file	q3Mat3.cpp

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

#include "q3Mat3.h"

//--------------------------------------------------------------------------------------------------
// q3Mat3
//--------------------------------------------------------------------------------------------------
q3Mat3::q3Mat3( )
{
}

//--------------------------------------------------------------------------------------------------
q3Mat3::q3Mat3( r32 a, r32 b, r32 c, r32 d, r32 e, r32 f, r32 g, r32 h, r32 i )
	: m00( a ), m01( b ), m02( c )
	, m10( d ), m11( e ), m12( f )
	, m20( g ), m21( h ), m22( i )
{
}

//--------------------------------------------------------------------------------------------------
q3Mat3::q3Mat3( const q3Vec3& _x, const q3Vec3& _y, const q3Vec3& _z )
	: x( _x )
	, y( _y )
	, z( _z )
{
}

//--------------------------------------------------------------------------------------------------
void q3Mat3::Set( r32 a, r32 b, r32 c, r32 d, r32 e, r32 f, r32 g, r32 h, r32 i )
{
	m00 = a; m01 = b; m02 = c;
	m10 = d; m11 = e; m12 = f;
	m20 = g; m21 = h; m22 = i;
}

//--------------------------------------------------------------------------------------------------
void q3Mat3::Set( const q3Vec3& axis, r32 angle )
{
	r32 s = std::sin( angle );
	r32 c = std::cos( angle );
	r32 x = axis.x;
	r32 y = axis.y;
	r32 z = axis.z;
	r32 xy = x * y;
	r32 yz = y * z;
	r32 zx = z * x;
	r32 t = r32( 1.0 ) - c;

	Set(
		x * x * t + c, xy * t + z * s, zx * t - y * s,
		xy * t - z * s, y * y * t + c, yz * t + x * s,
		zx * t + y * s, yz * t - x * s, z * z * t + c
		);
}

//--------------------------------------------------------------------------------------------------
void q3Mat3::SetRows( const q3Vec3& x, const q3Vec3& y, const q3Vec3& z )
{
	m00 = x.x; m01 = x.y; m02 = x.z;
	m10 = y.x; m11 = y.y; m12 = y.z;
	m20 = z.x; m21 = z.y; m22 = z.z;
}

//--------------------------------------------------------------------------------------------------
q3Mat3& q3Mat3::operator=( const q3Mat3& rhs )
{
	m00 = rhs.m00; m01 = rhs.m01; m02 = rhs.m02;
	m10 = rhs.m10; m11 = rhs.m11; m12 = rhs.m12;
	m20 = rhs.m20; m21 = rhs.m21; m22 = rhs.m22;

	return *this;
}

//--------------------------------------------------------------------------------------------------
q3Mat3& q3Mat3::operator*=( const q3Mat3& rhs )
{
	*this = *this * rhs;

	return *this;
}

//--------------------------------------------------------------------------------------------------
q3Mat3& q3Mat3::operator*=( r32 f )
{
	m00 *= f; m10 *= f; m20 *= f;
	m01 *= f; m11 *= f; m21 *= f;
	m02 *= f; m12 *= f; m22 *= f;

	return *this;
}


//--------------------------------------------------------------------------------------------------
q3Mat3& q3Mat3::operator+=( const q3Mat3& rhs )
{
	m00 += rhs.m00; m10 += rhs.m10; m20 += rhs.m20;
	m01 += rhs.m01; m11 += rhs.m11; m21 += rhs.m21;
	m02 += rhs.m02; m12 += rhs.m12; m22 += rhs.m22;

	return *this;
}

//--------------------------------------------------------------------------------------------------
q3Mat3& q3Mat3::operator-=( const q3Mat3& rhs )
{
	m00 -= rhs.m00; m10 -= rhs.m10; m20 -= rhs.m20;
	m01 -= rhs.m01; m11 -= rhs.m11; m21 -= rhs.m21;
	m02 -= rhs.m02; m12 -= rhs.m12; m22 -= rhs.m22;

	return *this;
}

//--------------------------------------------------------------------------------------------------
q3Vec3& q3Mat3::operator[]( u32 index )
{
	return m[ index ];
}

//--------------------------------------------------------------------------------------------------
const q3Vec3& q3Mat3::operator[]( u32 index ) const
{
	return m[ index ];
}

//--------------------------------------------------------------------------------------------------
const q3Vec3 q3Mat3::Column0( ) const
{
	return q3Vec3( x.x, y.x, z.x );
}

//--------------------------------------------------------------------------------------------------
const q3Vec3 q3Mat3::Column1( ) const
{
	return q3Vec3( x.y, y.y, z.y );
}

//--------------------------------------------------------------------------------------------------
const q3Vec3 q3Mat3::Column2( ) const
{
	return q3Vec3( x.z, y.z, z.z );
}

//--------------------------------------------------------------------------------------------------
const q3Vec3 q3Mat3::operator*( const q3Vec3& rhs ) const
{
	return q3Vec3(
		x.x * rhs.x + y.x * rhs.y + z.x * rhs.z,
		x.y * rhs.x + y.y * rhs.y + z.y * rhs.z,
		x.z * rhs.x + y.z * rhs.y + z.z * rhs.z
		);
}

//--------------------------------------------------------------------------------------------------
const q3Mat3 q3Mat3::operator*( const q3Mat3& rhs ) const
{
	return q3Mat3(
		( *this * rhs.x ),
		( *this * rhs.y ),
		( *this * rhs.z )
		);
}

//--------------------------------------------------------------------------------------------------
const q3Mat3 q3Mat3::operator*( r32 f ) const
{
	return q3Mat3(
		m00 * f, m01 * f, m02 * f,
		m10 * f, m11 * f, m12 * f,
		m20 * f, m21 * f, m22 * f
		);
}

//--------------------------------------------------------------------------------------------------
const q3Mat3 q3Mat3::operator+( const q3Mat3& rhs ) const
{
	return q3Mat3(
		m00 + rhs.m00, m01 + rhs.m01, m02 + rhs.m02,
		m10 + rhs.m10, m11 + rhs.m11, m12 + rhs.m12,
		m20 + rhs.m20, m21 + rhs.m21, m22 + rhs.m22
		);
}

//--------------------------------------------------------------------------------------------------
const q3Mat3 q3Mat3::operator-( const q3Mat3& rhs ) const
{
	return q3Mat3(
		m00 - rhs.m00, m01 - rhs.m01, m02 - rhs.m02,
		m10 - rhs.m10, m11 - rhs.m11, m12 - rhs.m12,
		m20 - rhs.m20, m21 - rhs.m21, m22 - rhs.m22
		);
}
