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
    u64 oldPtr;
    u32 SDNAIndex;
    u32 numElements;
    u32 numBytes;
    void* dataPtr;
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
    i32 pointerSize, u8* mem, ZEBlenderDataObj** out_objects, u32* out_numObjects, i32* out_dna1Index)
{
    *out_objects = NULL;
    *out_numObjects = 0;
    *out_dna1Index = -1;
    // TODO: Make this resizeable... or scan for size before allocating array.
    // Even a simple object seemed to have 5k+ objects...
    const u32 maxObjects = 10000;
    u32 bytesForTable = sizeof(ZEBlenderDataObj) * maxObjects;
    printf("Allocating %dKB for file table\n", bytesForTable / 1024);
    ZEBlenderDataObj* objects = (ZEBlenderDataObj*)malloc(bytesForTable);
    u32 numObjects = 0;
    i32 dna1Index = -1;
    
    i32 bVerbose = NO;
    i32 numVerbose = 10;
    u8* cursor = mem;
	
	// stats
    i32 stat_numHeaders = 0;
	i32 stat_numDataItems = 0;
    i32 stat_numSDNA = 0;
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
        else if (strncmp(obj->asciCode, "DNA1", 4) == 0)
		{
            dna1Index = (numObjects - 1);
		}
        else if (strncmp(obj->asciCode, "SDNA", 4) == 0)
		{
            stat_numSDNA++;
		}
        stat_numHeaders++;
        //u32 asciCode = *(u32*)cursor;
        //cursor += sizeof(u32);
        obj->size = *(u32*)cursor;
        cursor += sizeof(u32);
        
	    if (pointerSize == 4)
	    {
	    	obj->oldPtr = *(u32*)cursor;
	    	cursor += 4;
	    }
	    else if (pointerSize == 8)
	    {
	    	obj->oldPtr = *(u64*)cursor;
	    	cursor += 8;
	    }
	    else
	    {
	    	printf("Invalid ptr size %d aborted header read\n", pointerSize);
	    	return 1;
	    }
	    if (bVerbose) { printf("Block type \"%c%c%c%c\" - size %d Addr: %lld\n",
	    	obj->asciCode[0], obj->asciCode[1], obj->asciCode[2], obj->asciCode[3], obj->size, obj->oldPtr); }
    
	    obj->SDNAIndex = *(u32*)cursor;
	    cursor += sizeof(u32);
    
	    obj->numElements = *(u32*)cursor;
	    cursor += sizeof(u32);

        // record location of data
        obj->dataPtr = cursor;

	    if (bVerbose) { printf("SDNA index: %d num elements: %d\n", obj->SDNAIndex, obj->numElements); }
	    cursor += obj->size;
        numVerbose--;
        if (numVerbose == 0) { bVerbose = NO; }
    }
	//printf("Read %d data objects\n", numHeaders);]
	printf("Num DATA items: %d\n", stat_numDataItems);
    printf("Num SDNA items: %d\n", stat_numSDNA);
    *out_objects = objects;
    *out_numObjects = numObjects;
    *out_dna1Index = dna1Index;
	return 0;
}

static i32 ZE_ReadBlenderSDNA(ZEBlenderDataObj* header)
{
    printf("Reading SDNA\n");
    u8* cursor = (u8*)header->dataPtr;
    const i32 sndaHeaderLength = 8;
    i32 compResult = strncmp((char*)cursor, "SDNANAME", sndaHeaderLength);
    if (compResult != 0)
    {
        printf("Invalid SNDA header prefix\n");
        return 1;
    }
    cursor += sndaHeaderLength;
    i32 numBlenderVarNames = *(i32*)cursor;
    cursor += sizeof(i32);
    printf("Num blender var names %d\n", numBlenderVarNames);

    return 0;
}

////////////////////////////////////////////////////////////
// Read file entry point
////////////////////////////////////////////////////////////
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
    i32 dna1Index;
    i32 err = ZE_ReadBlenderDataObjHeaders(
        def.pointerSize, def.firstDataObj, &objList, &numObjects, &dna1Index);
    if (err != 0)
    {
        printf("Error reading object headers\n");
        return 1;
    }
    printf("Read %d object headers. DNA1 index is %d\n", numObjects, dna1Index);
    if (dna1Index < 0)
    {
        printf("Error - Invalid DNA1 index %d\n", dna1Index);
        free(objList);
        return 1;
    }

    err = ZE_ReadBlenderSDNA(&objList[dna1Index]);

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