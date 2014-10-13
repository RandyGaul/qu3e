//--------------------------------------------------------------------------------------------------
/**
@file	q3Types.h

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

#ifndef Q3TYPES_H
#define Q3TYPES_H

typedef float r32;
typedef double r64;
typedef float f32;
typedef double f64;
typedef signed char	i8;
typedef signed short i16;
typedef signed int i32;
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

#define Q3_UNUSED( A ) \
	(void)A

#endif // Q3TYPES_H
