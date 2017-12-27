#pragma once

#include <stdio.h>
#include <windows.h>

#include "../Shared/shared.h"
#include "../Shared/Heap.cpp"
#include "../tests/test_shared_utils.h"

void *ptrSystemMemory;
Heap heap;
HeapBlock *block;
HeapRef file;

void HeapTest()
{
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

    HeapRef ref = {};
    Heap_Allocate(&heap, &ref, 52, "Test 1");

    HeapRef ref2 = {};
    Heap_Allocate(&heap, &ref2, 673, "Test 2");

    // This alloc should fail!
    //HeapRef ref3 = {};
    //Heap_Allocate(&heap, &ref3, 2147, "Test 3");

    HeapRef ref4 = {};
    Heap_Allocate(&heap, &ref4, 14, "Test 4");
    u32 *ptr = (u32 *)ref4.ptrMemory;
    u32 val = (u32)(*ptr);
    //printf("Mem at %d: %d\n", (u32)ref4.ptrMemory, val);

    Heap_DebugPrintAllocations(&heap);

    printf("Test block remove\n");
    Heap_RemoveBlockById(&heap, 2);
    Heap_RemoveBlockById(&heap, 1);
    Heap_DebugPrintAllocations(&heap);
    HeapRef ref5 = {};
    Heap_Allocate(&heap, &ref5, 500, "Test 5");

    Heap_DebugPrintAllocations(&heap);

    printf("\nPURGE HEAP\n");
    Heap_Purge(&heap);
    Heap_DebugPrintAllocations(&heap);

    //printf("Mem at %d: %d\n", (u32)ref4.ptrMemory, (u32)(*ptr));
}

// main function for everywhere except windows
int main(i32 argc, char* argv[])
{
    printf("Built on %s at %s, line %d\n", __DATE__, __TIME__, __LINE__);
    
    HeapTest();
    //Test_Com_Run();
    
    printf("End - Success");
    return 0;
}

// Windows 'Main'
// int CALLBACK WinMain(
//     HINSTANCE hInstance,
//     HINSTANCE hPrevInstance,
//     LPSTR lpCmdLine,
//     int nCmdShow) { }
