#pragma once

#include <stdio.h>

#include "../com_module.h"

#include "com_heap_types.h"

/* Reading Material

https://en.wikipedia.org/wiki/Scratch_space
https://www.reddit.com/r/programming/comments/149hpj/why_custom_allocatorspools_are_hard/
http://yosefk.com/blog/why-custom-allocatorspools-are-hard.html
http://www.dice.se/wp-content/uploads/2014/12/scopestacks_public.pdf / http://www.dice.se/news/scope-stack-allocation/
https://gist.github.com/jhaberstro/664938
https://www.gamasutra.com/blogs/MichaelKissner/20151104/258271/Writing_a_Game_Engine_from_Scratch__Part_2_Memory.php

*/

/**
So here's a first go at some kind of Heap
Steps to figure out for implementation:
DONE > Allocate a block;
DONE > Allocate a stack of blocks (create block linked list)
DONE > Retrieve a block
DONE > Free a block (resew the linked list)
> Defrag the block list

Working plan:
> The Heap: A block of memory malloc'd externally and assigned to a Heap struct
> HeapBlock: A block of memory on the heap
> BlockRef: A Reference to a block on the heap. This is what external users should
use to access memory on the Heap, as it will hold an up to date pointer
> Blocks are a 32 byte header + the databehind them

0 ------------------------------------------------------------------ Size Of Heap
Header->Data|Header->Data|Header->Data|Header->Data

Data is padded to align to size of Header

current block address + block size = linked list:
List Head                                    List Tail
Header----->|Header----->|Header----->|Header
--------Data|--------Data|--------Data|--------Data

Allocation involves:
> space required = size of allocation + 32
> walk block list until end. Jump to the end of the last block's size
  > Later improvement after free is added: Walk until a big enough gap is found OR the list ends
> Once at the new address error if no space is available
> Create the HeapBlock header.
> Link the HeapBlock header to the previous block, or assign it as the head otherwise.
> Create a reference to the new block, assign id and defrag index
> returning the reference

Deallocation involves:
> Find the block in question by walking the list
> Find the next and previous blocks change prev.next to next, thus removing the middle 
> For debugging: Zero the memory inbetween.

*/

inline static void Heap_Init(Heap *heap, void *allocatedMemory, uint32_t allocatedSize)
{
    // Address range
    heap->ptrMemory = allocatedMemory;
    heap->ptrEnd = (u8 *)heap->ptrMemory + allocatedSize;
    heap->size = allocatedSize;
    heap->alignmentBytes = HEAP_DEFAULT_ALIGNMENT;

    // ids for tracking
    heap->nextID = 1;
    heap->iteration = 1;

    // Allocated Block Linked list
    heap->headBlock = NULL;
    heap->tailBlock = NULL;

    // necessary?
    heap->usedSpace = 0;
    heap->freeSpace = allocatedSize;

    u32 lastAddress = (u32)allocatedMemory + allocatedSize;
    // printf("Heap init with %d bytes. Address range: %d to %d\n",
    //        allocatedSize, (u32)allocatedMemory, lastAddress);
}

HeapBlock *Heap_FindBlock(Heap *heap, uint32_t blockId)
{
    HeapBlock *block = heap->headBlock;
    while (block)
    {
        if (block->mem.id == blockId)
        {
            return block;
        }
        block = block->mem.next;
    }
    return NULL;
}

HeapBlock *Heap_FindBlockByLabel(Heap *heap, char* label)
{
    HeapBlock *block = heap->headBlock;
    while (block)
    {
        if (COM_CompareStrings(block->mem.debugLabel, label) == 0) { return block; }
        block = block->mem.next;
    }
    return NULL;
}

inline static MemoryBlock Heap_GetMemoryById(Heap* heap, u32 id)
{
    HeapBlock* block = Heap_FindBlock(heap, id);
    Assert(block != NULL);

    MemoryBlock result = {};
    result.ptrMemory = block->mem.ptrMemory;
    result.size = block->mem.objectSize;
    return result;
}

// Find the data pointer for this BlockRef for use. Update if dirty
inline static void* Heap_GetBlockMemoryAddress(Heap *heap, BlockRef *BlockRef)
{
    // if defrag indexes match, ref is okay
    if (BlockRef->iteration == heap->iteration)
    {
        return BlockRef->ptrMemory;
    }

    // Mismatch. Update the Reference address
    HeapBlock* block = Heap_FindBlock(heap, BlockRef->id);
    Assert(block != NULL);
    void* newAddress = block->mem.ptrMemory;

    if (newAddress == NULL)
    {
        // Either the address was invalid or the block was not found
        // Either way, Critical error, Stop Program
        // Crash
        return NULL;
    }
    BlockRef->ptrMemory = newAddress;
    BlockRef->iteration = heap->iteration;
    return newAddress;
}

