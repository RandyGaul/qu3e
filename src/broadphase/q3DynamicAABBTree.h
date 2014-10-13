//--------------------------------------------------------------------------------------------------
/**
@file	q3DynamicAABBTree.h

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

#ifndef Q3DYNAMICAABBTREE_H
#define Q3DYNAMICAABBTREE_H

#include "../math/q3Math.h"
#include "../common/q3Geometry.h"

//--------------------------------------------------------------------------------------------------
// q3DynamicAABBTree
//--------------------------------------------------------------------------------------------------
// Resources:
// http://box2d.org/2014/08/balancing-dynamic-trees/
// http://www.randygaul.net/2013/08/06/dynamic-aabb-tree/
class q3Render;

class q3DynamicAABBTree
{
public:
	q3DynamicAABBTree( );
	~q3DynamicAABBTree( );

	// Provide tight-AABB
	i32 Insert( const q3AABB& aabb, void *userData );
	void Remove( i32 id );
	bool Update( i32 id, const q3AABB& aabb );

	void *GetUserData( i32 id ) const;
	const q3AABB& GetFatAABB( i32 id ) const;
	void Render( q3Render *render ) const;

	template <typename T>
	void Query( T *cb, const q3AABB& aabb ) const;
	template <typename T>
	void Query( T *cb, q3RaycastData& rayCast ) const;

	// For testing
	void Validate( ) const;

private:
	struct Node
	{
		bool IsLeaf( void ) const
		{
			// The right leaf does not use the same memory as the userdata,
			// and will always be Null (no children)
			return right == Null;
		}

		// Fat AABB for leafs, bounding AABB for branches
		q3AABB aabb;

		union
		{
			i32 parent;
			i32 next; // free list
		};

		// Child indices
		struct
		{
			i32 left;
			i32 right;
		};

		// Since only leaf nodes hold userdata, we can use the
		// same memory used for left/right indices to store
		// the userdata void pointer
		void *userData;

		// leaf = 0, free nodes = -1
		i32 height;

		static const i32 Null = -1;
	};

	inline i32 AllocateNode( );
	inline void DeallocateNode( i32 index );
	i32 Balance( i32 index );
	void InsertLeaf( i32 index );
	void RemoveLeaf( i32 index );
	void ValidateStructure( i32 index ) const;
	void RenderNode( q3Render *render, i32 index ) const;

	// Correct AABB hierarchy heights and AABBs starting at supplied
	// index traversing up the heirarchy
	void SyncHeirarchy( i32 index );

	// Insert nodes at a given index until m_capacity into the free list
	void AddToFreeList( i32 index );

	i32 m_root;
	Node *m_nodes;
	i32 m_count;	// Number of active nodes
	i32 m_capacity;	// Max capacity of nodes
	i32 m_freeList;
};

#include "q3DynamicAABBTree.inl"

#endif // Q3DYNAMICAABBTREE_H
