//--------------------------------------------------------------------------------------------------
/**
@file	q3Mat3.h

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

#ifndef Q3MAT3_H
#define Q3MAT3_H

#include <cstring>	// memset
#include "../common/q3Types.h"
#include "q3Vec3.h"

//--------------------------------------------------------------------------------------------------
// q3Mat3
//--------------------------------------------------------------------------------------------------
struct q3Mat3
{
	q3Vec3 ex;
	q3Vec3 ey;
	q3Vec3 ez;

	q3Mat3( );
	q3Mat3( r32 a, r32 b, r32 c, r32 d, r32 e, r32 f, r32 g, r32 h, r32 i );
	q3Mat3( const q3Vec3& _x, const q3Vec3& _y, const q3Vec3& _z );

	void Set( r32 a, r32 b, r32 c, r32 d, r32 e, r32 f, r32 g, r32 h, r32 i );
	void Set( const q3Vec3& axis, r32 angle );
	void SetRows( const q3Vec3& x, const q3Vec3& y, const q3Vec3& z );

	q3Mat3& operator=( const q3Mat3& rhs );
	q3Mat3& operator*=( const q3Mat3& rhs );
	q3Mat3& operator*=( r32 f );
	q3Mat3& operator+=( const q3Mat3& rhs );
	q3Mat3& operator-=( const q3Mat3& rhs );

	q3Vec3& operator[]( u32 index );
	const q3Vec3& operator[]( u32 index ) const;
	const q3Vec3 Column0( ) const;
	const q3Vec3 Column1( ) const;
	const q3Vec3 Column2( ) const;

	const q3Vec3 operator*( const q3Vec3& rhs ) const;
	const q3Mat3 operator*( const q3Mat3& rhs ) const;
	const q3Mat3 operator*( r32 f ) const;
	const q3Mat3 operator+( const q3Mat3& rhs ) const;
	const q3Mat3 operator-( const q3Mat3& rhs ) const;
};

#include "q3Mat3.inl"

#endif // Q3MAT3_H
