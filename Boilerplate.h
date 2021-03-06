#ifndef __Boilerplate_INCLUDED__
#define __Boilerplate_INCLUDED__

// Texture constants
#define GL_TEXTURE_MAX_ANISOTROPY_EXT 0x84FE
#define GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT 0x84FF

// Constants for circles
#define DEGREES_TO_RADIANS  .017453292F
#define RADIANS_TO_DEGREES  57.29577951F
#define PI					3.141592654F
#define TWO_PI				6.283185307F

// Include standard system header files.
#include <iostream>
#include <time.h>
#include <stdlib.h>
#include <cmath>

// Include all the other useful headers.
#include "Vec2.h"
#include "Vec3.h"
#include "Mat4x4.h"
#include "Color.h"
#include "Camera.h"

// Include the GLUT header, which will include both gl.h and glu.h.
#ifdef WIN32
#include <glut.h>
#else
#include <GL/glut.h> //Should be Mac OS X standard in Xcode projects.
#endif

// Define "GL look-alike" functions to accept instances of own classes
// as arguments.

// Take transpose for all Matrix functions as C++ is row major but OpenGL assumes
// column major for matrices
inline void glLoadMatrix( const Mat4x4 &M )
{
	glLoadIdentity();
	// MUST take the transpose here
	glLoadMatrixd( Transpose(M).array() );
}

inline void glMultMatrix( const Mat4x4 &M )
{
	// MUST take the transpose here
	glMultMatrixd( Transpose(M).array() );
}

// Take own classes for other gl functions
inline void glVertex( const Vec2 &P )
{
	glVertex2d( P.x, P.y );
}

inline void glVertex( const Vec3 &P )
{
	glVertex3d( P.x, P.y, P.z );
}

inline void glNormal( const Vec3 &N )
{
	glNormal3d( N.x, N.y, N.z );
}

inline void glColor( const Color &C, double alpha = 1.0 )
{
	glColor4d( C.red, C.green, C.blue, alpha );
}

inline void gluLookAt( const Vec3 &eye, const Vec3 &cent, const Vec3 &up )
{
	gluLookAt(
		eye .x, eye .y, eye .z,  // Position of the eye.
		cent.x, cent.y, cent.z,  // The "look at" point, or "center".
		up  .x, up  .y, up  .z   // The "up vector".
		);
}

inline void gluLookAt(const Camera &cam)
{
	gluLookAt(cam.eye, cam.center, cam.up);
}

// Set the position of the specified light source.
inline void set_light_position( int index, const Vec3 &pos )
{
	float p[] = { pos.x, pos.y, pos.z, 1.0f };
	glLightfv( index, GL_POSITION, p );
}

// Set the given color field (one of GL_AMBIENT, GL_DIFFUSE, or GL_SPECULAR) to the
// specified color.  The material that is affected is the "current" material.
inline void glMaterial( int field, const Color &color, double alpha = 1.0 )
{
	float c[] = { color.red, color.green, color.blue, alpha };
	glMaterialfv( GL_FRONT_AND_BACK, field, c );
}

// Set the given scalar field (e.g. GL_SHININESS) to the specified value.
// The material that is affected is the "current" material.
inline void glMaterial( int field, double value )
{
	glMaterialf( GL_FRONT_AND_BACK, field, value );
}

// Create an enuerated type for the special keys recognized by OpenGL.
// These include function keys and arrow keys.
enum special_key {
	F1_key          = 1,
	F2_key          = 2,
	F3_key          = 3,
	F4_key          = 4,
	F5_key          = 5,
	F6_key          = 6,
	F7_key          = 7,
	F8_key          = 8,
	F9_key          = 9,
	F10_key         = 10,
	F11_key         = 11,
	F12_key         = 12,
	left_shift		= 27,
	left_arrow_key  = 100,
	up_arrow_key    = 101,
	right_arrow_key = 102,
	down_arrow_key  = 103,
	page_up_key     = 104,
	page_down_key   = 105,
	home_key        = 106,
	end_key         = 107,
	insert_key      = 108
};

#endif

