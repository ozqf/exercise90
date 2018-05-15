#pragma once

#include "../common/com_module.h"
#include <stdio.h>

/*
Disk
    Data01
    Data02
    
DiskFileHandles

Heap
    DataManifest
        list of all items in disk data files and their status. If they are loaded into the heap
        their heap block id is stored here.
    
    File1
    File2
    File3

*/

struct DataArchiveHandle
{
    FILE* handle;
   // header
   u32 fileListOffset;
   u32 numFiles;
};

struct DataFileItem
{
    u32 status;                 // 0 == not loaded, 1 == loaded
    // In RAM info
    i32 blockId;                // id of heap block this object is stored at
    i32 sizeInHeap;             // decompressed size in RAM
    
    // On Disk info
    i32 fileId;                 // id in array of DataFileHandles
    u32 positionInFile;         // position in file of data
    u32 sizeInFile;             // amount of data in asset file (may be compressed)
};


internal DataArchiveHandle dataFiles[256];  // OS handles to external data archives. Opened on load and only closed at shutdown/when app changes.
internal DataFileItem* manifest;            // start of array of file manifests. Each file has a manifest entry here
internal i32 numManifestItems;              // sum of file counts in all data items.

void Data_CopyFromDataToHeap(Heap* h, BlockRef* ref, char* filName)
{
   /*
   > Find Manifest item for file. If not found, error!
   If status is 'ready'
       > locate and return BlockRef based on blockId in manifest item
       Done
   Else If status is 'loading'
       Multithreading only:
       > wait for load to finish then return BlockRef?
   Else
       > Set manifest status to loading
   If file is compressed:
       > Malloc temp space and perform decompression
       > Copy from temp space to heap
       > Free temp space
   Else
       > Alloc space in Heap, and copy file to the heap.
   > Set block ref id on manifest.
   > return BlockRef to caller.
   Done
   */
}

void Data_DeleteItemFromHeap()
{
   /*
   > Find manifest item for file (could be iterated over if unloading)
   > Set status to "Not Loaded"
   > Clear BlockRef
   > Delete HeapBlock
   */
 
}
