#ifndef ZE_READ_BLENDER_FILE_H
#define ZE_READ_BLENDER_FILE_H

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
static void ZE_ReadBlenderDataObjHeader(i32 pointerSize, u8* mem)
{
	printf("Read data obj. Mem: %d\n", (u32)mem);
    u8* cursor = mem;
	char asciCode[4];
	asciCode[0] = *cursor++;
	asciCode[1] = *cursor++;
	asciCode[2] = *cursor++;
	asciCode[3] = *cursor++;
    //u32 asciCode = *(u32*)cursor;
    //cursor += sizeof(u32);
    u32 size = *(u32*)cursor;
    cursor += sizeof(u32);
    u64 ptr;
	if (pointerSize == 4)
	{
		ptr = *(u32*)cursor;
		cursor += 4;
	}
	else if (pointerSize == 8)
	{
		ptr = *(u64*)cursor;
		cursor += 8;
	}
	else
	{
		printf("Invalid ptr size %d aborted header read\n", pointerSize);
		return;
	}
	
	printf("Block type %c,%c,%c,%c - size %d Addr: %lld\n",
		asciCode[0], asciCode[1], asciCode[2], asciCode[3], size, ptr);
}

static void ZE_ReadBlenderFile(char* path)
{
    printf("Reading blender file \"%s\"\n", path);
    u8* data;
    u32 dataSize;
    ZE_std_ReadEntireFileToHeap(path, &data, &dataSize);
    if (data == NULL || dataSize == 0)
    {
        printf("Failed to open blender file\n");
        return;
    }
    printf("  Read %dKB\n", dataSize / 1024);

    char* cursor = (char*)data;
    char* end = cursor + dataSize;

    // Check file magic number - strncmp as no null terminator
    if (strncmp("BLENDER", (const char*)cursor, 7) != 0)
    {
        printf("Not blender format. Abort\n");
        free(data);
        return;
    }

    ZEBlenderFileDef def = {};
    def.file = data;
    def.fileSize = dataSize;
    
    // Check pointer size and endianness
    char c = cursor[7];
    if (c == '_')
    {
        def.pointerSize = 4;
        printf("32 bit\n");
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
        return;
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
        return;
    }
	else
    {
        printf("Unknown char at endianness def: %c\n", c);
        free(data);
        return;
    }
    printf("Blender version: ");
    //char version[4];
    def.version[0] = cursor[9];
    def.version[1] = cursor[10];
    def.version[2] = cursor[11];
    def.version[3] = '\0';

    printf("\"%s\"\n", def.version);

    def.firstDataObj = (u8*)cursor + 12;

    ZE_ReadBlenderDataObjHeader(def.pointerSize, def.firstDataObj);
}

static void ZE_ReadBlenderFileTest()
{
    ZE_ReadBlenderFile("tools_data/knight.blend");
}

#endif // ZE_READ_BLENDER_FILE_H