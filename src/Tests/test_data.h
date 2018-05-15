#pragma once

#include "../common/com_module.h"

#include "tinydir.h"

void Print_Directory(tinydir_dir dir)
{
    while (dir.has_next)
    {
        tinydir_file file;
        tinydir_readfile(&dir, &file);

        printf("%s", file.name);
        if (file.is_dir)
        {
            printf("/");
            if (*file.name != '.')
            {
                tinydir_dir childDir;
                printf("Open %s\n", file.name);
                tinydir_open(&childDir, file.name);
                Print_Directory(childDir);
            }
            
        }
        printf("\n");

        tinydir_next(&dir);
    }

    tinydir_close(&dir);
}

void Print_DirectoryTree()
{
    printf("\n******************** Print Tree ********************\n");
    tinydir_dir dir;
    tinydir_open(&dir, "./");
    Print_Directory(dir);
    tinydir_close(&dir);
}

void Print_SingleDirectory()
{
    printf("\n******************** Print Directory ********************\n");
    tinydir_dir dir;
    tinydir_open(&dir, "./");

    while (dir.has_next)
    {
        tinydir_file file;
        tinydir_readfile(&dir, &file);

        printf("%s", file.name);
        if (file.is_dir)
        {
            printf("/");
        }
        printf("\n");

        tinydir_next(&dir);
    }

    tinydir_close(&dir);
}


void TestDirectoryScan()
{
	Print_SingleDirectory();
	Print_DirectoryTree();
}

////////////////////////////////////////////////////////////////////
// Test custom archive read
////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////
// In Data file
#pragma pack(1)
struct DataFileDiskHeader
{
	u8 magic[4];
	u32 fileListOffset;
	u32 numFiles;
};

// 64 bytes
#pragma pack(1)
struct DataFileDiskEntry
{
	u8 fileName[52];    // 51 for name one for null terminator
	u32 offset;         // position offset from start of file
	u32 size;           // size in bytes
    u8 info[4];			// file information [0] = file type [1] = compression type
};

////////////////////////////////////////////////////
// In Heap

// Permanently open link between the file and the manifest in the heap
struct DataFileHandle
{
    FILE* file;
    u32 index;
    u32 fileListOffset;
    u32 numFiles;
    i32 blockId;
    void* ptrManifest;
};

// File manifest item stored in heap
struct DataFileItem
{
    DataFileDiskEntry file;     // manifest item in disk file
    i32 fileHandleId;           // id in array of DataFileHandles
    i32 status;                 // 0 == not loaded, 1 == loading, 2 == loaded

    i32 blockId;                // id of heap block this object is stored at
    u32 sizeInHeap;             // decompressed size in RAM
};

DataFileHandle g_dataHandles[16];
internal u32 g_nextDataHandle = 0;

