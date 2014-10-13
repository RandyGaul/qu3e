//--------------------------------------------------------------------------------------------------
/**
@file	q3Vec3.h

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

#ifndef Q3VEC3_H
#define Q3VEC3_H

#include "../common/q3Types.h"

r32 q3Abs( r32 a );
r32 q3Min( r32 a, r32 b );
r32 q3Max( r32 a, r32 b );

//--------------------------------------------------------------------------------------------------
// q3Vec3
//--------------------------------------------------------------------------------------------------
struct q3Vec3
{
	union
	{
		r32 v[ 3 ];

		struct
		{
			r32 x;
			r32 y;
			r32 z;
		};
	};

	q3Vec3( );
	q3Vec3( r32 _x, r32 _y, r32 _z );

	void Set( r32 _x, r32 _y, r32 _z );
	void SetAll( r32 a );
	q3Vec3& operator+=( const q3Vec3& rhs );
	q3Vec3& operator-=( const q3Vec3& rhs );
	q3Vec3& operator*=( r32 f );
	q3Vec3& operator/=( r32 f );

	r32& operator[]( u32 i );
	r32 operator[]( u32 i ) const;

	q3Vec3 operator-( void ) const;

	const q3Vec3 operator+( const q3Vec3& rhs ) const;
	const q3Vec3 operator-( const q3Vec3& rhs ) const;
	const q3Vec3 operator*( r32 f ) const;
	const q3Vec3 operator/( r32 f ) const;
};

#include "q3Vec3.inl"

#endif // Q3VEC3_H
