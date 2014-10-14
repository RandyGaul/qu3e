//--------------------------------------------------------------------------------------------------
/**
@file	q3Math.h

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

#ifndef Q3MATH_H
#define Q3MATH_H

#include <float.h>		// FLT_MAX
#include <cmath>		// abs, sqrt
#include <cassert>		// assert
#include <algorithm>	// max, min

#include "../common/q3Types.h"

//--------------------------------------------------------------------------------------------------
// q3Math
//--------------------------------------------------------------------------------------------------
#define Q3_R32_MAX FLT_MAX

const r32 q3PI = r32( 3.14159265 );

#include "q3Vec3.h"
#include "q3Mat3.h"
#include "q3Quaternion.h"

#include "q3Math.inl"

#endif // Q3MATH_H
