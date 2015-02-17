//--------------------------------------------------------------------------------------------------
/**
@file	q3DynamicAABBTree.cpp

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

#include "q3DynamicAABBTree.h"
#include "../debug/q3Render.h"
#include "../common/q3Memory.h"

//--------------------------------------------------------------------------------------------------
// q3DynamicAABBTree
//--------------------------------------------------------------------------------------------------
inline void FattenAABB( q3AABB& aabb )
{
	const r32 k_fattener = r32( 0.5 );
	q3Vec3 v( k_fattener, k_fattener, k_fattener );

	aabb.min -= v;
	aabb.max += v;
}

//--------------------------------------------------------------------------------------------------
q3DynamicAABBTree::q3DynamicAABBTree( )
{
	m_root = Node::Null;

	m_capacity = 1024;
	m_count = 0;
	m_nodes = (Node *)q3Alloc( sizeof( Node ) * m_capacity );

	AddToFreeList( 0 );
}

//--------------------------------------------------------------------------------------------------
q3DynamicAABBTree::~q3DynamicAABBTree( )
{
	q3Free( m_nodes );
}

//--------------------------------------------------------------------------------------------------
i32 q3DynamicAABBTree::Insert( const q3AABB& aabb, void *userData )
{
	i32 id = AllocateNode( );

	// Fatten AABB and set height/userdata
	m_nodes[ id ].aabb = aabb;
	FattenAABB( m_nodes[id].aabb );
	m_nodes[ id ].userData = userData;
	m_nodes[ id ].height = 0;

	InsertLeaf( id );

	return id;
}

void q3DynamicAABBTree::Remove( i32 id )
{
	assert( id >= 0 && id < m_capacity );
	assert( m_nodes[ id ].IsLeaf( ) );

	RemoveLeaf( id );
	DeallocateNode( id );
}

bool q3DynamicAABBTree::Update( i32 id, const q3AABB& aabb )
{
	assert( id >= 0 && id < m_capacity );
	assert( m_nodes[ id ].IsLeaf( ) );

	if ( m_nodes[ id ].aabb.Contains( aabb ) )
		return false;

	RemoveLeaf( id );

	m_nodes[ id ].aabb = aabb;
	FattenAABB( m_nodes[ id ].aabb );

	InsertLeaf( id );

	return true;
}

void *q3DynamicAABBTree::GetUserData( i32 id ) const
{
	assert( id >= 0 && id < m_capacity );

	return m_nodes[ id ].userData;
}

const q3AABB& q3DynamicAABBTree::GetFatAABB( i32 id ) const
{
	assert( id >= 0 && id < m_capacity );

	return m_nodes[ id ].aabb;
}

void q3DynamicAABBTree::Render( q3Render *render ) const
{
	if ( m_root != Node::Null )
	{
		render->SetPenColor( 0.5f, 0.5f, 1.0f );
		RenderNode( render, m_root );
	}
}

void q3DynamicAABBTree::RenderNode( q3Render *render, i32 index ) const
{
	assert( index >= 0 && index < m_capacity );

	Node *n = m_nodes + index;
	const q3AABB& b = n->aabb;

	render->SetPenPosition( b.min.x, b.max.y, b.min.z );

	render->Line( b.min.x, b.max.y, b.max.z );
	render->Line( b.max.x, b.max.y, b.max.z );
	render->Line( b.max.x, b.max.y, b.min.z );
	render->Line( b.min.x, b.max.y, b.min.z );

	render->SetPenPosition( b.min.x, b.min.y, b.min.z );

	render->Line( b.min.x, b.min.y, b.max.z );
	render->Line( b.max.x, b.min.y, b.max.z );
	render->Line( b.max.x, b.min.y, b.min.z );
	render->Line( b.min.x, b.min.y, b.min.z );

	render->SetPenPosition( b.min.x, b.min.y, b.min.z );
	render->Line( b.min.x, b.max.y, b.min.z );
	render->SetPenPosition( b.max.x, b.min.y, b.min.z );
	render->Line( b.max.x, b.max.y, b.min.z );
	render->SetPenPosition( b.max.x, b.min.y, b.max.z );
	render->Line( b.max.x, b.max.y, b.max.z );
	render->SetPenPosition( b.min.x, b.min.y, b.max.z );
	render->Line( b.min.x, b.max.y, b.max.z );

	if ( !n->IsLeaf( ) )
	{
		RenderNode( render, n->left );
		RenderNode( render, n->right );
	}
}

void q3DynamicAABBTree::Validate( ) const
{
	// Verify free list
	i32 freeNodes = 0;
	i32 index = m_freeList;

	while ( index != Node::Null )
	{
		assert( index >= 0 && index < m_capacity );
		index = m_nodes[ index ].next;
		++freeNodes;
	}

	assert( m_count + freeNodes == m_capacity );

	// Validate tree structure
	if ( m_root != Node::Null )
	{
		assert( m_nodes[m_root].parent == Node::Null );

#ifdef _DEBUG
		ValidateStructure( m_root );
#endif
	}
}

void q3DynamicAABBTree::ValidateStructure( i32 index ) const
{
	Node *n = m_nodes + index;

	i32 il = n->left;
	i32 ir = n->right;

	if ( n->IsLeaf( ) )
	{
		assert( ir == Node::Null );
		assert( n->height == 0 );
		return;
	}

	assert( il >= 0 && il < m_capacity );
	assert( ir >= 0 && ir < m_capacity );
	Node *l = m_nodes + il;
	Node *r = m_nodes + ir;

	assert( l->parent == index );
	assert( r->parent == index );

	ValidateStructure( il );
	ValidateStructure( ir );
}

i32 q3DynamicAABBTree::AllocateNode( )
{
	if ( m_freeList == Node::Null )
	{
		m_capacity *= 2;
		Node *newNodes = (Node *)q3Alloc( sizeof( Node ) * m_capacity );
		memcpy( newNodes, m_nodes, sizeof( Node ) * m_count );
		q3Free( m_nodes );
		m_nodes = newNodes;

		AddToFreeList( m_count );
	}

	i32 freeNode = m_freeList;
	m_freeList = m_nodes[m_freeList].next;
	m_nodes[ freeNode ].height = 0;
	m_nodes[ freeNode ].left = Node::Null;
	m_nodes[ freeNode ].right = Node::Null;
	m_nodes[ freeNode ].parent = Node::Null;
	m_nodes[ freeNode ].userData = NULL;
	++m_count;
	return freeNode;
}

i32 q3DynamicAABBTree::Balance( i32 iA )
{
	Node *A = m_nodes + iA;

	if ( A->IsLeaf( ) || A->height == 1 )
		return iA;

	//      A
	//    /   \
	//   B     C
	//  / \   / \
	// D   E F   G

	i32 iB = A->left;
	i32 iC = A->right;
	Node *B = m_nodes + iB;
	Node *C = m_nodes + iC;

	i32 balance = C->height - B->height;

	// C is higher, promote C
	if ( balance > 1 )
	{
		i32 iF = C->left;
		i32 iG = C->right;
		Node *F = m_nodes + iF;
		Node *G = m_nodes + iG;

		// grandParent point to C
		if ( A->parent != Node::Null )
		{
			if(m_nodes[ A->parent ].left == iA)
				m_nodes[ A->parent ].left = iC;

			else
				m_nodes[ A->parent ].right = iC;
		}
		else
			m_root = iC;

		// Swap A and C
		C->left = iA;
		C->parent = A->parent;
		A->parent = iC;

		// Finish rotation
		if ( F->height > G->height )
		{
			C->right = iF;
			A->right = iG;
			G->parent = iA;
			A->aabb = q3Combine( B->aabb, G->aabb );
			C->aabb = q3Combine( A->aabb, F->aabb );

			A->height = 1 + q3Max( B->height, G->height );
			C->height = 1 + q3Max( A->height, F->height );
		}

		else
		{
			C->right = iG;
			A->right = iF;
			F->parent = iA;
			A->aabb = q3Combine( B->aabb, F->aabb );
			C->aabb = q3Combine( A->aabb, G->aabb );

			A->height = 1 + q3Max( B->height, F->height );
			C->height = 1 + q3Max( A->height, G->height );
		}

		return iC;
	}

	// B is higher, promote B
	else if ( balance < -1 )
	{
		i32 iD = B->left;
		i32 iE = B->right;
		Node *D = m_nodes + iD;
		Node *E = m_nodes + iE;

		// grandParent point to B
		if ( A->parent != Node::Null )
		{
			if(m_nodes[ A->parent ].left == iA)
				m_nodes[ A->parent ].left = iB;
			else
				m_nodes[ A->parent ].right = iB;
		}

		else
			m_root = iB;

		// Swap A and B
		B->right = iA;
		B->parent = A->parent;
		A->parent = iB;

		// Finish rotation
		if ( D->height > E->height )
		{
			B->left = iD;
			A->left = iE;
			E->parent = iA;
			A->aabb = q3Combine( C->aabb, E->aabb );
			B->aabb = q3Combine( A->aabb, D->aabb );

			A->height = 1 + q3Max( C->height, E->height );
			B->height = 1 + q3Max( A->height, D->height );
		}

		else
		{
			B->left = iE;
			A->left = iD;
			D->parent = iA;
			A->aabb = q3Combine( C->aabb, D->aabb );
			B->aabb = q3Combine( A->aabb, E->aabb );

			A->height = 1 + q3Max( C->height, D->height );
			B->height = 1 + q3Max( A->height, E->height );
		}

		return iB;
	}

	return iA;
}

void q3DynamicAABBTree::InsertLeaf( i32 id )
{
	if ( m_root == Node::Null )
	{
		m_root = id;
		m_nodes[ m_root ].parent = Node::Null;
		return;
	}

	// Search for sibling
	i32 searchIndex = m_root;
	q3AABB leafAABB = m_nodes[ id ].aabb;
	while ( !m_nodes[ searchIndex ].IsLeaf( ) )
	{
		// Cost for insertion at index (branch node), involves creation
		// of new branch to contain this index and the new leaf
		q3AABB combined = q3Combine( leafAABB, m_nodes[ searchIndex ].aabb );
		r32 combinedArea = combined.SurfaceArea( );
		r32 branchCost = r32( 2.0 ) * combinedArea;

		// Inherited cost (surface area growth from heirarchy update after descent)
		r32 inheritedCost = r32( 2.0 ) * (combinedArea - m_nodes[ searchIndex ].aabb.SurfaceArea( ));

		i32 left = m_nodes[ searchIndex ].left;
		i32 right = m_nodes[ searchIndex ].right;

		// Calculate costs for left/right descents. If traversal is to a leaf,
		// then the cost of the combind AABB represents a new branch node. Otherwise
		// the cost is only the inflation of the pre-existing branch.
		r32 leftDescentCost;
		if ( m_nodes[left].IsLeaf( ) )
			leftDescentCost = q3Combine( leafAABB, m_nodes[left].aabb ).SurfaceArea( ) + inheritedCost;
		else
		{
			r32 inflated = q3Combine( leafAABB, m_nodes[left].aabb ).SurfaceArea( );
			r32 branchArea = m_nodes[ left ].aabb.SurfaceArea( );
			leftDescentCost = inflated - branchArea + inheritedCost;
		}

		// Cost for right descent
		r32 rightDescentCost;
		if ( m_nodes[right].IsLeaf( ) )
			rightDescentCost = q3Combine( leafAABB, m_nodes[right].aabb ).SurfaceArea( ) + inheritedCost;
		else
		{
			r32 inflated = q3Combine( leafAABB, m_nodes[right].aabb ).SurfaceArea( );
			r32 branchArea = m_nodes[ right ].aabb.SurfaceArea( );
			rightDescentCost = inflated - branchArea + inheritedCost;
		}

		// Determine traversal direction, or early out on a branch index
		if ( branchCost < leftDescentCost && branchCost < rightDescentCost )
			break;

		if ( leftDescentCost < rightDescentCost )
			searchIndex = left;

		else
			searchIndex = right;
	}

	i32 sibling = searchIndex;

	// Create new parent
	i32 oldParent = m_nodes[sibling].parent;
	i32 newParent = AllocateNode( );
	m_nodes[ newParent ].parent = oldParent;
	m_nodes[ newParent ].userData = NULL;
	m_nodes[ newParent ].aabb = q3Combine( leafAABB, m_nodes[sibling].aabb );
	m_nodes[ newParent ].height = m_nodes[sibling].height + 1;

	// Sibling was root
	if ( oldParent == Node::Null )
	{
		m_nodes[ newParent ].left = sibling;
		m_nodes[ newParent ].right = id;
		m_nodes[ sibling ].parent = newParent;
		m_nodes[ id ].parent = newParent;
		m_root = newParent;
	}

	else
	{
		if ( m_nodes[ oldParent ].left == sibling )
			m_nodes[ oldParent ].left = newParent;

		else
			m_nodes[ oldParent ].right = newParent;

		m_nodes[ newParent ].left = sibling;
		m_nodes[ newParent ].right = id;
		m_nodes[ sibling ].parent = newParent;
		m_nodes[ id ].parent = newParent;
	}

	SyncHeirarchy( m_nodes[ id ].parent );
}

void q3DynamicAABBTree::RemoveLeaf( i32 id )
{
	if ( id == m_root )
	{
		m_root = Node::Null;
		return;
	}

	// Setup parent, grandParent and sibling
	i32 parent = m_nodes[ id ].parent;
	i32 grandParent = m_nodes[parent].parent;
	i32 sibling;

	if ( m_nodes[ parent ].left == id )
		sibling = m_nodes[ parent ].right;

	else
		sibling = m_nodes[ parent ].left;

	// Remove parent and replace with sibling
	if ( grandParent != Node::Null )
	{
		// Connect grandParent to sibling
		if ( m_nodes[ grandParent ].left == parent )
			m_nodes[ grandParent ].left = sibling;

		else
			m_nodes[ grandParent ].right = sibling;

		// Connect sibling to grandParent
		m_nodes[ sibling ].parent = grandParent;
	}

	// Parent was root
	else
	{
		m_root = sibling;
		m_nodes[ sibling ].parent = Node::Null;
	}

	DeallocateNode( parent );
	SyncHeirarchy( grandParent );
}

void q3DynamicAABBTree::SyncHeirarchy( i32 index )
{
	while ( index != Node::Null )
	{
		index = Balance( index );

		i32 left = m_nodes[ index ].left;
		i32 right = m_nodes[ index ].right;

		m_nodes[ index ].height = 1 + q3Max( m_nodes[ left ].height, m_nodes[ right ].height );
		m_nodes[ index ].aabb = q3Combine( m_nodes[ left ].aabb, m_nodes[ right ].aabb );

		index = m_nodes[ index ].parent;
	}
}
