//--------------------------------------------------------------------------------------------------
/**
@file	q3ContactManager.h

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

#ifndef Q3CONTACTMANAGER_H
#define Q3CONTACTMANAGER_H

#include "../common/q3Types.h"
#include "../broadphase/q3BroadPhase.h"
#include "../common/q3Memory.h"

//--------------------------------------------------------------------------------------------------
// q3ContactManager
//--------------------------------------------------------------------------------------------------
struct q3ContactConstraint;
class q3ContactListener;
struct q3Box;
class q3Body;
class q3Render;
class q3Stack;

class q3ContactManager
{
public:
	q3ContactManager( q3Stack* stack );

	// Add a new contact constraint for a pair of objects
	// unless the contact constraint already exists
	void AddContact( q3Box *A, q3Box *B );

	// Has broadphase find all contacts and call AddContact on the
	// ContactManager for each pair found
	void FindNewContacts( void );

	// Remove a specific contact
	void RemoveContact( q3ContactConstraint *contact );

	// Remove all contacts from a body
	void RemoveContactsFromBody( q3Body *body );
	void RemoveFromBroadphase( q3Body *body );

	// Remove contacts without broadphase overlap
	// Solves contact manifolds
	void TestCollisions( void );
	static void SolveCollision( void* param );

	void RenderContacts( q3Render* debugDrawer ) const;

private:
	q3ContactConstraint* m_contactList;
	i32 m_contactCount;
	q3Stack* m_stack;
	q3PagedAllocator m_allocator;
	q3BroadPhase m_broadphase;
	q3ContactListener *m_contactListener;

	friend class q3BroadPhase;
	friend class q3Scene;
	friend struct q3Box;
	friend class q3Body;
};

#endif // Q3CONTACTMANAGER_H
