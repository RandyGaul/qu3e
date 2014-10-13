//--------------------------------------------------------------------------------------------------
// q3Box.inl
//
//	Copyright (c) 2014 Randy Gaul http://www.randygaul.net
//
//	This software is provided 'as-is', without any express or implied
//	warranty. In no event will the authors be held liable for any damages
//	arising from the use of this software.
//
//	Permission is granted to anyone to use this software for any purpose,
//	including commercial applications, and to alter it and redistribute it
//	freely, subject to the following restrictions:
//	  1. The origin of this software must not be misrepresented; you must not
//	     claim that you wrote the original software. If you use this software
//	     in a product, an acknowledgment in the product documentation would be
//	     appreciated but is not required.
//	  2. Altered source versions must be plainly marked as such, and must not
//	     be misrepresented as being the original software.
//	  3. This notice may not be removed or altered from any source distribution.
//--------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------
// q3Box
//--------------------------------------------------------------------------------------------------
inline void q3Box::SetUserdata( void* data ) const
{
	userData = data;
}

//--------------------------------------------------------------------------------------------------
inline void* q3Box::GetUserdata( ) const
{
	return userData;
}

//--------------------------------------------------------------------------------------------------
// q3BoxDef
//--------------------------------------------------------------------------------------------------
inline void q3BoxDef::Set( const q3Transform& tx, const q3Vec3& extents )
{
	m_tx = tx;
	m_e = extents * r32( 0.5 );
}

//--------------------------------------------------------------------------------------------------
inline void q3BoxDef::SetRestitution( r32 restitution )
{
	m_restitution = restitution;
}

//--------------------------------------------------------------------------------------------------
inline void q3BoxDef::SetFriction( r32 friction )
{
	m_friction = friction;
}

//--------------------------------------------------------------------------------------------------
inline void q3BoxDef::SetDensity( r32 density )
{
	m_density = density;
}

//--------------------------------------------------------------------------------------------------
inline void q3BoxDef::SetSensor( bool sensor )
{
	m_sensor = sensor;
}
