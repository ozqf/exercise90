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

void Test_PackRead()
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
		return;
	}
	
	printf("File is an archive with %d files. File Manifest offset: %d\n", header.numFiles, header.fileListOffset);

	DataFileDiskEntry* files = (DataFileDiskEntry*)malloc(header.numFiles * sizeof(DataFileDiskEntry));

	fseek(f, header.fileListOffset, SEEK_SET);

	for (u32 i = 0; i < header.numFiles; ++i)
	{
		fread(&files[i], sizeof(DataFileDiskEntry), header.numFiles, f);
		printf("Read manifest item for %s. Type: %d, size: %d, location: %d\n", files[i].fileName, files[i].info[0], files[i].size, files[i].offset);
	}

	free((void*)files);
}


void Test_Pack()
{
	//TestDirectoryScan();
	Test_PackRead();
}
