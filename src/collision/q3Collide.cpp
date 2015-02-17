//--------------------------------------------------------------------------------------------------
/**
@file	q3Collide.cpp

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

#include "q3Collide.h"
#include "../dynamics/q3Body.h"
#include "../dynamics/q3Contact.h"

//--------------------------------------------------------------------------------------------------
// qBoxtoBox
//--------------------------------------------------------------------------------------------------
inline bool q3TrackFaceAxis( i32* axis, i32 n, r32 s, r32* sMax, const q3Vec3& normal, q3Vec3* axisNormal )
{
	if ( s > r32( 0.0 ) )
		return true;

	if ( s > *sMax )
	{
		*sMax = s;
		*axis = n;
		*axisNormal = normal;
	}

	return false;
}

//--------------------------------------------------------------------------------------------------
inline bool q3TrackEdgeAxis( i32* axis, i32 n, r32 s, r32* sMax, const q3Vec3& normal, q3Vec3* axisNormal )
{
	if ( s > r32( 0.0 ) )
		return true;

	r32 l = r32( 1.0 ) / q3Length( normal );
	s *= l;

	if ( s > *sMax )
	{
		*sMax = s;
		*axis = n;
		*axisNormal = normal * l;
	}

	return false;
}

//--------------------------------------------------------------------------------------------------
struct q3ClipVertex
{
	q3ClipVertex( )
	{
		f.key = ~0;
	}

	q3Vec3 v;
	q3FeaturePair f;
};

//--------------------------------------------------------------------------------------------------
void q3ComputeReferenceEdgesAndBasis( const q3Vec3& eR, const q3Transform& rtx, q3Vec3 n, i32 axis, u8* out, q3Mat3* basis, q3Vec3* e )
{
	n = q3MulT( rtx.rotation, n );

	if ( axis >= 3 )
		axis -= 3;

	switch ( axis )
	{
	case 0:
		if ( n.x > r32( 0.0 ) )
		{
			out[ 0 ] = 1;
			out[ 1 ] = 8;
			out[ 2 ] = 7;
			out[ 3 ] = 9;

			e->Set( eR.y, eR.z, eR.x );
			basis->SetRows( rtx.rotation.ey, rtx.rotation.ez, rtx.rotation.ex );
		}

		else
		{
			out[ 0 ] = 11;
			out[ 1 ] = 3;
			out[ 2 ] = 10;
			out[ 3 ] = 5;

			e->Set( eR.z, eR.y, eR.x );
			basis->SetRows( rtx.rotation.ez, rtx.rotation.ey, -rtx.rotation.ex );
		}
		break;

	case 1:
		if ( n.y > r32( 0.0 ) )
		{
			out[ 0 ] = 0;
			out[ 1 ] = 1;
			out[ 2 ] = 2;
			out[ 3 ] = 3;

			e->Set( eR.z, eR.x, eR.y );
			basis->SetRows( rtx.rotation.ez, rtx.rotation.ex, rtx.rotation.ey );
		}

		else
		{
			out[ 0 ] = 4;
			out[ 1 ] = 5;
			out[ 2 ] = 6;
			out[ 3 ] = 7;

			e->Set( eR.z, eR.x, eR.y );
			basis->SetRows( rtx.rotation.ez, -rtx.rotation.ex, -rtx.rotation.ey );
		}
		break;

	case 2:
		if ( n.z > r32( 0.0 ) )
		{
			out[ 0 ] = 11;
			out[ 1 ] = 4;
			out[ 2 ] = 8;
			out[ 3 ] = 0;

			e->Set( eR.y, eR.x, eR.z );
			basis->SetRows( -rtx.rotation.ey, rtx.rotation.ex, rtx.rotation.ez );
		}

		else
		{
			out[ 0 ] = 6;
			out[ 1 ] = 10;
			out[ 2 ] = 2;
			out[ 3 ] = 9;

			e->Set( eR.y, eR.x, eR.z );
			basis->SetRows( -rtx.rotation.ey, -rtx.rotation.ex, -rtx.rotation.ez );
		}
		break;
	}
}

//--------------------------------------------------------------------------------------------------
void q3ComputeIncidentFace( const q3Transform& itx, const q3Vec3& e, q3Vec3 n, q3ClipVertex* out )
{
	n = -q3MulT( itx.rotation, n );
	q3Vec3 absN = q3Abs( n );

	if ( absN.x > absN.y && absN.x > absN.z )
	{
		if ( n.x > r32( 0.0 ) )
		{
			out[ 0 ].v.Set(  e.x,  e.y, -e.z );
			out[ 1 ].v.Set(  e.x,  e.y,  e.z );
			out[ 2 ].v.Set(  e.x, -e.y,  e.z );
			out[ 3 ].v.Set(  e.x, -e.y, -e.z );

			out[ 0 ].f.inI = 9;
			out[ 0 ].f.outI = 1;
			out[ 1 ].f.inI = 1;
			out[ 1 ].f.outI = 8;
			out[ 2 ].f.inI = 8;
			out[ 2 ].f.outI = 7;
			out[ 3 ].f.inI = 7;
			out[ 3 ].f.outI = 9;
		}

		else
		{
			out[ 0 ].v.Set( -e.x, -e.y,  e.z );
			out[ 1 ].v.Set( -e.x,  e.y,  e.z );
			out[ 2 ].v.Set( -e.x,  e.y, -e.z );
			out[ 3 ].v.Set( -e.x, -e.y, -e.z );

			out[ 0 ].f.inI = 5;
			out[ 0 ].f.outI = 11;
			out[ 1 ].f.inI = 11;
			out[ 1 ].f.outI = 3;
			out[ 2 ].f.inI = 3;
			out[ 2 ].f.outI = 10;
			out[ 3 ].f.inI = 10;
			out[ 3 ].f.outI = 5;
		}
	}

	else if ( absN.y > absN.x && absN.y > absN.z )
	{
		if ( n.y > r32( 0.0 ) )
		{
			out[ 0 ].v.Set( -e.x,  e.y,  e.z );
			out[ 1 ].v.Set(  e.x,  e.y,  e.z );
			out[ 2 ].v.Set(  e.x,  e.y, -e.z );
			out[ 3 ].v.Set( -e.x,  e.y, -e.z );

			out[ 0 ].f.inI = 3;
			out[ 0 ].f.outI = 0;
			out[ 1 ].f.inI = 0;
			out[ 1 ].f.outI = 1;
			out[ 2 ].f.inI = 1;
			out[ 2 ].f.outI = 2;
			out[ 3 ].f.inI = 2;
			out[ 3 ].f.outI = 3;
		}

		else
		{
			out[ 0 ].v.Set(  e.x, -e.y,  e.z );
			out[ 1 ].v.Set( -e.x, -e.y,  e.z );
			out[ 2 ].v.Set( -e.x, -e.y, -e.z );
			out[ 3 ].v.Set(  e.x, -e.y, -e.z );

			out[ 0 ].f.inI = 7;
			out[ 0 ].f.outI = 4;
			out[ 1 ].f.inI = 4;
			out[ 1 ].f.outI = 5;
			out[ 2 ].f.inI = 5;
			out[ 2 ].f.outI = 6;
			out[ 3 ].f.inI = 5;
			out[ 3 ].f.outI = 6;
		}
	}

	else
	{
		if ( n.z > r32( 0.0 ) )
		{
			out[ 0 ].v.Set( -e.x,  e.y,  e.z );
			out[ 1 ].v.Set( -e.x, -e.y,  e.z );
			out[ 2 ].v.Set(  e.x, -e.y,  e.z );
			out[ 3 ].v.Set(  e.x,  e.y,  e.z );

			out[ 0 ].f.inI = 0;
			out[ 0 ].f.outI = 11;
			out[ 1 ].f.inI = 11;
			out[ 1 ].f.outI = 4;
			out[ 2 ].f.inI = 4;
			out[ 2 ].f.outI = 8;
			out[ 3 ].f.inI = 8;
			out[ 3 ].f.outI = 0;
		}

		else
		{
			out[ 0 ].v.Set(  e.x, -e.y, -e.z );
			out[ 1 ].v.Set( -e.x, -e.y, -e.z );
			out[ 2 ].v.Set( -e.x,  e.y, -e.z );
			out[ 3 ].v.Set(  e.x,  e.y, -e.z );

			out[ 0 ].f.inI = 9;
			out[ 0 ].f.outI = 6;
			out[ 1 ].f.inI = 6;
			out[ 1 ].f.outI = 10;
			out[ 2 ].f.inI = 10;
			out[ 2 ].f.outI = 2;
			out[ 3 ].f.inI = 2;
			out[ 3 ].f.outI = 9;
		}
	}

	for ( i32 i = 0; i < 4; ++i )
		out[ i ].v = q3Mul( itx, out[ i ].v );
}

//--------------------------------------------------------------------------------------------------
#define InFront( a ) \
	((a) < r32( 0.0 ))

#define Behind( a ) \
	((a) >= r32( 0.0 ))

#define On( a ) \
	((a) < r32( 0.005 ) && (a) > -r32( 0.005 ))

i32 q3Orthographic( r32 sign, r32 e, i32 axis, i32 clipEdge, q3ClipVertex* in, i32 inCount, q3ClipVertex* out )
{
	i32 outCount = 0;
	q3ClipVertex a = in[ inCount - 1 ];

	for ( i32 i = 0; i < inCount; ++i )
	{
		q3ClipVertex b = in[ i ];

		r32 da = sign * a.v[ axis ] - e;
		r32 db = sign * b.v[ axis ] - e;

		q3ClipVertex cv;

		// B
		if ( ((InFront( da ) && InFront( db )) || On( da ) || On( db )) )
		{
			assert( outCount < 8 );
			out[ outCount++ ] = b;
		}

		// I
		else if ( InFront( da ) && Behind( db ) )
		{
			cv.f = b.f;
			cv.v = a.v + (b.v - a.v) * (da / (da - db));
			cv.f.outR = clipEdge;
			cv.f.outI = 0;
			assert( outCount < 8 );
			out[ outCount++ ] = cv;
		}

		// I, B
		else if ( Behind( da ) && InFront( db ) )
		{
			cv.f = a.f;
			cv.v = a.v + (b.v - a.v) * (da / (da - db));
			cv.f.inR = clipEdge;
			cv.f.inI = 0;
			assert( outCount < 8 );
			out[ outCount++ ] = cv;

			assert( outCount < 8 );
			out[ outCount++ ] = b;
		}

		a = b;
	}

	return outCount;
}

//--------------------------------------------------------------------------------------------------
// Resources (also see q3BoxtoBox's resources):
// http://www.randygaul.net/2013/10/27/sutherland-hodgman-clipping/
i32 q3Clip( const q3Vec3& rPos, const q3Vec3& e, u8* clipEdges, const q3Mat3& basis, q3ClipVertex* incident, q3ClipVertex* outVerts, r32* outDepths )
{
	i32 inCount = 4;
	i32 outCount;
	q3ClipVertex in[ 8 ];
	q3ClipVertex out[ 8 ];

	for ( i32 i = 0; i < 4; ++i )
		in[ i ].v = q3MulT( basis, incident[ i ].v - rPos );

	outCount = q3Orthographic( r32( 1.0 ), e.x, 0, clipEdges[ 0 ], in, inCount, out );

	if ( !outCount )
		return 0;

	inCount = q3Orthographic( r32( 1.0 ), e.y, 1, clipEdges[ 1 ], out, outCount, in );

	if ( !inCount )
		return 0;

	outCount = q3Orthographic( r32( -1.0 ), e.x, 0, clipEdges[ 2 ], in, inCount, out );

	if ( !outCount )
		return 0;

	inCount = q3Orthographic( r32( -1.0 ), e.y, 1, clipEdges[ 3 ], out, outCount, in );

	// Keep incident vertices behind the reference face
	outCount = 0;
	for ( i32 i = 0; i < inCount; ++i )
	{
		r32 d = in[ i ].v.z - e.z;

		if ( d <= r32( 0.0 ) )
		{
			outVerts[ outCount ].v = q3Mul( basis, in[ i ].v ) + rPos;
			outVerts[ outCount ].f = in[ i ].f;
			outDepths[ outCount++ ] = d;
		}
	}

	assert( outCount <= 8 );

	return outCount;
}

//--------------------------------------------------------------------------------------------------
inline void q3EdgesContact( q3Vec3 *CA, q3Vec3 *CB, const q3Vec3& PA, const q3Vec3& QA, const q3Vec3& PB, const q3Vec3& QB )
{
	q3Vec3 DA = QA - PA;
	q3Vec3 DB = QB - PB;
	q3Vec3 r = PA - PB;
	r32 a = q3Dot( DA, DA );
	r32 e = q3Dot( DB, DB );
	r32 f = q3Dot( DB, r );
	r32 c = q3Dot( DA, r );

	r32 b = q3Dot( DA, DB );
	r32 denom = a * e - b * b;

	r32 TA = (b * f - c * e) / denom;
	r32 TB = (b * TA + f) / e;

	*CA = PA + DA * TA;
	*CB = PB + DB * TB;
}

//--------------------------------------------------------------------------------------------------
void q3SupportEdge( const q3Transform& tx, const q3Vec3& e, q3Vec3 n, q3Vec3* aOut, q3Vec3* bOut )
{
	n = q3MulT( tx.rotation, n );
	q3Vec3 absN = q3Abs( n );
	q3Vec3 a, b;

	// x > y
	if ( absN.x > absN.y )
	{
		// x > y > z
		if ( absN.y > absN.z )
		{
			a.Set( e.x, e.y, e.z );
			b.Set( e.x, e.y, -e.z );
		}

		// x > z > y || z > x > y
		else
		{
			a.Set( e.x, e.y, e.z );
			b.Set( e.x, -e.y, e.z );
		}
	}

	// y > x
	else
	{
		// y > x > z
		if ( absN.x > absN.z )
		{
			a.Set( e.x, e.y, e.z );
			b.Set( e.x, e.y, -e.z );
		}

		// z > y > x || y > z > x
		else
		{
			a.Set( e.x, e.y, e.z );
			b.Set( -e.x, e.y, e.z );
		}
	}

	r32 signx = q3Sign( n.x );
	r32 signy = q3Sign( n.y );
	r32 signz = q3Sign( n.z );

	a.x *= signx;
	a.y *= signy;
	a.z *= signz;
	b.x *= signx;
	b.y *= signy;
	b.z *= signz;

	*aOut = q3Mul( tx, a );
	*bOut = q3Mul( tx, b );
}

//--------------------------------------------------------------------------------------------------
// Resources:
// http://www.randygaul.net/2014/05/22/deriving-obb-to-obb-intersection-sat/
// https://box2d.googlecode.com/files/GDC2007_ErinCatto.zip
// https://box2d.googlecode.com/files/Box2D_Lite.zip
void q3BoxtoBox( q3Manifold* m, q3Box* a, q3Box* b )
{
	q3Transform atx = a->body->GetTransform( );
	q3Transform btx = b->body->GetTransform( );
	q3Vec3 eA = a->e;
	q3Vec3 eB = b->e;

	// B's frame in A's space
	q3Mat3 C = q3Transpose( atx.rotation ) * btx.rotation;

	q3Mat3 absC;
	bool parallel = false;
	const r32 kCosTol = r32( 1.0e-6 );
	for ( i32 i = 0; i < 3; ++i )
	{
		for ( i32 j = 0; j < 3; ++j )
		{
			r32 val = q3Abs( C[ i ][ j ] );
			absC[ i ][ j ] = val;

			if ( val + kCosTol >= r32( 1.0 ) )
				parallel = true;
		}
	}

	// Vector from center A to center B in A's space
	q3Vec3 t = q3MulT( atx.rotation, btx.position - atx.position );

	// Query states
	r32 s;
	r32 aMax = -Q3_R32_MAX;
	r32 bMax = -Q3_R32_MAX;
	r32 eMax = -Q3_R32_MAX;
	i32 aAxis = ~0;
	i32 bAxis = ~0;
	i32 eAxis = ~0;
	q3Vec3 nA;
	q3Vec3 nB;
	q3Vec3 nE;

	// Face axis checks

	// a's x axis
	s = q3Abs( t.x ) - (eA.x + q3Dot( absC.Column0( ), eB ));
	if ( q3TrackFaceAxis( &aAxis, 0, s, &aMax, atx.rotation.ex, &nA ) )
		return;

	// a's y axis
	s = q3Abs( t.y ) - (eA.y + q3Dot( absC.Column1( ), eB ));
	if ( q3TrackFaceAxis( &aAxis, 1, s, &aMax, atx.rotation.ey, &nA ) )
		return;

	// a's z axis
	s = q3Abs( t.z ) - (eA.z + q3Dot( absC.Column2( ), eB ));
	if ( q3TrackFaceAxis( &aAxis, 2, s, &aMax, atx.rotation.ez, &nA ) )
		return;

	// b's x axis
	s = q3Abs( q3Dot( t, C.ex ) ) - (eB.x + q3Dot( absC.ex, eA ));
	if ( q3TrackFaceAxis( &bAxis, 3, s, &bMax, btx.rotation.ex, &nB ) )
		return;

	// b's y axis
	s = q3Abs( q3Dot( t, C.ey ) ) - (eB.y + q3Dot( absC.ey, eA ));
	if ( q3TrackFaceAxis( &bAxis, 4, s, &bMax, btx.rotation.ey, &nB ) )
		return;

	// b's z axis
	s = q3Abs( q3Dot( t, C.ez ) ) - (eB.z + q3Dot( absC.ez, eA ));
	if ( q3TrackFaceAxis( &bAxis, 5, s, &bMax, btx.rotation.ez, &nB ) )
		return;

	if ( !parallel )
	{
		// Edge axis checks
		r32 rA;
		r32 rB;

		// Cross( a.x, b.x )
		rA = eA.y * absC[ 0 ][ 2 ] + eA.z * absC[ 0 ][ 1 ];
		rB = eB.y * absC[ 2 ][ 0 ] + eB.z * absC[ 1 ][ 0 ];
		s = q3Abs( t.z * C[ 0 ][ 1 ] - t.y * C[ 0 ][ 2 ] ) - (rA + rB);
		if ( q3TrackEdgeAxis( &eAxis, 6, s, &eMax, q3Vec3( r32( 0.0 ), -C[ 0 ][ 2 ], C[ 0 ][ 1 ] ), &nE ) )
			return;

		// Cross( a.x, b.y )
		rA = eA.y * absC[ 1 ][ 2 ] + eA.z * absC[ 1 ][ 1 ];
		rB = eB.x * absC[ 2 ][ 0 ] + eB.z * absC[ 0 ][ 0 ];
		s = q3Abs( t.z * C[ 1 ][ 1 ] - t.y * C[ 1 ][ 2 ] ) - (rA + rB);
		if ( q3TrackEdgeAxis( &eAxis, 7, s, &eMax, q3Vec3( r32( 0.0 ), -C[ 1 ][ 2 ], C[ 1 ][ 1 ] ), &nE ) )
			return;

		// Cross( a.x, b.z )
		rA = eA.y * absC[ 2 ][ 2 ] + eA.z * absC[ 2 ][ 1 ];
		rB = eB.x * absC[ 1 ][ 0 ] + eB.y * absC[ 0 ][ 0 ];
		s = q3Abs( t.z * C[ 2 ][ 1 ] - t.y * C[ 2 ][ 2 ] ) - (rA + rB);
		if ( q3TrackEdgeAxis( &eAxis, 8, s, &eMax, q3Vec3( r32( 0.0 ), -C[ 2 ][ 2 ], C[ 2 ][ 1 ] ), &nE ) )
			return;

		// Cross( a.y, b.x )
		rA = eA.x * absC[ 0 ][ 2 ] + eA.z * absC[ 0 ][ 0 ];
		rB = eB.y * absC[ 2 ][ 1 ] + eB.z * absC[ 1 ][ 1 ];
		s = q3Abs( t.x * C[ 0 ][ 2 ] - t.z * C[ 0 ][ 0 ] ) - (rA + rB);
		if ( q3TrackEdgeAxis( &eAxis, 9, s, &eMax, q3Vec3( C[ 0 ][ 2 ], r32( 0.0 ), -C[ 0 ][ 0 ] ), &nE ) )
			return;

		// Cross( a.y, b.y )
		rA = eA.x * absC[ 1 ][ 2 ] + eA.z * absC[ 1 ][ 0 ];
		rB = eB.x * absC[ 2 ][ 1 ] + eB.z * absC[ 0 ][ 1 ];
		s = q3Abs( t.x * C[ 1 ][ 2 ] - t.z * C[ 1 ][ 0 ] ) - (rA + rB);
		if ( q3TrackEdgeAxis( &eAxis, 10, s, &eMax, q3Vec3( C[ 1 ][ 2 ], r32( 0.0 ), -C[ 1 ][ 0 ] ), &nE ) )
			return;

		// Cross( a.y, b.z )
		rA = eA.x * absC[ 2 ][ 2 ] + eA.z * absC[ 2 ][ 0 ];
		rB = eB.x * absC[ 1 ][ 1 ] + eB.y * absC[ 0 ][ 1 ];
		s = q3Abs( t.x * C[ 2 ][ 2 ] - t.z * C[ 2 ][ 0 ] ) - (rA + rB);
		if ( q3TrackEdgeAxis( &eAxis, 11, s, &eMax, q3Vec3( C[ 2 ][ 2 ], r32( 0.0 ), -C[ 2 ][ 0 ] ), &nE ) )
			return;

		// Cross( a.z, b.x )
		rA = eA.x * absC[ 0 ][ 1 ] + eA.y * absC[ 0 ][ 0 ];
		rB = eB.y * absC[ 2 ][ 2 ] + eB.z * absC[ 1 ][ 2 ];
		s = q3Abs( t.y * C[ 0 ][ 0 ] - t.x * C[ 0 ][ 1 ] ) - (rA + rB);
		if ( q3TrackEdgeAxis( &eAxis, 12, s, &eMax, q3Vec3( -C[ 0 ][ 1 ], C[ 0 ][ 0 ], r32( 0.0 ) ), &nE ) )
			return;

		// Cross( a.z, b.y )
		rA = eA.x * absC[ 1 ][ 1 ] + eA.y * absC[ 1 ][ 0 ];
		rB = eB.x * absC[ 2 ][ 2 ] + eB.z * absC[ 0 ][ 2 ];
		s = q3Abs( t.y * C[ 1 ][ 0 ] - t.x * C[ 1 ][ 1 ] ) - (rA + rB);
		if ( q3TrackEdgeAxis( &eAxis, 13, s, &eMax, q3Vec3( -C[ 1 ][ 1 ], C[ 1 ][ 0 ], r32( 0.0 ) ), &nE ) )
			return;

		// Cross( a.z, b.z )
		rA = eA.x * absC[ 2 ][ 1 ] + eA.y * absC[ 2 ][ 0 ];
		rB = eB.x * absC[ 1 ][ 2 ] + eB.y * absC[ 0 ][ 2 ];
		s = q3Abs( t.y * C[ 2 ][ 0 ] - t.x * C[ 2 ][ 1 ] ) - (rA + rB);
		if ( q3TrackEdgeAxis( &eAxis, 14, s, &eMax, q3Vec3( -C[ 2 ][ 1 ], C[ 2 ][ 0 ], r32( 0.0 ) ), &nE ) )
			return;
	}

	// Artificial axis bias to improve frame coherence
	const r32 kRelTol = r32( 0.95 );
	const r32 kAbsTol = r32( 0.01 );
	i32 axis;
	r32 sMax;
	q3Vec3 n;
	r32 faceMax = q3Max( aMax, bMax );
	if ( kRelTol * eMax > faceMax + kAbsTol )
	{
		axis = eAxis;
		sMax = eMax;
		n = nE;
	}

	else
	{
		if ( kRelTol * bMax > aMax + kAbsTol )
		{
			axis = bAxis;
			sMax = bMax;
			n = nB;
		}

		else
		{
			axis = aAxis;
			sMax = aMax;
			n = nA;
		}
	}

	if ( q3Dot( n, btx.position - atx.position ) < r32( 0.0 ) )
		n = -n;

	assert( axis != ~0 );

	if ( axis < 6 )
	{
		q3Transform rtx;
		q3Transform itx;
		q3Vec3 eR;
		q3Vec3 eI;
		bool flip;

		if ( axis < 3 )
		{
			rtx = atx;
			itx = btx;
			eR = eA;
			eI = eB;
			flip = false;
		}

		else
		{
			rtx = btx;
			itx = atx;
			eR = eB;
			eI = eA;
			flip = true;
			n = -n;
		}

		// Compute reference and incident edge information necessary for clipping
		q3ClipVertex incident[ 4 ];
		q3ComputeIncidentFace( itx, eI, n, incident );
		u8 clipEdges[ 4 ];
		q3Mat3 basis;
		q3Vec3 e;
		q3ComputeReferenceEdgesAndBasis( eR, rtx, n, axis, clipEdges, &basis, &e );

		// Clip the incident face against the reference face side planes
		q3ClipVertex out[ 8 ];
		r32 depths[ 8 ];
		i32 outNum;
		outNum = q3Clip( rtx.position, e, clipEdges, basis, incident, out, depths );

		if ( outNum )
		{
			m->contactCount = outNum;
			m->normal = flip ? -n : n;

			for ( i32 i = 0; i < outNum; ++i )
			{
				q3Contact* c = m->contacts + i;

				q3FeaturePair pair = out[ i ].f;

				if ( flip )
				{
					std::swap( pair.inI, pair.inR );
					std::swap( pair.outI, pair.outR );
				}

				c->fp = out[ i ].f;
				c->position = out[ i ].v;
				c->penetration = depths[ i ];
			}
		}
	}

	else
	{
		n = atx.rotation * n;

		if ( q3Dot( n, btx.position - atx.position ) < r32( 0.0 ) )
			n = -n;

		q3Vec3 PA, QA;
		q3Vec3 PB, QB;
		q3SupportEdge( atx, eA, n, &PA, &QA );
		q3SupportEdge( btx, eB, -n, &PB, &QB );

		q3Vec3 CA, CB;
		q3EdgesContact( &CA, &CB, PA, QA, PB, QB );

		m->normal = n;
		m->contactCount = 1;

		q3Contact* c = m->contacts;
		q3FeaturePair pair;
		pair.key = axis;
		c->fp = pair;
		c->penetration = sMax;
		c->position = (CA + CB) * r32( 0.5 );
	}
}
