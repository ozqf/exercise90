#pragma once

#include "../common/com_module.h"

typedef struct
{ u8 magic[4];     // Name of the new WAD format
  i32 diroffset;              // Position of WAD directory from start of file
  i32 dirsize;                // Number of entries * 0x40 (64 char)
} pakheader_t;

typedef struct
{ u8 filename[0x38];       // Name of the file, Unix style, with extension,
                               // 50 chars, padded with '\0'.
  long offset;                 // Position of the entry in PACK file
  long size;                   // Size of the entry in PACK file
} pakentry_t;

void Test_pak()
{
    printf("Test pak files\n");
    FILE* f;
    fopen_s(&f, "d:\\games\\quake\\id1\\PAK0.pak", "rb");

    i32 end;

    // Scan for file size
    // pos = ftell(f); // assuming start at 0 anyway
    fseek(f, 0, SEEK_END);
    end = ftell(f);
    fseek(f, 0, SEEK_SET);

#if 0
    // Read magic as raw bytes
    u8 magic[4];
    fread(&magic, 1, 4, file);
    printf("Read magic: %c, %c, %c, %c\n", magic[0], magic[1], magic[2], magic[3]);
#endif

    // Read pak header
    pakheader_t header = {};
    fread(&header, sizeof(pakheader_t), 1, f);
    printf("Read magic: %c, %c, %c, %c\n", header.magic[0], header.magic[1], header.magic[2], header.magic[3]);
    if (header.magic[0] != 'P' || header.magic[1] != 'A' || header.magic[2] != 'C' || header.magic[3] != 'K')
    {
        printf("Magic not recognised as 'P A C K', aborted\n");
    }
    else
    {
        printf("Size: %d bytes, dir offset: %d, dir size: %d\n", end, header.diroffset, header.dirsize);
        int numFiles = header.dirsize / sizeof(pakentry_t);
        printf("num files: %d\n", numFiles);
        int tableSize = end - header.diroffset;
        printf("Table size: %d\n", tableSize);

        //void* mem;
        //mem = malloc(numFiles * sizeof(pakentry_t));
        //pakentry_t* fileManifest = (void*)malloc(numFiles * sizeof(pakentry_t));
        pakentry_t* fileManifest = (pakentry_t*)malloc(numFiles * sizeof(pakentry_t));
        fseek(f, header.diroffset, SEEK_SET);
        fread((void*)fileManifest, header.dirsize, 1, f);

        printf("First file in manifest:\n");
        printf("%s", fileManifest[0].filename);
    }
    

    fclose(f);
}