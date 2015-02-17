//--------------------------------------------------------------------------------------------------
/**
@file	q3Quaternion.cpp

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
#include "q3Quaternion.h"
#include "q3Mat3.h"

//--------------------------------------------------------------------------------------------------
// q3Quaternion
//--------------------------------------------------------------------------------------------------
q3Quaternion::q3Quaternion( )
{
}

//--------------------------------------------------------------------------------------------------
q3Quaternion::q3Quaternion( r32 a, r32 b, r32 c, r32 d )
	: x( a )
	, y( b )
	, z( c )
	, w( d )
{
}

//--------------------------------------------------------------------------------------------------
q3Quaternion::q3Quaternion( const q3Vec3& axis, r32 radians )
{
	Set( axis, radians );
}

//--------------------------------------------------------------------------------------------------
void q3Quaternion::Set( const q3Vec3& axis, r32 radians )
{
	r32 halfAngle = r32( 0.5 ) * radians;
	r32 s = std::sin( halfAngle );
	x = s * axis.x;
	y = s * axis.y;
	z = s * axis.z;
	w = std::cos( halfAngle );
}

//--------------------------------------------------------------------------------------------------
void q3Quaternion::ToAxisAngle( q3Vec3* axis, r32* angle ) const
{
	assert( w <= r32( 1.0 ) );

	*angle = r32( 2.0 ) * std::acos( w );

	r32 l = std::sqrt( r32( 1.0 ) - w * w );

	if ( l == r32( 0.0 ) )
	{
		axis->Set( r32( 0.0 ), r32( 0.0 ), r32( 0.0 ) );
	}

	else
	{
		l = r32( 1.0 ) / l;
		axis->Set( x * l, y * l, z * l );
	}
}

//--------------------------------------------------------------------------------------------------
void q3Quaternion::Integrate( const q3Vec3& dv, r32 dt )
{
	q3Quaternion q( dv.x * dt, dv.y * dt, dv.z * dt, r32( 0.0 ) );

	q *= *this;

	x += q.x * r32( 0.5 );
	y += q.y * r32( 0.5 );
	z += q.z * r32( 0.5 );
	w += q.w * r32( 0.5 );

	*this = q3Normalize( *this );
}

//--------------------------------------------------------------------------------------------------
const q3Quaternion q3Quaternion::operator*( const q3Quaternion& rhs ) const
{
	return q3Quaternion(
		w * rhs.x + x * rhs.w + y * rhs.z - z * rhs.y,
		w * rhs.y + y * rhs.w + z * rhs.x - x * rhs.z,
		w * rhs.z + z * rhs.w + x * rhs.y - y * rhs.x,
		w * rhs.w - x * rhs.x - y * rhs.y - z * rhs.z
		);
}

//--------------------------------------------------------------------------------------------------
q3Quaternion& q3Quaternion::operator*=( const q3Quaternion& rhs )
{
	q3Quaternion temp(
		w * rhs.x + x * rhs.w + y * rhs.z - z * rhs.y,
		w * rhs.y + y * rhs.w + z * rhs.x - x * rhs.z,
		w * rhs.z + z * rhs.w + x * rhs.y - y * rhs.x,
		w * rhs.w - x * rhs.x - y * rhs.y - z * rhs.z
		);

	*this = temp;
	return *this;
}

//--------------------------------------------------------------------------------------------------
const q3Mat3 q3Quaternion::ToMat3( void ) const
{
	r32 qx2 = x + x;
	r32 qy2 = y + y;
	r32 qz2 = z + z;
	r32 qxqx2 = x * qx2;
	r32 qxqy2 = x * qy2;
	r32 qxqz2 = x * qz2;
	r32 qxqw2 = w * qx2;
	r32 qyqy2 = y * qy2;
	r32 qyqz2 = y * qz2;
	r32 qyqw2 = w * qy2;
	r32 qzqz2 = z * qz2;
	r32 qzqw2 = w * qz2;

	return q3Mat3(
		q3Vec3( r32( 1.0 ) - qyqy2 - qzqz2, qxqy2 + qzqw2, qxqz2 - qyqw2 ),
		q3Vec3( qxqy2 - qzqw2, r32( 1.0 ) - qxqx2 - qzqz2, qyqz2 + qxqw2 ),
		q3Vec3( qxqz2 + qyqw2, qyqz2 - qxqw2, r32( 1.0 ) - qxqx2 - qyqy2 )
		);
}
