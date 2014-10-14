//--------------------------------------------------------------------------------------------------
/**
@file	main.cpp

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

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif
#include "../freeglut/freeglut.h"
#include "Clock.h"
#include "../src/q3.h"

#define ESC_KEY 27

class Renderer : public q3Render
{
public:
	void SetPenColor( f32 r, f32 g, f32 b, f32 a = 1.0f ) override
	{
		Q3_UNUSED( a );

		glColor3f( (float)r, (float)g, (float)b );
	}

	void SetPenPosition( f32 x, f32 y, f32 z ) override
	{
		x_ = x, y_ = y, z_ = z;
	}

	void SetScale( f32 sx, f32 sy, f32 sz ) override
	{
		glPointSize( (float)sx );
		sx_ = sx, sy_ = sy, sz_ = sz;
	}

	void Line( f32 x, f32 y, f32 z ) override
	{
		glBegin( GL_LINES );
		glVertex3f( (float)x_, (float)y_, (float)z_ );
		glVertex3f( (float)x, (float)y, (float)z );
		SetPenPosition( x, y, z );
		glEnd( );
	}

	void Triangle(
		f32 x1, f32 y1, f32 z1,
		f32 x2, f32 y2, f32 z2,
		f32 x3, f32 y3, f32 z3
		) override
	{
		glEnable( GL_LIGHTING );
		glBegin( GL_TRIANGLES );
		glColor4f( 0.2f, 0.4f, 0.7f, 0.5f );
		glVertex3f( (float)x1, (float)y1, (float)z1 );
		glVertex3f( (float)x2, (float)y2, (float)z2 );
		glVertex3f( (float)x3, (float)y3, (float)z3 );
		glEnd( );
		glDisable( GL_LIGHTING );
	}

	void SetTriNormal( f32 x, f32 y, f32 z ) override
	{
		glNormal3f( (float)x, (float)y, (float)z );
	}

	void Point( ) override
	{
		glBegin( GL_POINTS );
		glVertex3f( (float)x_, (float)y_, (float)z_ );
		glEnd( );
	};

private:
	f32 x_, y_, z_;
	f32 sx_, sy_, sz_;
};

const float dt = 1.0f / 60.0f;
q3Scene scene( dt );
Clock g_clock;
bool frameStepping = false;
bool canStep = false;
Renderer renderer;
i32 seed = 0;

void Demo( )
{
	scene.SetIterations( 20 );
	scene.SetAllowSleep( true );
	scene.RemoveAllBodies( );
	//seed = 23;
	//printf( "seed: %d\n", seed );
	srand( seed++ );

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
	for ( i32 i = 0; i < 10; ++i )
	{
		bodyDef.position.Set( 0.0f, 1.2f * (i + 1), -0.0f );
		//bodyDef.axis.Set( 0.0f, 1.0f, 0.0f );
		//bodyDef.angle = q3PI * q3RandomFloat( -1.0f, 1.0f );
		//bodyDef.angularVelocity.Set( 3.0f, 3.0f, 3.0f );
		//bodyDef.linearVelocity.Set( 2.0f, 0.0f, 0.0f );
		bodyDef.bodyType = eDynamicBody;
		body = scene.CreateBody( bodyDef );
		boxDef.Set( tx, q3Vec3( 1.0f, 1.0f, 1.0f ) );
		body->AddBox( boxDef );
	}
}

namespace Camera
{
	float position[ 3 ] = { 0.0f, 5.0f, 20.0f };
	float target[ 3 ] = { 0.0f, 0.0f, 0.0f };
};

namespace Light
{
	float ambient[ 4 ] = { 1.0f, 1.0f, 1.0f, 0.5f };
	float diffuse[ 4 ] = { 0.2f, 0.4f, 0.7f, 1.0f };
	float specular[ 4 ] = { 1.0f, 1.0f, 1.0f, 1.0f };
}

void Mouse( int button, int state, int x, int y )
{
	if ( state == GLUT_DOWN )
		switch(button)
		{
		case GLUT_LEFT_BUTTON:
			{
			}
			break;
		case GLUT_RIGHT_BUTTON:
			{
			}
			break;
		}
}

void Keyboard(unsigned char key, int x, int y)
{
	const float increment = 0.2f;

	switch(key)
	{
	case ESC_KEY:
		exit( 0 );
		break;
	case 'f':
		frameStepping = frameStepping ? false : true;
		break;
	case ' ':
		frameStepping = true;
		canStep = true;
		break;
	case 'w':
		Camera::position[ 2 ] -= increment;
		Camera::target[ 2 ] -= increment;
		break;
	case 's':
		Camera::position[ 2 ] += increment;
		Camera::target[ 2 ] += increment;
		break;
	case 'a':
		Camera::position[ 0 ] -= increment;
		Camera::target[ 0 ] -= increment;
		break;
	case 'd':
		Camera::position[ 0 ] += increment;
		Camera::target[ 0 ] += increment;
		break;
	case 'q':
		Camera::position[ 1 ] -= increment;
		Camera::target[ 1 ] -= increment;
		break;
	case 'e':
		Camera::position[ 1 ] += increment;
		Camera::target[ 1 ] += increment;
		break;
	case 'r':
		Demo( );
		break;
	}
}

void Reshape( int width, int height )
{
	if ( height <= 0 )
		height = 1;

	f32 aspectRatio = (f32)width / (f32)height;
	glViewport( 0, 0, width, height );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity( );
	gluPerspective( 45.0f, aspectRatio, 0.1f, 10000.0f );
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity( );
	gluLookAt(
		Camera::position[ 0 ], Camera::position[ 1 ], Camera::position[ 2 ],
		Camera::target[ 0 ], Camera::target[ 1 ], Camera::target[ 2 ],
		0.0f, 1.0f, 0.0f
		);
}

void PhysicsLoop( void )
{
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	i32 w = glutGet( GLUT_WINDOW_WIDTH );
	i32 h = glutGet( GLUT_WINDOW_HEIGHT );
	Reshape( w, h );

	static float accumulator = 0;

	accumulator += g_clock.Start( ) * 0.5f;

	accumulator = q3Clamp01( accumulator );
	while ( accumulator >= dt )
	{
		if ( !frameStepping )
			scene.Step( );

		else
		{
			if ( canStep )
			{
				scene.Step( );
				canStep = false;
			}
		}

		accumulator -= dt;
	}

	g_clock.Stop( );

	scene.Render( &renderer );

	glutSwapBuffers( );
}

int main( int argc, char** argv )
{
	// Starting width / height of the window
	const u32 kWindowWidth = 1000;
	const u32 kWindowHeight = 600;

	// Initialize GLUT
	glutInit( &argc, argv );

	// Get how big our screen is that we're displaying the window on
	int screenWidth = glutGet( GLUT_SCREEN_WIDTH );
	int screenHeight = glutGet( GLUT_SCREEN_HEIGHT );

	// Initialize the display mode to utilize double buffering, 4-channel framebuffer and depth buffer
	glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL );

	// Setup the window
	glutInitWindowSize( kWindowWidth, kWindowHeight );
	glutInitWindowPosition( (screenWidth - kWindowWidth) / 2, (screenHeight - kWindowHeight) / 2 );
	glutCreateWindow( "qu3e Physics by Randy Gaul" );

	glutDisplayFunc( PhysicsLoop );
	glutReshapeFunc( Reshape );
	glutKeyboardFunc( Keyboard );
	glutMouseFunc( Mouse );
	glutIdleFunc( PhysicsLoop );

	// Setup all the open-gl states we want to use (ones that don't change in the lifetime of the application)
	// Note: These can be changed anywhere, but generally we don't change the back buffer color
	glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
	glEnable( GL_CULL_FACE );
	glEnable( GL_DEPTH_TEST );
	glCullFace( GL_BACK );
	glFrontFace( GL_CCW );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	glEnable( GL_BLEND );

	// Show the window that we just initailized
	glutShowWindow( );

	// Used FFP to setup lights
	float floats[ 4 ];
	for ( i32 i = 0; i < 4; ++i )
		floats[ i ] = (float)Light::ambient[ i ];
	glLightfv( GL_LIGHT0, GL_AMBIENT, floats );
	for ( i32 i = 0; i < 4; ++i )
		floats[ i ] = (float)Light::diffuse[ i ];
	glLightfv( GL_LIGHT0, GL_DIFFUSE, floats );
	for ( i32 i = 0; i < 4; ++i )
		floats[ i ] = (float)Light::specular[ i ];
	glLightfv( GL_LIGHT0, GL_SPECULAR, floats );
	for ( i32 i = 0; i < 4; ++i )
		floats[ i ] = (float)Camera::position[ i ];
	glLightfv( GL_LIGHT0, GL_POSITION, floats );
	glEnable( GL_LIGHT0 );
	glColorMaterial( GL_FRONT, GL_AMBIENT_AND_DIFFUSE );

	Demo( );

	glutMainLoop( );

	return 0;
}
