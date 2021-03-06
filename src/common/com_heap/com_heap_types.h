#pragma once

#include "../common.h"


/*****************************
 * A reference to specific block on the Heap
 * do NOT use ptrMemory directly. Grab it via
 * Heap_GetBlockMemoryAddress(heap, blockRef)
 * to make sure the pointer is up to date
 ****************************/
struct BlockRef
{
    uint32_t id;            // identify this allocation, for searching
    uint32_t iteration;   // if this fails to match the current heap iteration, the pointer is invalid
    void* ptrMemory;
    uint32_t objectSize;
};

/*****************************
 * Information about an allocated memory block on the heap
 * Stored in the heap as a header in front of the memory block itself
 ****************************/
union HeapBlock;
struct HeapBlockData
{
    uint32_t id;             // identify this allocation, for searching
    void* ptrMemory;
    uint32_t objectSize;
	uint32_t volumeSize;
	// ^ 16 bytes
	HeapBlock* next;
	HeapBlock* prev; // <-- 24 bytes
	char debugLabel[BLOCK_LABEL_SIZE + 1]; // 8 + null terminator
	// sizeof in 32 bit, 29 bytes, or 2 16 aligned blocks
};

// Union to enforce a specific minimum size of the HeapBlock
union HeapBlock
{
    HeapBlockData mem;
    uint8_t sizePadding[48]; // Force the size of a heap block
};

/*****************************
 * Describes a gap in the heap between either
 * HeapOrigin <gap> First Block
 * or
 * Block <gap> Block
 ****************************/
struct Heap_Fragment
{
    // Distance into heap the gap starts at
    u32 originOffset;
    u32 space;
    
    // u32 destinationOffset;
    // // Distance to copy
    // u32 distance;
    // u32 numBytes;
};

/*****************************
 * A specific Heap, in case you want multiple heaps
 ****************************/
struct Heap
{
    uint32_t nextID;
    uint32_t alignmentBytes;
	void* ptrMemory;
    void* ptrEnd;
	uint32_t size;
	// Increment for every defragmentation run.
    // Allows references to know if their pointer is out of date.
	uint32_t iteration;

    HeapBlock *headBlock;
    HeapBlock *tailBlock;
	
	// Stats really
	uint32_t usedSpace;
	uint32_t freeSpace;
};
