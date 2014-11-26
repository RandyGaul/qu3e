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

struct DropBoxes : public Demo
{
	virtual void Init( )
	{
		acc = 0;

		// Create the floor
		q3BodyDef bodyDef;
		//bodyDef.axis.Set( q3RandomFloat( -1.0f, 1.0f ), q3RandomFloat( -1.0f, 1.0f ), q3RandomFloat( -1.0f, 1.0f ) );
		//bodyDef.angle = q3PI * q3RandomFloat( -1.0f, 1.0f );
		q3Body* body = scene.CreateBody( bodyDef );

		q3BoxDef boxDef;
		boxDef.SetRestitution( 0 );
		q3Transform tx;
		q3Identity( tx );
		boxDef.Set( tx, q3Vec3( 50.0f, 1.0f, 50.0f ) );
		body->AddBox( boxDef );

		// Create boxes
		//for ( i32 i = 0; i < 10; ++i )
		//{
		//	bodyDef.position.Set( 0.0f, 1.2f * (i + 1), -0.0f );
		//	//bodyDef.axis.Set( 0.0f, 1.0f, 0.0f );
		//	//bodyDef.angle = q3PI * q3RandomFloat( -1.0f, 1.0f );
		//	//bodyDef.angularVelocity.Set( 3.0f, 3.0f, 3.0f );
		//	//bodyDef.linearVelocity.Set( 2.0f, 0.0f, 0.0f );
		//	bodyDef.bodyType = eDynamicBody;
		//	body = scene.CreateBody( bodyDef );
		//	boxDef.Set( tx, q3Vec3( 1.0f, 1.0f, 1.0f ) );
		//	body->AddBox( boxDef );
		//}
	}

	virtual void Update( )
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
	}

	virtual void Shutdown( )
	{
		scene.RemoveAllBodies( );
	}

	float acc;
};