inline MemoryBlock Heap_RefToMemoryBlock(Heap* heap, BlockRef* ref)
{
    MemoryBlock mem = {};
    Heap_GetBlockMemoryAddress(heap, ref);
    mem.ptrMemory = ref->ptrMemory;
    mem.size = ref->objectSize;
    return mem;
}

inline ByteBuffer Heap_RefToByteBuffer(Heap* heap, BlockRef* ref)
{
    ByteBuffer buf = {};
    Heap_GetBlockMemoryAddress(heap, ref);
    buf.ptrStart = (u8*)ref->ptrMemory;
    buf.ptrWrite = (u8*)ref->ptrMemory;
	buf.ptrEnd = buf.ptrStart;// (u8*)ref->ptrMemory + ref->objectSize;
    buf.capacity = ref->objectSize;
    return buf;
}

static u32 Heap_CalcSpaceAfterBlock(Heap *heap, HeapBlock *block)
{
    u32 result;
    HeapBlock *next = block->mem.next;
    if (next != NULL)
    {
        // calculate space between end of this block and the start of the next
        u32 distToNext = (u32)next->mem.ptrMemory - (u32)block->mem.ptrMemory;
        result = distToNext - (BLOCK_HEADER_SIZE + block->mem.volumeSize);

// #if VERBOSE
//         printf(" Space between block %d and block %d: %d\n",
//                block->mem.id, next->mem.id, result);
// #endif
    }
    else
    {
        // No block after this one. result = end of heap - end of block volume
        result = ((u32)heap->ptrEnd - (u32)block) - (BLOCK_HEADER_SIZE + block->mem.volumeSize);

// #if VERBOSE
//         printf(" Space between block %d and Heap end: %d\n",
//                block->mem.id, result);
// #endif
    }
    return result;
}

#if 1
static void Heap_DebugPrintAllocations2(Heap *heap)
{

    printf("\n** HEAP PRINT **\n");
    u32 startAddress = (u32)heap->ptrMemory;
    u32 fragments[Z_HEAP_FRAGMENT_TALLY_MAX];
    fragments[0] = 0;

    u32 fragmentIndex = 0;
    HeapBlock *heapBlock = heap->headBlock;
    if (heapBlock == NULL)
    {
        printf("Heap is empty. Size: %d\n", heap->size);
        return;
    }

    u32 spaceBeforeHead = (u32)heapBlock - (u32)heap->ptrMemory;
    printf("Space before Block head: %d.\n", spaceBeforeHead);
    u32 totalSpace = 0;
    if (totalSpace > 0) { fragments[fragmentIndex++] = totalSpace; }

    while (heapBlock != NULL)
    {
        u32 space = Heap_CalcSpaceAfterBlock(heap, heapBlock);
        u32 blockVolume = heapBlock->mem.volumeSize + BLOCK_HEADER_SIZE;
        totalSpace += blockVolume;
        if (space > 0 && fragmentIndex < Z_HEAP_FRAGMENT_TALLY_MAX)
        {
            fragments[fragmentIndex++] = space;
        }
        u32 address = (u32)heapBlock - startAddress;

        printf("%d: BLK %d '%s' Size: %d VolSize: %d. Gap %d\n",
               address,
               heapBlock->mem.id,
               heapBlock->mem.debugLabel,
               heapBlock->mem.objectSize,
               blockVolume,
               space);

        heapBlock = heapBlock->mem.next;

    }
    printf("Fragments: ");
    for (u32 i = 0; i < fragmentIndex; ++i)
    {
        printf("%d, ", fragments[i]);
    }

    f32 percent = ((f32)totalSpace / (f32)heap->size) * 100.0f;

    printf("\nUsing: %dKB of %dKB (%.2f%%)\n", (u32)(totalSpace / 1024), (u32)(heap->size / 1024), percent);
}
#endif

/*****************************
 * LINKED LIST OPERATIONS
 ****************************/

inline static void Heap_InsertBlock(Heap *heap, HeapBlock *block, HeapBlock *previous)
{
    if (previous == NULL)
    {
        heap->headBlock = block;
        heap->tailBlock = block;
        block->mem.next = NULL;
        block->mem.prev = NULL;
        return;
    }
    HeapBlock *next = previous->mem.next;
    if (next != NULL)
    {
        // Insert between
        block->mem.next = next;
        block->mem.prev = previous;
        next->mem.prev = block;
        previous->mem.next = block;
    }
    else
    {
        // New Tail
        previous->mem.next = block;
        block->mem.prev = previous;
        heap->tailBlock = block;
    }
}

