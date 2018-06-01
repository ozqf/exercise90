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

#include "com_heap_types.h"

static inline void Heap_Init(Heap *heap, void *allocatedMemory, uint32_t allocatedSize);
static inline HeapBlock *Heap_FindBlock(Heap *heap, uint32_t blockId);
static inline HeapBlock *Heap_FindBlockByLabel(Heap *heap, char* label);
static inline MemoryBlock Heap_GetMemoryById(Heap* heap, u32 id);
static inline void* Heap_GetBlockMemoryAddress(Heap *heap, BlockRef *BlockRef);
static inline u32 Heap_CalcSpaceAfterBlock(Heap *heap, HeapBlock *block);
static inline void Heap_InsertBlock(Heap *heap, HeapBlock *block, HeapBlock *previous);
static inline void Heap_RemoveBlock(Heap *heap, const HeapBlock *block);
static inline bool Heap_RemoveBlockById(Heap *heap, i32 id);
static inline void Heap_Free(Heap *heap, u32 id);
static inline u32 Heap_Allocate(Heap *heap, BlockRef *bRef, uint32_t objectSize, char *label, u8 clearToZero);
static inline void Heap_InitBlockRef(Heap* heap, BlockRef* bRef, i32 blockId);
static inline void Heap_Purge(Heap* heap);
static inline i32 Heap_ScanForFragments(Heap *heap, Heap_Fragment* fragments, i32 arraySize);
static inline void Heap_Defrag2(Heap *heap);