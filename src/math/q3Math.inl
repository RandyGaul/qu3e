//--------------------------------------------------------------------------------------------------
// q3Math.inl
//
//	Copyright (c) 2014 Randy Gaul http://www.randygaul.net
//
//	This software is provided 'as-is', without any express or implied
//	warranty. In no event will the authors be held liable for any damages
//	arising from the use of this software.
//
//	Permission is granted to anyone to use this software for any purpose,
//	including commercial applications, and to alter it and redistribute it
//	freely, subject to the following restrictions:
//	  1. The origin of this software must not be misrepresented; you must not
//	     claim that you wrote the original software. If you use this software
//	     in a product, an acknowledgment in the product documentation would be
//	     appreciated but is not required.
//	  2. Altered source versions must be plainly marked as such, and must not
//	     be misrepresented as being the original software.
//	  3. This notice may not be removed or altered from any source distribution.
//--------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------
inline r32 q3Invert( r32 a )
{
	return a != 0.0f ? 1.0f / a : 0.0f;
}

//--------------------------------------------------------------------------------------------------
inline r32 q3Sign( r32 a )
{
	if ( a >= r32( 0.0 ) )
	{
		return r32( 1.0 );
	}

	else
	{
		return r32( -1.0 );
	}
}

//--------------------------------------------------------------------------------------------------
inline r32 q3Abs( r32 a )
{
	if ( a < r32( 0.0 ) )
		return -a;

	return a;
}

//--------------------------------------------------------------------------------------------------
inline i32 q3Min( i32 a, i32 b )
{
	if ( a < b )
		return a;

	return b;
}

//--------------------------------------------------------------------------------------------------
inline r32 q3Min( r32 a, r32 b )
{
	if ( a < b )
		return a;

	return b;
}

//--------------------------------------------------------------------------------------------------
inline r32 q3Max( r32 a, r32 b )
{
	if ( a > b )
		return a;

	return b;
}

//--------------------------------------------------------------------------------------------------
inline i32 q3Max( i32 a, i32 b )
{
	if ( a > b )
		return a;

	return b;
}

//--------------------------------------------------------------------------------------------------
inline u8 q3Max( u8 a, u8 b )
{
	if ( a > b )
		return a;

	return b;
}

//--------------------------------------------------------------------------------------------------
inline r32 q3Clamp01( r32 val )
{
	if ( val >= r32( 1.0 ) )
		return 1.0;

	if ( val <= r32( 0.0 ) )
		return 0.0;

	return val;
}

//--------------------------------------------------------------------------------------------------
inline r32 q3Clamp( r32 min, r32 max, r32 a )
{
	if ( a < min )
		return min;

	if ( a > max )
		return max;

	return a;
}

//--------------------------------------------------------------------------------------------------
inline r32 q3Lerp( r32 a, r32 b, r32 t )
{
	return a * (r32( 1.0 ) - t) + b * t;
}

//--------------------------------------------------------------------------------------------------
inline const q3Vec3 q3Lerp( const q3Vec3& a, const q3Vec3& b, r32 t )
{
	return a * (r32( 1.0 ) - t) + b * t;
}

//--------------------------------------------------------------------------------------------------
inline r32 q3RandomFloat( r32 l, r32 h )
{
	r32 a = r32( rand( ) );
	a /= r32( RAND_MAX );
	a = (h - l) * a + l;

	return a;
}

//--------------------------------------------------------------------------------------------------
inline i32 q3RandomInt( i32 low, i32 high )
{
	return (rand( ) % (high - low + 1) + low);
}
