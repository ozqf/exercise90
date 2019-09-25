#ifndef ZE_READ_BLENDER_FILE_H
#define ZE_READ_BLENDER_FILE_H

/*
https://raginggazebo.com/parsing-blender-3d-files-blend-1-of-3/

*/

#include "ze_std_utils.h"
#include "../common/common.h"

//////////////////////////////////////////////////////////
// Data types
//////////////////////////////////////////////////////////
struct ZEBlenderDataObj
{
    char asciCode[4];
    u32 size;
    // pointer size could be 4 or 8 bytes
    u64 ptr;
    u32 SDNAIndex;
    u32 numElements;
    u32 numBytes;
};

struct ZEBlenderFileDef
{
    u8* file;
    u32 fileSize;
    i32 pointerSize;
    i32 bLittleEndian;
    char version[4];
    u8* firstDataObj;
};

//////////////////////////////////////////////////////////
// Functions
//////////////////////////////////////////////////////////
static i32 ZE_ReadBlenderDataObjHeaders(
    i32 pointerSize, u8* mem, ZEBlenderDataObj** out_objects, u32* out_numObjects)
{
    // TODO: Make this resizeable... or scan for size before allocating array.
    // Even a simple object seemed to have 5k+ objects...
    const u32 maxObjects = 10000;
    u32 bytesForTable = sizeof(ZEBlenderDataObj) * maxObjects;
    printf("Allocating %dKB for file table\n", bytesForTable / 1024);
    ZEBlenderDataObj* objects = (ZEBlenderDataObj*)malloc(bytesForTable);
    u32 numObjects = 0;
    
    i32 bVerbose = YES;
    i32 numVerbose = 10;
    u8* cursor = mem;
	
	// stats
    i32 stat_numHeaders = 0;
	i32 stat_numDataItems = 0;
    for (;;)
    {
        if (numObjects == maxObjects)
        {
            printf("No capacity for object header\n");
            free(objects);
            return 1;
        }
        ZEBlenderDataObj* obj = &objects[numObjects];
        numObjects++;
        *obj = {};

        //if (bVerbose) { printf("Read data obj - Mem: %d\n", (u32)cursor); }
	    obj->asciCode[0] = *cursor++;
	    obj->asciCode[1] = *cursor++;
	    obj->asciCode[2] = *cursor++;
	    obj->asciCode[3] = *cursor++;
        if (strncmp(obj->asciCode, "ENDB", 4) == 0)
        {
            if (bVerbose) { printf("  EOF\n"); }
            break;
        }
		else if (strncmp(obj->asciCode, "DATA", 4) == 0)
		{
			stat_numDataItems++;
		}
        stat_numHeaders++;
        //u32 asciCode = *(u32*)cursor;
        //cursor += sizeof(u32);
        obj->size = *(u32*)cursor;
        cursor += sizeof(u32);
        
	    if (pointerSize == 4)
	    {
	    	obj->ptr = *(u32*)cursor;
	    	cursor += 4;
	    }
	    else if (pointerSize == 8)
	    {
	    	obj->ptr = *(u64*)cursor;
	    	cursor += 8;
	    }
	    else
	    {
	    	printf("Invalid ptr size %d aborted header read\n", pointerSize);
	    	return 1;
	    }
	    if (bVerbose) { printf("Block type \"%c%c%c%c\" - size %d Addr: %lld\n",
	    	obj->asciCode[0], obj->asciCode[1], obj->asciCode[2], obj->asciCode[3], obj->size, obj->ptr); }
    
	    obj->SDNAIndex = *(u32*)cursor;
	    cursor += sizeof(u32);
    
	    obj->numElements = *(u32*)cursor;
	    cursor += sizeof(u32);

	    if (bVerbose) { printf("SDNA index: %d num elements: %d\n", obj->SDNAIndex, obj->numElements); }
	    cursor += obj->size;
        numVerbose--;
        if (numVerbose == 0) { bVerbose = NO; }
    }
	//printf("Read %d data objects\n", numHeaders);]
	printf("Num DATA items: %d\n", stat_numDataItems);
    *out_objects = objects;
    *out_numObjects = numObjects;
	return 0;
}

static i32 ZE_ReadBlenderFile(char* path)
{
    printf("Reading blender file \"%s\"\n", path);
    u8* data;
    u32 dataSize;
    ZE_std_ReadEntireFileToHeap(path, &data, &dataSize);
    if (data == NULL || dataSize == 0)
    {
        printf("Failed to open blender file\n");
        return 1;
    }
    printf("  Read %dKB\n", dataSize / 1024);

    char* cursor = (char*)data;
    char* end = cursor + dataSize;

    // Check file magic number - strncmp as no null terminator
    if (strncmp("BLENDER", (const char*)cursor, 7) != 0)
    {
        printf("Not blender format. Abort\n");
        free(data);
        return 1;
    }

    ZEBlenderFileDef def = {};
    def.file = data;
    def.fileSize = dataSize;
    
    // Check pointer size and endianness
    char c = cursor[7];
    if (c == '_')
    {
        //def.pointerSize = 4;
        //printf("32 bit\n");
		printf("32 bit files are supported\n");
		free(data);
		return 1;
    }
    else if (c == '-')
    {
        def.pointerSize = 8;
        printf("64 bit\n");
    }
    else
    {
        printf("Unknown char at pointer size def: %c\n", c);
        free(data);
        return 1;
    }
    c = cursor[8];
    if (c == 'v')
    {
        def.bLittleEndian = YES;
    }
    else if (c == 'V')
    {
        def.bLittleEndian = NO;
        printf("Big endian files are not supported\n");
        free(data);
        return 1;
    }
	else
    {
        printf("Unknown char at endianness def: %c\n", c);
        free(data);
        return 1;
    }
    printf("Blender version: ");
    //char version[4];
    def.version[0] = cursor[9];
    def.version[1] = cursor[10];
    def.version[2] = cursor[11];
    def.version[3] = '\0';

    printf("\"%s\"\n", def.version);

    def.firstDataObj = (u8*)cursor + 12;
    ZEBlenderDataObj* objList;
    u32 numObjects;
    if (ZE_ReadBlenderDataObjHeaders(def.pointerSize, def.firstDataObj, &objList, &numObjects) != 0)
    {
        printf("Error reading object headers\n");
        return 1;
    }
    printf("Read %d object headers\n", numObjects);

    // Cleanup
    free(objList);
    return 0;
}

static void ZE_ReadBlenderFileTest()
{
    i32 err = ZE_ReadBlenderFile("tools_data/knight.blend");
    if (err != 0)
    {
        printf("Failed to successfully read test file\n");
    }
}

#endif // ZE_READ_BLENDER_FILE_H