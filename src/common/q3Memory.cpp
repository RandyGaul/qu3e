//--------------------------------------------------------------------------------------------------
/**
@file	q3Memory.cpp

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

#include <cassert>
#include <cstdio>

#include "q3Memory.h"
#include "../math/q3Math.h"

//--------------------------------------------------------------------------------------------------
// q3Stack
//--------------------------------------------------------------------------------------------------
q3Stack::q3Stack( )
	: m_entries( (q3StackEntry*)q3Alloc( sizeof( q3StackEntry ) * 64 ) )
	, m_index( 0 )
	, m_allocation( 0 )
	, m_entryCount( 0 )
	, m_entryCapacity( 64 )
{
}

//--------------------------------------------------------------------------------------------------
q3Stack::~q3Stack( )
{
	assert( m_index == 0 );
	assert( m_entryCount == 0 );
}

//--------------------------------------------------------------------------------------------------
void *q3Stack::Allocate( i32 size )
{
	if ( m_entryCount == m_entryCapacity )
	{
		q3StackEntry* oldEntries = m_entries;
		m_entryCapacity *= 2;
		m_entries = (q3StackEntry*)q3Alloc( m_entryCapacity * sizeof( q3StackEntry ) );
		memcpy( m_entries, oldEntries, m_entryCount * sizeof( q3StackEntry ) );
		q3Free( oldEntries );
	}

	q3StackEntry* entry = m_entries + m_entryCount;
	entry->size = size;

	// Make sure enough memory is available for this allocation
	assert( m_index + size <= q3k_stackSize );

	entry->data = m_memory + m_index;
	m_index += size;

	m_allocation += size;
	++m_entryCount;

	return entry->data;
}

//--------------------------------------------------------------------------------------------------
void q3Stack::Free( void *data )
{
	// Cannot call free when there are no entries.
	assert( m_entryCount > 0 );

	q3StackEntry *entry = m_entries + m_entryCount - 1;

	// Validate that the data * is a proper location to free.
	// Must be in reverse order of allocation.
	assert( data == entry->data );

	m_index -= entry->size;

	m_allocation -= entry->size;
	--m_entryCount;
}

//--------------------------------------------------------------------------------------------------
// q3Heap
//--------------------------------------------------------------------------------------------------
q3Heap::q3Heap( )
{
	m_memory = (q3Header*)q3Alloc( q3k_heapSize );
	m_memory->next = NULL;
	m_memory->prev = NULL;
	m_memory->size = q3k_heapSize;

	m_freeBlocks = (q3FreeBlock*)q3Alloc( sizeof( q3FreeBlock ) * q3k_heapInitialCapacity );
	m_freeBlockCount = 1;
	m_freeBlockCapacity = q3k_heapInitialCapacity;

	m_freeBlocks->header = m_memory;
	m_freeBlocks->size = q3k_heapSize;
}

//--------------------------------------------------------------------------------------------------
q3Heap::~q3Heap( )
{
	q3Free( m_memory );
	q3Free( m_freeBlocks );
}

//--------------------------------------------------------------------------------------------------
void *q3Heap::Allocate( i32 size )
{
	i32 sizeNeeded = size + sizeof( q3Header );
	q3FreeBlock* firstFit = NULL;

	for ( i32 i = 0; i < m_freeBlockCount; ++i )
	{
		q3FreeBlock* block = m_freeBlocks + i;

		if ( block->size >= sizeNeeded )
		{
			firstFit = block;
			break;
		}
	}

	if ( !firstFit )
		return NULL;

	q3Header* node = firstFit->header;
	q3Header* newNode = Q3_PTR_ADD( node, sizeNeeded );
	node->size = sizeNeeded;

	firstFit->size -= sizeNeeded;
	firstFit->header = newNode;

	newNode->next = node->next;
	if ( node->next )
		node->next->prev = newNode;
	node->next = newNode;
	newNode->prev = node;

	return Q3_PTR_ADD( node, sizeof( q3Header ) );
}

//--------------------------------------------------------------------------------------------------
void q3Heap::Free( void *memory )
{
	assert( memory );
	q3Header* node = (q3Header*)Q3_PTR_ADD( memory, -i32( sizeof( q3Header ) ) );

	q3Header* next = node->next;
	q3Header* prev = node->prev;
	q3FreeBlock* nextBlock = NULL;
	i32 prevBlockIndex = ~0;
	q3FreeBlock* prevBlock = NULL;
	i32 freeBlockCount = m_freeBlockCount;

	for ( i32 i = 0; i < freeBlockCount; ++i )
	{
		q3FreeBlock* block = m_freeBlocks + i;
		q3Header* header = block->header;

		if ( header == next )
			nextBlock = block;

		else if ( header == prev )
		{
			prevBlock = block;
			prevBlockIndex = i;
		}
	}

	bool merged = false;

	if ( prevBlock )
	{
		merged = true;

		prev->next = next;
		if ( next )
			next->prev = prev;

		prevBlock->size += node->size;
		prev->size = prevBlock->size;

		if ( nextBlock )
		{
			nextBlock->header = prev;
			nextBlock->size += prev->size;
			prev->size = nextBlock->size;

			q3Header* nextnext = next->next;
			prev->next = nextnext;

			if ( nextnext )
				nextnext->prev = prev;

			// Remove the nextBlock from the freeBlocks array
			assert( m_freeBlockCount );
			assert( prevBlockIndex != ~0 );
			--m_freeBlockCount;
			m_freeBlocks[ prevBlockIndex ] = m_freeBlocks[ m_freeBlockCount ];
		}
	}

	else if ( nextBlock )
	{
		merged = true;

		nextBlock->header = node;
		nextBlock->size += node->size;
		node->size = nextBlock->size;

		q3Header* nextnext = next->next;

		if ( nextnext )
			nextnext->prev = node;

		node->next = nextnext;
	}

	if ( !merged )
	{
		q3FreeBlock block;
		block.header = node;
		block.size = node->size;

		if ( m_freeBlockCount == m_freeBlockCapacity )
		{
			q3FreeBlock* oldBlocks = m_freeBlocks;
			i32 oldCapacity = m_freeBlockCapacity;

			m_freeBlockCapacity *= 2;
			m_freeBlocks = (q3FreeBlock*)q3Alloc( sizeof( q3FreeBlock ) * m_freeBlockCapacity );

			memcpy( m_freeBlocks, oldBlocks, sizeof( q3FreeBlock ) * oldCapacity );
			q3Free( oldBlocks );
		}

		m_freeBlocks[ m_freeBlockCount++ ] = block;
	}
}

//--------------------------------------------------------------------------------------------------
// q3PagedAllocator
//--------------------------------------------------------------------------------------------------
q3PagedAllocator::q3PagedAllocator( i32 elementSize, i32 elementsPerPage )
{
	m_blockSize = elementSize;
	m_blocksPerPage = elementsPerPage;

	m_pages = NULL;
	m_pageCount = 0;

	m_freeList = NULL;
}

//--------------------------------------------------------------------------------------------------
q3PagedAllocator::~q3PagedAllocator( )
{
	Clear( );
}

//--------------------------------------------------------------------------------------------------
void* q3PagedAllocator::Allocate( )
{
	if ( m_freeList )
	{
		q3Block* data = m_freeList;
		m_freeList = data->next;

		return data;
	}

	else
	{
		q3Page* page = (q3Page*)q3Alloc( m_blockSize * m_blocksPerPage + sizeof( q3Page ) );
		++m_pageCount;

		page->next = m_pages;
		page->data = (q3Block*)Q3_PTR_ADD( page, sizeof( q3Page ) );
		m_pages = page;

		i32 blocksPerPageMinusOne = m_blocksPerPage - 1;
		for ( i32 i = 0; i < blocksPerPageMinusOne; ++i )
		{
			q3Block *node = Q3_PTR_ADD( page->data, m_blockSize * i );
			q3Block *next = Q3_PTR_ADD( page->data, m_blockSize * (i + 1) );
			node->next = next;
		}

		q3Block *last = Q3_PTR_ADD( page->data, m_blockSize * (blocksPerPageMinusOne) );
		last->next = NULL;

		m_freeList = page->data->next;

		return page->data;
	}
}

//--------------------------------------------------------------------------------------------------
void q3PagedAllocator::Free( void* data )
{
#ifdef _DEBUG
	bool found = false;

	for ( q3Page *page = m_pages; page; page = page->next )
	{
		if ( data >= page->data && data < Q3_PTR_ADD( page->data, m_blockSize * m_blocksPerPage ) )
		{
			found = true;
			break;
		}
	}

	// Address of data does not lie within any pages of this allocator.
	assert( found );
#endif // DEBUG

	((q3Block*)data)->next = m_freeList;
	m_freeList = ((q3Block*)data);
}

//--------------------------------------------------------------------------------------------------
void q3PagedAllocator::Clear( )
{
	q3Page* page = m_pages;

	for ( i32 i = 0; i < m_pageCount; ++i )
	{
		q3Page* next = page->next;
		q3Free( page );
		page = next;
	}

	m_freeList = NULL;
	m_pageCount = 0;
}
