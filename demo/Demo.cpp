//--------------------------------------------------------------------------------------------------
/**
@file	Demo.cpp

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

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif
#include "../freeglut/freeglut.h"
#include "Clock.h"
#include "Demo.h"
#include "../imgui/imgui.h"
#include "stb_image.h"

#include "DropBoxes.h"
#include "RayPush.h"
#include "BoxStack.h"

float dt = 1.0f / 60.0f;
q3Scene scene( dt );
Clock g_clock;
bool paused = false;
bool singleStep = false;
bool enableSleep = true;
bool enableFriction = true;
int velocityIterations = 10;
i32 mouseX;
i32 mouseY;
bool mouseLeftDown;
bool mouseRightDown;
static GLuint fontTex;
int windowWidth;
int windowHeight;
i32 demoCount;
i32 currentDemo;
char sceneFileName[ 256 ];
i32 lastDemo;
Demo* demos[ Q3_DEMO_MAX_COUNT ];

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

Renderer renderer;

// This is the main rendering function that you have to implement and provide to ImGui (via setting up 'RenderDrawListsFn' in the ImGuiIO structure)
// If text or lines are blurry when integrating ImGui in your engine:
// - in your Render function, try translating your projection matrix by (0.5f,0.5f) or (0.375f,0.375f)
// - try adjusting ImGui::GetIO().PixelCenterOffset to 0.5f or 0.375f
static void ImImpl_RenderDrawLists(ImDrawList** const cmd_lists, int cmd_lists_count)
{
    if (cmd_lists_count == 0)
        return;

    // We are using the OpenGL fixed pipeline to make the example code simpler to read!
    // A probable faster way to render would be to collate all vertices from all cmd_lists into a single vertex buffer.
    // Setup render state: alpha-blending enabled, no face culling, no depth testing, scissor enabled, vertex/texcoord/color pointers.
    glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_TRANSFORM_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_SCISSOR_TEST);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    // Setup texture
    glBindTexture(GL_TEXTURE_2D, fontTex);
    glEnable(GL_TEXTURE_2D);

    // Setup orthographic projection matrix
    const float width = ImGui::GetIO().DisplaySize.x;
    const float height = ImGui::GetIO().DisplaySize.y;
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0.0f, width, height, 0.0f, -1.0f, +1.0f);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    // Render command lists
    for (int n = 0; n < cmd_lists_count; n++)
    {
        const ImDrawList* cmd_list = cmd_lists[n];
        const unsigned char* vtx_buffer = (const unsigned char*)&cmd_list->vtx_buffer.front();
        glVertexPointer(2, GL_FLOAT, sizeof(ImDrawVert), (void*)(vtx_buffer + offsetof(ImDrawVert, pos)));
        glTexCoordPointer(2, GL_FLOAT, sizeof(ImDrawVert), (void*)(vtx_buffer + offsetof(ImDrawVert, uv)));
        glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(ImDrawVert), (void*)(vtx_buffer + offsetof(ImDrawVert, col)));

        int vtx_offset = 0;
		for (size_t cmd_i = 0; cmd_i < cmd_list->commands.size(); cmd_i++)
        {
			const ImDrawCmd* pcmd = &cmd_list->commands[cmd_i];
            glScissor((int)pcmd->clip_rect.x, (int)(height - pcmd->clip_rect.w), (int)(pcmd->clip_rect.z - pcmd->clip_rect.x), (int)(pcmd->clip_rect.w - pcmd->clip_rect.y));
            glDrawArrays(GL_TRIANGLES, vtx_offset, pcmd->vtx_count);
            vtx_offset += pcmd->vtx_count;
        }
    }
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);

    // Restore modified state
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glPopAttrib();
}

void Mouse( int button, int state, int x, int y )
{
	mouseX = x;
	mouseY = y;
	ImGuiIO& io = ImGui::GetIO();
	io.MousePos = ImVec2( (float)mouseX, (float)mouseY );

	if ( state == GLUT_DOWN )
	{
		switch(button)
		{
		case GLUT_LEFT_BUTTON:
			{
				mouseLeftDown = true;
				demos[ currentDemo ]->LeftClick( x, y );
			}
			break;
		case GLUT_RIGHT_BUTTON:
			{
				mouseRightDown = true;
			}
			break;
		}
	}

	else if ( state == GLUT_UP )
	{
		switch(button)
		{
		case GLUT_LEFT_BUTTON:
			{
				mouseLeftDown = false;
			}
			break;
		case GLUT_RIGHT_BUTTON:
			{
				mouseRightDown = false;
			}
			break;
		}
	}
}

void MouseMotion( int dx, int dy )
{
	mouseX = dx;
	mouseY = dy;
	ImGuiIO& io = ImGui::GetIO();
	io.MousePos = ImVec2( (float)mouseX, (float)mouseY );
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

void Keyboard(unsigned char key, int x, int y)
{
	const float increment = 0.2f;

	switch(key)
	{
	case 27:
		exit( 0 );
		break;
	case 'p':
		paused = paused ? true : false;
		break;
	case ' ':
		paused = true;
		singleStep = true;
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
	default:
		//printf( "%d\n", key );
		break;
	}

	ImGuiIO& io = ImGui::GetIO();
	io.KeysDown[key] = true;

	int mod = glutGetModifiers( );
	io.KeyCtrl = (mod & GLUT_ACTIVE_CTRL) != 0;
	io.KeyShift = (mod & GLUT_ACTIVE_SHIFT) != 0;

	demos[ currentDemo ]->KeyDown( key );
}

void KeyboardUp(unsigned char key, int x, int y)
{

	ImGuiIO& io = ImGui::GetIO();
	io.KeysDown[key] = false;

	int mod = glutGetModifiers( );
	io.KeyCtrl = (mod & GLUT_ACTIVE_CTRL) != 0;
	io.KeyShift = (mod & GLUT_ACTIVE_SHIFT) != 0;

	demos[ currentDemo ]->KeyUp( key );
}

void Reshape( int width, int height )
{
	if ( height <= 0 )
		height = 1;

	windowWidth = width;
	windowHeight = height;

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

void UpdateImGui( float dt )
{
	ImGuiIO& io = ImGui::GetIO();

	io.DeltaTime = dt;
	io.DisplaySize.x = float( windowWidth );
	io.DisplaySize.y = float( windowHeight );
	io.MousePos = ImVec2( (float)mouseX, (float)mouseY + 8 );
	io.MouseDown[0] = mouseLeftDown;
	io.MouseDown[1] = mouseRightDown;
	
	// Start the frame
	ImGui::NewFrame();
}

// In main.cpp
void UpdateScene( f32 time );

void MainLoop( void )
{
	if ( currentDemo != lastDemo )
	{
		demos[ lastDemo ]->Shutdown( );
		demos[ currentDemo ]->Init( );
		lastDemo = currentDemo;
	}

	i32 w = glutGet( GLUT_WINDOW_WIDTH );
	Reshape( w, glutGet( GLUT_WINDOW_HEIGHT ) );

	f32 time = g_clock.Start( );

	UpdateImGui( 1.0f / 60.0f );

	ImGui::SetNewWindowDefaultPos( ImVec2( float( w - 300 - 30 ), 30 ) );
	ImGui::Begin( "q3Scene Settings", NULL, ImVec2( 300, 225 ) );
	ImGui::Combo( "Demo", &currentDemo, "Drop Boxes\0Ray Push\0Box Stack\0" );
	ImGui::Checkbox( "Pause", &paused );
	if ( paused )
		ImGui::Checkbox( "Single Step", &singleStep );
	ImGui::Checkbox( "Sleeping", &enableSleep );
	ImGui::Checkbox( "Friction", &enableFriction );
	ImGui::SliderInt( "Iterations", &velocityIterations, 1, 50 );
	int flags = (1 << 0) | (1 << 1) | (1 << 2);
	ImGui::InputText( "Dump File Name", sceneFileName, ((int)(sizeof(sceneFileName)/sizeof(*sceneFileName))), flags );
	if ( ImGui::Button( "Dump Scene" ) )
	{
		FILE* fp = fopen( sceneFileName, "w" );
		scene.Dump( fp );
		fclose( fp );
	}
	ImGui::End( );

	scene.SetAllowSleep( enableSleep );
	scene.SetEnableFriction( enableFriction );
	scene.SetIterations( velocityIterations );

	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	UpdateScene( time );

	g_clock.Stop( );

	scene.Render( &renderer );

	demos[ currentDemo ]->Render( &renderer );

	ImGui::Render( );

	glutSwapBuffers( );
}

void InitImGui( )
{
	int w = glutGet( GLUT_WINDOW_WIDTH );
	int h = glutGet( GLUT_WINDOW_HEIGHT );

	ImGuiIO& io = ImGui::GetIO();
	io.DisplaySize = ImVec2((float)w, (float)h);  // Display size, in pixels. For clamping windows positions.
	io.DeltaTime = 1.0f/60.0f;                          // Time elapsed since last frame, in seconds (in this sample app we'll override this every frame because our timestep is variable)
	io.PixelCenterOffset = 0.0f;                        // Align OpenGL texels
	io.KeyMap[ImGuiKey_Tab] = 9;             // Keyboard mapping. ImGui will use those indices to peek into the io.KeyDown[] array.
	io.KeyMap[ImGuiKey_LeftArrow] = GLUT_KEY_LEFT;
	io.KeyMap[ImGuiKey_RightArrow] = GLUT_KEY_RIGHT;
	io.KeyMap[ImGuiKey_UpArrow] = GLUT_KEY_UP;
	io.KeyMap[ImGuiKey_DownArrow] = GLUT_KEY_DOWN;
	io.KeyMap[ImGuiKey_Home] = GLUT_KEY_HOME;
	io.KeyMap[ImGuiKey_End] = GLUT_KEY_END;
	io.KeyMap[ImGuiKey_Delete] = 127;
	io.KeyMap[ImGuiKey_Backspace] = 8;
	io.KeyMap[ImGuiKey_Enter] = 13;
	io.KeyMap[ImGuiKey_Escape] = 27;
	io.KeyMap[ImGuiKey_A] = 'a';
	io.KeyMap[ImGuiKey_C] = 'c';
	io.KeyMap[ImGuiKey_V] = 'v';
	io.KeyMap[ImGuiKey_X] = 'x';
	io.KeyMap[ImGuiKey_Y] = 'y';
	io.KeyMap[ImGuiKey_Z] = 'z';

	io.RenderDrawListsFn = ImImpl_RenderDrawLists;
	//io.SetClipboardTextFn = ImImpl_SetClipboardTextFn;
	//io.GetClipboardTextFn = ImImpl_GetClipboardTextFn;

	glGenTextures(1, &fontTex);
	glBindTexture(GL_TEXTURE_2D, fontTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Default font (embedded in code)
	const void* png_data;
	unsigned int png_size;
	ImGui::GetDefaultFontData(NULL, NULL, &png_data, &png_size);
	int tex_x, tex_y, tex_comp;
	void* tex_data = stbi_load_from_memory((const unsigned char*)png_data, (int)png_size, &tex_x, &tex_y, &tex_comp, 0);
	IM_ASSERT(tex_data != NULL);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex_x, tex_y, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex_data);
	stbi_image_free(tex_data);

	// Disable usage of .ini file
	ImGui::GetIO( ).IniSavingRate = -1.0f;
}

int InitApp( int argc, char** argv )
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

	glutDisplayFunc( MainLoop );
	glutReshapeFunc( Reshape );
	glutKeyboardUpFunc( KeyboardUp );
	glutKeyboardFunc( Keyboard );
	glutMouseFunc( Mouse );
	glutMotionFunc( MouseMotion );
	glutPassiveMotionFunc( MouseMotion );
	glutIdleFunc( MainLoop );

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

	demos[ 0 ] = new DropBoxes( );
	demos[ 1 ] = new RayPush( );
	demos[ 2 ] = new BoxStack( );
	demoCount = 3;
	demos[ currentDemo ]->Init( );
	sprintf( sceneFileName, "q3dump.txt" );

	InitImGui( );
	glutMainLoop( );

	return 0;
}
