#pragma once

#include "../com_module.h"

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

struct DataFile
{
    FILE* file;
    DataFileDiskheader header;
    DataFileDiskEntry* manifest;
};
