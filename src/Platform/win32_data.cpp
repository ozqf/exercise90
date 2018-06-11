#pragma once

#include "../common/com_module.h"

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
    FILE* handle;
    DataFileDiskHeader header;
	u32 fileSize;
};

struct DataFileEntryReader
{
	FILE* handle;
	DataFileDiskEntry entry;
};

// Data files will be opened and kept open for program duration
#define PLATFORM_MAX_DATA_FILES 64
static DataFile g_dataFiles[PLATFORM_MAX_DATA_FILES];
static i32 g_nextDataFileIndex = 0;
static char* g_baseDirectoryName = "base";

void Win32_DebugPrintDataManifest()
{
	for (i32 i = g_nextDataFileIndex - 1; i >= 0; --i)
	{
		DataFile file = g_dataFiles[i];
		Assert(file.handle);

		printf("--- data file %d ---\n", i);

		u32 offset = file.header.fileListOffset;
		fseek(file.handle, file.header.fileListOffset, SEEK_SET);
		DataFileDiskEntry entry = {};

		for (u32 j = 0; j < file.header.numFiles; ++j)
		{
			fread(&entry, sizeof(DataFileDiskEntry), 1, file.handle);

			printf("%s - Size %d bytes, FileType: %d\n", entry.fileName, entry.size, entry.info[0]);
		}
	}
}

// File path should be something like textures/tex.bmp
u8 Win32_FindDataFileEntry(char* filePath, DataFileEntryReader* reader)
{
	Assert(reader);
	char* path = filePath;
	// data file entry paths are stored with an opening backslash.
	// if the request didn't include one we should skip it when comparing
	// file names
	u8 skipOpeningSlash = (*filePath != '\\');

	// search data files in reverse order
	// so that later files can 'patch earlier files
	for (i32 i = g_nextDataFileIndex - 1; i >= 0; --i)
	{
		DataFile file = g_dataFiles[i];
		Assert(file.handle);
		u32 offset = file.header.fileListOffset;
		fseek(file.handle, file.header.fileListOffset, SEEK_SET);
		DataFileDiskEntry entry = {};

		for (u32 j = 0; j < file.header.numFiles; ++j)
		{
			fread(&entry, sizeof(DataFileDiskEntry), 1, file.handle);

			char* fileName = (char*)entry.fileName;

			if (skipOpeningSlash) { fileName++; }

			if (COM_CompareStrings(path, fileName) == 0)
			{
				reader->handle = file.handle;
				reader->entry = entry;
				return 1;
			}
		}
	}
	return 0;
}

void Win32_AddDataFileHandle(char* filePath)
{
	
	DataFile dFile = {};
	

	fopen_s(&dFile.handle, filePath, "rb");
	Assert(dFile.handle != NULL);

	i32 end;
	fseek(dFile.handle, 0, SEEK_END);
	end = ftell(dFile.handle);
	fseek(dFile.handle, 0, SEEK_SET);
	dFile.fileSize = end;

	fread(&dFile.header, sizeof(DataFileDiskHeader), 1, dFile.handle);

	if (dFile.header.magic[0] != 'P'
		|| dFile.header.magic[1] != 'A'
		|| dFile.header.magic[2] != 'C'
		|| dFile.header.magic[3] != 'K'
		)
	{
		printf("PLATFORM Magic number of .dat not recognised\n");
		fclose(dFile.handle);
		return;
	}

	char buf[512];
	sprintf_s(buf, 512, "PLATFORM Loaded handle to %s. Size: %dkb, num files: %d\n", filePath, (dFile.fileSize / 1024), dFile.header.numFiles);
	printf("%s", buf);
	//dFile->header = 


	//increment next available handle now we know that this one is okay
	g_dataFiles[g_nextDataFileIndex] = dFile;
	g_nextDataFileIndex++;

	Assert((g_nextDataFileIndex + 1) < PLATFORM_MAX_DATA_FILES);
}

/**
 * returns number of data file handles established
 */
i32 Win32_ScanForDataFiles(Heap* heap, DataFile* results, i32 maxResults, char* path)
{
	char currentDir[256];
	u32 dirStrSize = GetCurrentDirectory(256, currentDir);
	Assert(dirStrSize > 0);

	char searchPath[256];
	sprintf_s(searchPath, 256, "%s\\%s\\*", currentDir, path);


	// printf("Current Dir/base dir:\n");
	// printf(currentDir);
	// printf("\n");
	// printf(searchPath);
	// printf("\n");
	printf("PLATFORM Data path \"%s\"\n", searchPath);

	// concat of search dir and file name
	char fileLoadPath[256];

	WIN32_FIND_DATA findData;

	HANDLE handle = INVALID_HANDLE_VALUE;
	handle = FindFirstFile(searchPath, &findData);
	if (handle == INVALID_HANDLE_VALUE)
	{
		return 0;
	}

	i32 count = 0;
	do
	{
		if (COM_CheckForFileExtension(findData.cFileName, ".dat"))
		{
			//printf("Found data file %s:\n", findData.cFileName);
			// rebuild from exe path not game path as we don't want the '*' in the path
			sprintf_s(fileLoadPath, 256, "%s\\%s\\%s", currentDir, path, findData.cFileName);
			Win32_AddDataFileHandle(fileLoadPath);
			count++;
		}
	} while (FindNextFile(handle, &findData) != 0);
	FindClose(handle);
	return count;
}

void Win32_CloseDataFiles()
{
	for (i32 i = g_nextDataFileIndex - 1; i >= 0; --i)
	{
		fclose(g_dataFiles[i].handle);
		g_dataFiles[i] = {};
	}
	g_nextDataFileIndex = 0;
}

void Win32_LoadDataFiles()
{
	// Possibly reloading. close any datafiles currently open
	printf("PLATFORM: Recreate Data File Handles\n");
	Win32_CloseDataFiles();
	Win32_ScanForDataFiles(NULL, g_dataFiles, PLATFORM_MAX_DATA_FILES, g_baseDirectoryName);

	//Win32_FindDataFileEntry(NULL, "charset.bmp");
}
