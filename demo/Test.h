//--------------------------------------------------------------------------------------------------
/**
@file	Test.h

@author	Randy Gaul
@date	10/26/2015

	Copyright (c) 2015 Randy Gaul http://www.randygaul.net

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


struct Test : public Demo
{
	virtual void Init( )
	{
		q3BodyDef bodyDef;
		q3Body* body = scene.CreateBody( bodyDef );
		q3BoxDef boxDef;
		boxDef.SetRestitution( 0 );
		q3Transform tx;
		q3Identity( tx );
		boxDef.Set( tx, q3Vec3( 50.0f, 1.0f, 50.0f ) );
		body->AddBox( boxDef );

		bodyDef.bodyType = eDynamicBody;
		bodyDef.position.Set( 0, 5.0f, 0 );
		body = scene.CreateBody( bodyDef );
		for ( int i = 0; i < 20; ++i )
		{
			tx.position.Set( q3RandomFloat( 1.0f, 10.0f ), q3RandomFloat( 1.0f, 10.0f ), q3RandomFloat( 1.0f, 10.0f ) );
			boxDef.Set( tx, q3Vec3( 1.0f, 1.0f, 1.0f ) );
			body->AddBox( boxDef );
		}
	}

	virtual void Shutdown( )
	{
		scene.RemoveAllBodies( );
	}
};
