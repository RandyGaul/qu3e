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

#include "Clock.h"

const float kClockDivisor = 1.0f / (float)std::chrono::duration_cast<clock_freq>( std::chrono::seconds( 1 ) ).count( );

Clock::Clock( )
{
	// Setup initial times
	m_start = hr_clock::now( );
	m_stop = hr_clock::now( );
}

float Clock::Start( void )
{
	m_start = hr_clock::now( );
	return std::chrono::duration_cast<clock_freq>( m_start - m_stop ).count( ) * kClockDivisor;
}

void Clock::Stop( )
{
	m_stop = hr_clock::now( );
}