inline static void Heap_RemoveBlock(Heap *heap, const HeapBlock *block)
{
    HeapBlock *next = block->mem.next;
    HeapBlock *prev = block->mem.prev;
    #if PARANOID
    // Zero out space
    u32 startAddress = (u32)block;
    u32 numBytes = BLOCK_HEADER_SIZE + block->mem.volumeSize;
    COM_ZeroMemory((u8*)startAddress, numBytes);
    #endif
    // unsew list
    if (prev != NULL)
    {
        prev->mem.next = next;
    }
    else
    {
        heap->headBlock = next;
    }
    if (next != NULL)
    {
        next->mem.prev = prev;
    }
    else
    {
        heap->tailBlock = prev;
    }
}

bool Heap_RemoveBlockById(Heap *heap, i32 id)
{
    HeapBlock *block = Heap_FindBlock(heap, id);
    if (block != NULL)
    {
        Heap_RemoveBlock(heap, block);
        return true;
    }
    else
    {
        return false;
    }
}

#define HEAP_NO_STACK_INSERT 1

/*****************************
 * ALLOC/FREE
 ****************************/
inline static void Heap_Free(Heap *heap, u32 id)
{
	HeapBlock* block = Heap_FindBlock(heap, id);
	Assert(block != NULL);
    Heap_RemoveBlock(heap, block);
}

/**
 * Create a new HeapBlock - returns id of new allocation, and fills out the provided BlockRef
 * > provided BlockRef can be NULL.
 * > Label will be clapped to maximum space in a block label
 * > Will crash if no space of suffient size is available
 */
u32 Heap_Allocate(Heap *heap, BlockRef *bRef, uint32_t objectSize, char *label, u8 clearToZero)
{
    Assert(heap != NULL);
// #if VERBOSE
//     printf("> HEAP ALLOC %d bytes\n", objectSize);
// #endif
    u32 volumeSize = COM_AlignSize(objectSize, heap->alignmentBytes);
    HeapBlock *newBlock = NULL;
    if (heap->headBlock == NULL)
    {
        // No blocks are in the heap. Create first
        u32 heapSpace = (u32)heap->ptrEnd - (u32)heap->ptrMemory;
// #if VERBOSE
//         printf("  First alloc Heap space: %d vs volume size: %d\n", heapSpace, volumeSize);
// #endif
//         printf("Vol Size (%d) < Heap Space (%d)?\n", volumeSize, heapSpace);
        AssertAlways(volumeSize <= heapSpace);

        newBlock = (HeapBlock *)heap->ptrMemory;
        *newBlock = {};
        Heap_InsertBlock(heap, newBlock, NULL);

// #if VERBOSE
//         printf("  Allocating first block, no jump\n");
// #endif
    }
    else
    {
        u32 space = 0;
        HeapBlock *iterator = heap->headBlock;
        // measure space between start and first block.
        space = (u32)iterator - (u32)heap->ptrMemory;
        if (volumeSize <= space)
        {
            // allocate and replace list head
            newBlock = (HeapBlock*)heap->ptrMemory;
            *newBlock = {};
            heap->headBlock = newBlock;
            newBlock->mem.next = iterator;
            iterator->mem.prev = newBlock;
        }
        else
        {
            // Look for spaces between blocks
            u32 count = 1;
            while (iterator != NULL)
            {
                if (count > 9999)
                {
                    //printf("Block scan ran away... aborting");
                    AssertAlways(false);
                    break;
                }
                space = Heap_CalcSpaceAfterBlock(heap, iterator);
                if (space >= volumeSize)
                {
                    // assign block
                    u32 jumpDistance = BLOCK_HEADER_SIZE + iterator->mem.volumeSize;
                    newBlock = (HeapBlock *)((u8 *)iterator + jumpDistance);
                    *newBlock = {};
                    // Sew list
                    Heap_InsertBlock(heap, newBlock, iterator);

                    iterator->mem.next = newBlock;
// #if VERBOSE
//                     printf("  Address at block %d: %d. Volume size: %d. Jumping %d\n",
//                            count, (u32)iterator, iterator->mem.volumeSize, jumpDistance);
// #endif
                    break;
                }
                else
                {
// #if VERBOSE
//                     printf("  No space at block %d\n", count);
// #endif
                    iterator = iterator->mem.next;
                    // If space check failed and no more blocks are available
                    // then the heap is full... realloc or crash
                    AssertAlways(iterator != NULL);
                }
                count++;
            }
        }
    }

    // New block has been placed, initialise
    newBlock->mem.id = heap->nextID;
    newBlock->mem.objectSize = objectSize;
    newBlock->mem.volumeSize = volumeSize;
    newBlock->mem.ptrMemory = (void *)((u8 *)newBlock + BLOCK_HEADER_SIZE);
    COM_CopyStringLimited(label, newBlock->mem.debugLabel, BLOCK_LABEL_SIZE);

    heap->nextID += 1;
	if (clearToZero)
	{
		COM_SetMemory((u8*)newBlock->mem.ptrMemory, volumeSize, 0X00);
	}
    
// #if VERBOSE
//     printf("    NEW BLOCK %d - BLOCK ADDRESS: %d, MEM ADDRESS: %d, OBJECT SIZE: %d VOLUME SIZE: %d\n",
//            heap->nextID, (i32)newBlock, (u32)newBlock->mem.ptrMemory, objectSize, newBlock->mem.volumeSize);
// #endif

    if (bRef != NULL)
    {
        bRef->iteration = heap->iteration;
        bRef->id = newBlock->mem.id;
        bRef->ptrMemory = newBlock->mem.ptrMemory;
        bRef->objectSize = objectSize;
    }
    
    return newBlock->mem.id;
}

