#pragma once

#include <stdint.h>

/*****************************
 * A heap allocation header
 ****************************/
struct HeapRef
{
    uint32_t id;            // identify this allocation, for searching
    uint32_t defragIndex;   // if this fails to match the current heap defragIndex, the pointer is invalid
    void* ptrMemory;
    uint32_t size;
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
	
	HeapBlock* next;
	HeapBlock* prev;
	char debugLabel[8];
	// sizeof in 32 bit, 28 bytes, or 2 16 aligned blocks
};

union HeapBlock
{
    HeapBlockData mem;
    uint8_t sizePadding[32];
};

/*****************************
 * A specific Heap, in case you want multiple heaps
 ****************************/
struct Heap
{
    uint32_t nextID;
	void* ptrMemory;
    void* ptrEnd;
	uint32_t size;
	// Increment for every defragmentation run.
	uint32_t defragIndex;

    HeapBlock *headBlock;
    HeapBlock *tailBlock;
	
	// Stats really
	uint32_t usedSpace;
	uint32_t freeSpace;
};
