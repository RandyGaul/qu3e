//--------------------------------------------------------------------------------------------------
/**
@file	DropBoxes.h

@author	Randy Gaul
@date	11/25/2014

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

class Raycast : public q3QueryCallback
{
public:
	q3RaycastData data;
	r32 tfinal;
	q3Vec3 nfinal;
	q3Body *impactBody;
	
	bool ReportShape( q3Box *shape )
	{
		if ( data.toi < tfinal )
		{
			tfinal = data.toi;
			nfinal = data.normal;
			impactBody = shape->body;
		}

		data.toi = tfinal;
		return true;
	}

	void Init( const q3Vec3& spot, const q3Vec3& dir )
	{
		data.start = spot;
		data.dir = q3Normalize( dir );
		data.t = r32( 10000.0 );
		tfinal = FLT_MAX;
		data.toi = data.t;
		impactBody = NULL;
	}
};

struct RayPush : public Demo
{
	Raycast rayCast;

	void Init( )
	{
		acc = 0;

		// Create the floor
		q3BodyDef bodyDef;
		q3Body* body = scene.CreateBody( bodyDef );

		q3BoxDef boxDef;
		boxDef.SetRestitution( 0 );
		q3Transform tx;
		q3Identity( tx );
		boxDef.Set( tx, q3Vec3( 50.0f, 1.0f, 50.0f ) );
		body->AddBox( boxDef );
	}

	void Update( )
	{
		acc += dt;

		if ( acc > 1.0f )
		{
			acc = 0;

			q3BodyDef bodyDef;
			bodyDef.position.Set( 0.0f, 3.0f, 0.0f );
			bodyDef.axis.Set( q3RandomFloat( -1.0f, 1.0f ), q3RandomFloat( -1.0f, 1.0f ), q3RandomFloat( -1.0f, 1.0f ) );
			bodyDef.angle = q3PI * q3RandomFloat( -1.0f, 1.0f );
			bodyDef.bodyType = eDynamicBody;
			bodyDef.angularVelocity.Set( q3RandomFloat( 1.0f, 3.0f ), q3RandomFloat( 1.0f, 3.0f ), q3RandomFloat( 1.0f, 3.0f ) );
			bodyDef.angularVelocity *= q3Sign( q3RandomFloat( -1.0f, 1.0f ) );
			bodyDef.linearVelocity.Set( q3RandomFloat( 1.0f, 3.0f ), q3RandomFloat( 1.0f, 3.0f ), q3RandomFloat( 1.0f, 3.0f ) );
			bodyDef.linearVelocity *= q3Sign( q3RandomFloat( -1.0f, 1.0f ) );
			q3Body* body = scene.CreateBody( bodyDef );

			q3Transform tx;
			q3Identity( tx );
			q3BoxDef boxDef;
			boxDef.Set( tx, q3Vec3( 1.0f, 1.0f, 1.0f ) );
			body->AddBox( boxDef );
		}

		rayCast.Init( q3Vec3( 3.0f, 5.0f, 3.0f ), q3Vec3( -1.0f, -1.0f, -1.0f ) );
		scene.RayCast( &rayCast, rayCast.data );

		if ( rayCast.impactBody )
		{
			rayCast.impactBody->SetToAwake( );
			rayCast.impactBody->ApplyForceAtWorldPoint( rayCast.data.dir * 20.0f, rayCast.data.GetImpactPoint( ) );
		}
	}

	void Shutdown( )
	{
		scene.RemoveAllBodies( );
	}

	void Render( q3Render *render )
	{
		render->SetScale( 1.0f, 1.0f, 1.0f );
		render->SetPenColor( 0.2f, 0.5f, 1.0f );
		render->SetPenPosition( rayCast.data.start.x, rayCast.data.start.y, rayCast.data.start.z );
		q3Vec3 impact = rayCast.data.GetImpactPoint( );
		render->Line( impact.x, impact.y, impact.z );

		render->SetPenPosition( impact.x, impact.y, impact.z );
		render->SetPenColor( 1.0f, 0.5f, 0.5f );
		render->SetScale( 10.0f, 10.0f, 10.0f );
		render->Point( );

		render->SetPenColor( 1.0f, 0.5f, 0.2f );
		render->SetScale( 1.0f, 1.0f, 1.0f );
		impact += rayCast.nfinal * 2.0f;
		render->Line( impact.x, impact.y, impact.z );
	}

	float acc;
};