inline static void Heap_InitBlockRef(Heap* heap, BlockRef* bRef, i32 blockId)
{
    Assert(bRef != NULL);
    HeapBlock* block = Heap_FindBlock(heap, blockId);
    Assert(block != NULL);
    if (block == NULL)
    {
        return;
    }
    bRef->iteration = heap->iteration;
    bRef->id = block->mem.id;
    bRef->ptrMemory = block->mem.ptrMemory;
    bRef->objectSize = block->mem.objectSize;
}

inline static void Heap_Purge(Heap* heap)
{
    HeapBlock* next;
    HeapBlock* block = heap->headBlock;
    while (block != NULL)
    {
        next = block->mem.next;
        
        Heap_RemoveBlock(heap, block);
        block = next;
    }
}

/*
            0 ------------------------------------------------------------------ Size Of Heap
Fragmented: ------------|Header->Data|------------|Header->Data-----------------
Copy 0      <-----------
Copy 1                  <-------------------------
Result:     Header->Data|Header->Data|------------------------------------------

example:    < Distance > < numBytes >
            <-----------|Header->Data|
            ^           ^ originOffset
            | destinationOffset


*/

/**
 * fill the given array with fragments
 * Returns the number of fragments found
 */
i32 Heap_ScanForFragments(Heap *heap, Heap_Fragment* fragments, i32 arraySize)
{
    i32 fragIndex = 0;
    // space before headblock, different between block address and heap memory address
    HeapBlock* block = heap->headBlock;
    u32 space = (u32)block - (u32)heap->ptrMemory;
    
    //i32 numBytes;
    if (space > 0)
    {
        
        ++fragIndex;
    }

    // while (block != NULL && fragIndex < arraySize)
    // {
    //     // Space after last block is not a fragment!
    //     if (block->mem.next == NULL)
    //     {
    //         break;
    //     }

    //     space = Heap_CalcSpaceAfterBlock(heap, block);
    //     if (space > 0)
    //     {
    //         fragments[fragIndex].originOffset = (u32)block - (u32)heap->ptrMemory;
    //         fragments[fragIndex].distance = 0;
            
    //     }

    //     block = block->mem.next;
    // }

    return fragIndex;
}

inline static void Heap_Defrag2(Heap *heap)
{
    //heap->iteration++;
    /*
    - Naive Defrag -
    > Scan for first gap, then count blocks to the next gap
    > Shift block range to close gap
    > Repeat until space is only after last block
    */
    // char strArray[32];
    // char* str = strArray;
    // str = "boo";
    // printf("%s\n", str);

    // Heap_Fragment fragments[1];
    // i32 numFragments = Heap_ScanForFragments(heap, fragments, 1);
}

inline static i32 Heap_AllocString(Heap* heap, BlockRef* ref, i32 maxStringCapacity)
{
    // length of string + 1 for null terminator
    u32 totalSize = maxStringCapacity + 1;
    return Heap_Allocate(heap, ref, totalSize, "ZString", 1);
}

inline void Heap_NeverCall()
{
    ILLEGAL_CODE_PATH
    Heap_DebugPrintAllocations2(NULL);
}
