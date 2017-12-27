#pragma once

#include <stdint.h>
#include <stdio.h>
#include "Heap.h"
#include "shared.h"

#define BLOCK_HEADER_SIZE sizeof(HeapBlock)
#define HEAP_ALIGNMENT 32

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
> Allocate a block
> Allocate a stack of blocks (create block linked list)
> Retrieve a block
> Free a block (resew the linked list)

Working plan:
> The Heap: A block of memory malloc'd externally and assigned to a Heap struct
> HeapBlock: A block of memory on the heap
> HeapRef: A Reference to a block on the heap
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

void Heap_Init(Heap *heap, void *allocatedMemory, uint32_t allocatedSize)
{
    // Address range
    heap->ptrMemory = allocatedMemory;
    heap->ptrEnd = (u8 *)heap->ptrMemory + allocatedSize;
    heap->size = allocatedSize;

    // ids for tracking
    heap->nextID = 1;
    heap->defragIndex = 1;

    // Allocated Block Linked list
    heap->headBlock = NULL;
    heap->tailBlock = NULL;

    // necessary?
    heap->usedSpace = 0;
    heap->freeSpace = allocatedSize;

    u32 lastAddress = (u32)allocatedMemory + allocatedSize;
    printf("Heap init with %d bytes. Address range: %d to %d\n",
           allocatedSize, (u32)allocatedMemory, lastAddress);
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

// Find the data pointer for this heapRef for use. Update if dirty
void *Heap_GetBlockMemoryAddress(Heap *heap, HeapRef *heapRef)
{
    // if defrag indexes match, ref is okay
    if (heapRef->defragIndex == heap->defragIndex)
    {
        return heapRef->ptrMemory;
    }

    // Mismatch. Update the Reference address
    void *newAddress = Heap_FindBlock(heap, heapRef->id);

    if (newAddress == NULL)
    {
        // Either the address was invalid or the block was not found
        // Either way, Critical error, Stop Program
        // Crash
        return NULL;
    }
    heapRef->ptrMemory = newAddress;
    heapRef->defragIndex = heap->defragIndex;
    return newAddress;
}

u32 Heap_CalcSpaceAfterBlock(Heap *heap, HeapBlock *block)
{
    u32 result;
    HeapBlock *next = block->mem.next;
    if (next != NULL)
    {
        // calculate space between end of this block and the start of the next
        u32 distToNext = (u32)next->mem.ptrMemory - (u32)block->mem.ptrMemory;
        result = distToNext - (BLOCK_HEADER_SIZE + block->mem.volumeSize);

#if VERBOSE
        printf(" Space between block %d and block %d: %d\n",
               block->mem.id, next->mem.id, result);
#endif
    }
    else
    {
        // No block after this one. result = end of heap - end of block volume
        result = ((u32)heap->ptrEnd - (u32)block) - (BLOCK_HEADER_SIZE + block->mem.volumeSize);

#if VERBOSE
        printf(" Space between block %d and Heap end: %d\n",
               block->mem.id, result);
#endif
    }
    return result;
}

/*****************************
 * LINKED LIST OPERATIONS
 ****************************/

void Heap_DebugPrintAllocations(Heap *heap)
{
    printf("\n** HEAP PRINT **\n");
    HeapBlock *block = heap->headBlock;
    u32 usedSpace = 0;
    while (block != NULL)
    {
        u32 space = Heap_CalcSpaceAfterBlock(heap, block);
        usedSpace += block->mem.volumeSize + BLOCK_HEADER_SIZE;
        printf("Block %d '%s'. Size: %d. Space after: %d\n",
               block->mem.id, block->mem.debugLabel, block->mem.volumeSize, space);
        block = block->mem.next;
    }
    printf("Used: %d\n", usedSpace);
}

void Heap_InsertBlock(Heap *heap, HeapBlock *block, HeapBlock *previous)
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

void Heap_RemoveBlock(Heap *heap, const HeapBlock *block)
{
    HeapBlock *next = block->mem.next;
    HeapBlock *prev = block->mem.prev;
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
void Heap_FreeBlock(Heap *heap, HeapBlock *block)
{
    Heap_RemoveBlock(heap, block);
}

void Heap_Allocate(Heap *heap, HeapRef *hRef, uint32_t objectSize, char *label)
{
#if VERBOSE
    printf("> HEAP ALLOC %d bytes\n", objectSize);
#endif
    u32 volumeSize = Com_AlignSize(objectSize, HEAP_ALIGNMENT);
    HeapBlock *newBlock = NULL;
    if (heap->headBlock == NULL)
    {
        // No blocks are in the heap. Create first
        u32 heapSpace = (u32)heap->ptrEnd - (u32)heap->ptrMemory;
#if VERBOSE
        printf("  First alloc Heap space: %d vs volume size: %d\n", heapSpace, volumeSize);
#endif
        printf("Vol Size (%d) < Heap Space (%d)?\n", volumeSize, heapSpace);
        AssertAlways(volumeSize <= heapSpace);

        newBlock = (HeapBlock *)heap->ptrMemory;
        *newBlock = {};
        Heap_InsertBlock(heap, newBlock, NULL);
// heap->headBlock = newBlock;
// heap->tailBlock = newBlock;
#if VERBOSE
        printf("  Allocating first block, no jump\n");
#endif
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
                if (count > 10)
                {
                    printf("Block scan ran away... aborting");
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
#if VERBOSE
                    printf("  Address at block %d: %d. Volume size: %d. Jumping %d\n",
                           count, (u32)iterator, iterator->mem.volumeSize, jumpDistance);
#endif
                    break;
                }
                else
                {
#if VERBOSE
                    printf("  No space at block %d\n", count);
#endif
                    iterator = iterator->mem.next;
                    // If space check failed and no more blocks are available
                    // then the heap is full... realloc or crash
                    AssertAlways(iterator != NULL);
                }
                count++;
            }
        }
    }
    newBlock->mem.id = heap->nextID;
    newBlock->mem.objectSize = objectSize;
    newBlock->mem.volumeSize = volumeSize;
    newBlock->mem.ptrMemory = (void *)((u8 *)newBlock + BLOCK_HEADER_SIZE);
    Com_CopyString(label, newBlock->mem.debugLabel);

#if VERBOSE
    printf("    NEW BLOCK %d - BLOCK ADDRESS: %d, MEM ADDRESS: %d, OBJECT SIZE: %d VOLUME SIZE: %d\n",
           heap->nextID, (i32)newBlock, (u32)newBlock->mem.ptrMemory, objectSize, newBlock->mem.volumeSize);
#endif

    hRef->defragIndex = heap->defragIndex;
    hRef->id = newBlock->mem.id;
    hRef->ptrMemory = newBlock->mem.ptrMemory;
    hRef->size = objectSize;

    heap->nextID++;
}

void Heap_Defrag(Heap *heap)
{
    heap->defragIndex++;
    // Do defrag
}

void Heap_Purge(Heap* heap)
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
