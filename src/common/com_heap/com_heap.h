#pragma once

#include "../com_module.h"


#define BLOCK_HEADER_SIZE sizeof(HeapBlock)
#define HEAP_DEFAULT_ALIGNMENT 16
#define BLOCK_LABEL_SIZE 20

// Size of array used to track fragments during defragging
#define Z_HEAP_FRAGMENT_TALLY_MAX 256

#define HEAP_NO_STACK_INSERT 1

struct BlockRef;
union HeapBlock;
struct HeapBlockData;
union HeapBlock;
struct Heap_Fragment;
struct Heap;


void Heap_Init(Heap *heap, void *allocatedMemory, uint32_t allocatedSize);
HeapBlock *Heap_FindBlock(Heap *heap, uint32_t blockId);
HeapBlock *Heap_FindBlockByLabel(Heap *heap, char* label);
void* Heap_GetBlockMemoryAddress(Heap *heap, BlockRef *BlockRef);
u32 Heap_CalcSpaceAfterBlock(Heap *heap, HeapBlock *block);
void Heap_InsertBlock(Heap *heap, HeapBlock *block, HeapBlock *previous);
void Heap_RemoveBlock(Heap *heap, const HeapBlock *block);
bool Heap_RemoveBlockById(Heap *heap, i32 id);

void Heap_Free(Heap *heap, u32 id);
u32 Heap_Allocate(Heap *heap, BlockRef *bRef, uint32_t objectSize, char *label);
void Heap_InitBlockRef(Heap* heap, BlockRef* bRef, i32 blockId);
void Heap_Purge(Heap* heap);
i32 Heap_ScanForFragments(Heap *heap, Heap_Fragment* fragments, i32 arraySize);
void Heap_Defrag2(Heap *heap);