DataFileHandle* Test_ReadPackManifest(Heap* h, BlockRef* manifestRef, char* filePath)
{
	printf("Test Read custom archive \"data01.dat\"\n");
	printf("Size of archive header: %d, size of manifest entry: %d\n", sizeof(DataFileDiskHeader), sizeof(DataFileDiskEntry));
	FILE* f;
	fopen_s(&f, "./data01.dat", "rb");
	i32 end;

	fseek(f, 0, SEEK_END);
	end = ftell(f);
	fseek(f, 0, SEEK_SET);

	printf("File size: %d bytes\n", end);

	DataFileDiskHeader header = {};
	fread(&header, sizeof(DataFileDiskHeader), 1, f);
	printf("Read magic: %c, %c, %c, %c\n", header.magic[0], header.magic[1], header.magic[2], header.magic[3]);
	if (header.magic[0] != 'P' || header.magic[1] != 'A' || header.magic[2] != 'C' || header.magic[3] != 'K')
	{
		printf("Magic not recognised as 'P A C K', aborted\n");
		fclose(f);
		return {};
	}

    // Find an empty header
    DataFileHandle* handle = &g_dataHandles[g_nextDataHandle];
    handle->index = g_nextDataHandle;
    g_nextDataHandle++;
    *handle = {};
    handle->file = f;
    handle->fileListOffset = header.fileListOffset;
    handle->numFiles = header.numFiles;
	
	printf("File is an archive with %d files, handle index %d. File Manifest offset: %d\n", handle->numFiles, handle->index, handle->fileListOffset);

    // malloc temporary space for disk manifest
    DataFileDiskEntry* files = (DataFileDiskEntry*)malloc(header.numFiles * sizeof(DataFileDiskEntry));
    //DataFileDiskEntry* files = (DataFileDiskEntry*)malloc(sizeof(DataFileDiskEntry) * header.numFiles);
    //BlockRef ref = {};

    // allocate heap manifest
    Heap_Allocate(h, manifestRef, header.numFiles * sizeof(DataFileItem), "./data01.dat");
	DataFileItem* items = (DataFileItem*)Heap_GetBlockMemoryAddress(h, manifestRef);
    printf("Allocated %d bytes for Manifest\n", sizeof(DataFileItem) * header.numFiles);

	fseek(f, header.fileListOffset, SEEK_SET);

	for (u32 i = 0; i < header.numFiles; ++i)
	{
		fread(&files[i], sizeof(DataFileDiskEntry), header.numFiles, f);
        // Clear entire struct to 0!
        items[i] = {};
        items[i].file = files[i];
        items[i].fileHandleId = handle->index;
		printf("%d, ", + i);
	}
    printf("\nDone\n");

    handle->ptrManifest = manifestRef->ptrMemory;
    handle->blockId = manifestRef->id;

    //Heap_Free(h, ref.id);
    // Clean up temporary space
	free((void*)files);

    return handle;
}

void Test_PrintManifest(DataFileItem* entry, i32 numFiles)
{
    printf("Reading File Manifest\n");
    for (i32 i = 0; i < numFiles; ++i)
    {
        printf("File %d Item (%d): %s. Type: %d, size: %d, location: %d\n", entry->fileHandleId, i, entry->file.fileName, entry->file.info[0], entry->file.size, entry->file.offset);
        entry++;
    }
}

DataFileItem* Test_FindDataFileItem(Heap* h, char* filePath)
{
    DataFileHandle*  handle = g_dataHandles;
    // iterate file handles
    for (u32 i = 0; i < g_nextDataHandle; ++i)
    {
        MemoryBlock b = Heap_GetMemoryById(h, handle[i].blockId);
        DataFileItem* item = (DataFileItem*)b.ptrMemory;
        i32 numItems = g_dataHandles[i].numFiles;
        for (int j = 0; j < numItems; ++j)
        {
            if (COM_CompareStrings((char*)&item[j].file.fileName, filePath) == 0)
            {
                return &item[j];
            }
        }
    }
    return NULL;
}

void Test_FindFileTest(Heap* h, char* fileName)
{
    DataFileItem* item = Test_FindDataFileItem(h, fileName);
    if (item == NULL)
    {
        printf("Failed to find test item\n");
    }
    else
    {
        printf("Found %s in Data file %d, at %d. Status: %d\n", item->file.fileName, item->fileHandleId, item->file.offset, item->status);
    }
}

Heap g_heap = {};

void Test_Pack()
{
    //TestDirectoryScan();

    // Create main heap
    Heap_Init(&g_heap, malloc(MegaBytes(16)), MegaBytes(16));
    // Load file manifest into references
    BlockRef r = {};
	DataFileHandle* dataHandle = Test_ReadPackManifest(&g_heap, &r, "./data01.dat");
    
    Test_PrintManifest((DataFileItem*)dataHandle->ptrManifest, dataHandle->numFiles);

    printf("\n** Search tests **\n");
    Test_FindFileTest(&g_heap, "\\BAL1A0.bmp");
    Test_FindFileTest(&g_heap, "\\brbrick2.bmp");
}
