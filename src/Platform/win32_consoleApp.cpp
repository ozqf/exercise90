/*******************************************
 * Windows Console application main for 
 * testing purposes
 ******************************************/

#pragma once

#include <stdio.h>
#include "win32_system_include.h"

#include "../common/com_module.h"
#include "../tests/test_shared_utils.h"

void HeapTest()
{
    void *ptrSystemMemory;
    Heap heap;

    printf("\n** HEAP TESTS **\n");
    u32 alignedSize = Com_AlignSize(52, 32);
    printf("32 Aligned size of 52 is %d\n", alignedSize);

    alignedSize = Com_AlignSize(673, 32);
    printf("32 Aligned size of 673 is %d\n", alignedSize);

    u32 sizeOfBlock = sizeof(HeapBlock);

    printf("Size of HeapBlock: %d\n", sizeOfBlock);

    u32 systemBytes = 1024;
    ptrSystemMemory = VirtualAlloc(
        0,
        systemBytes,
        MEM_RESERVE | MEM_COMMIT,
        PAGE_READWRITE);
    //ptrSystemMemory = malloc(systemBytes);
    Heap_Init(&heap, ptrSystemMemory, systemBytes);
    //printf("Allocated %d bytes\n", systemBytes);

    BlockRef ref = {};
    Heap_Allocate(&heap, &ref, 52, "B1 52");

    BlockRef ref2 = {};
    u32 stringBlockId = Heap_Allocate(&heap, &ref2, 128, "B2 128");
	Com_CopyString("The quick brown fox jumped over the lazy dogs.", (char*)ref2.ptrMemory);

    BlockRef stringRef = {};
    Heap_InitBlockRef(&heap, &stringRef, stringBlockId);
    char* stringMem = (char*)Heap_GetBlockMemoryAddress(&heap, &stringRef);
    printf("\nContents of block %d: %s\n", stringRef.id, stringMem);

    // This alloc should fail!
    //BlockRef ref3 = {};
    //Heap_Allocate(&heap, &ref3, 2147, "Test 3");

    BlockRef ref4 = {};
	Heap_Allocate(&heap, &ref4, 14, "B3 14 - This label is far to long and should cause an overflow");
    u32 *ptr = (u32 *)ref4.ptrMemory;
    u32 val = (u32)(*ptr);
    //printf("Mem at %d: %d\n", (u32)ref4.ptrMemory, val);

    Heap_DebugPrintAllocations(&heap);

    printf("Test block remove\n");
    Heap_RemoveBlockById(&heap, 2);
    Heap_RemoveBlockById(&heap, 1);
    Heap_DebugPrintAllocations(&heap);
    BlockRef ref5 = {};
    Heap_Allocate(&heap, &ref5, 500, "Test 5");

    Heap_DebugPrintAllocations(&heap);

    printf("\nPURGE HEAP\n");
    Heap_Purge(&heap);
    Heap_DebugPrintAllocations(&heap);

    VirtualFree(ptrSystemMemory, 0, MEM_RELEASE);
    //printf("Mem at %d: %d\n", (u32)ref4.ptrMemory, (u32)(*ptr));
}

void HeapTest2()
{
    
}

// main function for everywhere except windows
int main(i32 argc, char* argv[])
{
    printf("Built on %s at %s, file: %s, line: %d\n", __DATE__, __TIME__, __FILE__, __LINE__);
    
    //HeapTest();
    Test_Com_Run();
    
    printf("End - Success");
    return 0;
}

// Windows 'Main'
// int CALLBACK WinMain(
//     HINSTANCE hInstance,
//     HINSTANCE hPrevInstance,
//     LPSTR lpCmdLine,
//     int nCmdShow) { }
