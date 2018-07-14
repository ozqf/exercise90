#pragma once

#include "../common/com_module.cpp"

union HeaderTest
{
    struct
    {
        u8 type;
        u8 flags;
        u16 size;
    };
    u32 data;
};

// size of this struct is 8 bytes, not 6, due to alignment
struct PackingTest
{
    u8 a;
    u16 b;
    u32 c;
};

// Returns bytes written
u32 Test_PatchCommandBufferToFile(u8* ptrRead, u8* ptrEnd, FILE* tar, FileSegment* segment)
{
    u32 writeStart = ftell(tar);
    segment->offset = writeStart;
    if (ptrRead == ptrEnd)
    {
        // no commands to write, clear and return
        printf("  No commands to write for this segment\n");
        *segment = {};
        return 0;
    }
    while (ptrRead < ptrEnd)
    {
        CmdHeader_ProtocolZero sourceHeader;
        ptrRead += sourceHeader.Read(ptrRead);
        i32 bytesToWrite = sourceHeader.GetSize();
        printf("CMD: type %d size %d \n", sourceHeader.GetType(), bytesToWrite);

        CmdHeader h = {};
        h.SetType(sourceHeader.GetType());
        h.SetSize(sourceHeader.GetSize());

        // write new header. Not affecting the actual command
        fwrite(&h, sizeof(CmdHeader), 1, tar);
        fwrite(ptrRead, 1, bytesToWrite, tar);
        // advance
        ptrRead += sourceHeader.GetSize();
        segment->count++;
    }
    segment->size = (ftell(tar) - writeStart);
    return segment->size;
}

void Test_PatchToProtocol2(char* sourceFilePath, char* targetFilePath)
{
    printf("*** Patching %s to %s ***\n", sourceFilePath, targetFilePath);
    
    FILE* srcFile;
    fopen_s(&srcFile, sourceFilePath, "rb");
    if (srcFile == NULL)
    {
        printf("Failed to open patch source %s\n", sourceFilePath);
        return;
    }

    FILE* tarFile;
    fopen_s(&tarFile, targetFilePath, "wb");

    if (tarFile == NULL)
    {
        printf("Failed to open patch target %s\n", targetFilePath);
    }


    ///////////////////////////////////////////////////
    // Copy Source file into memory
    ///////////////////////////////////////////////////
    fseek(srcFile, 0, SEEK_END);
    i32 sourceSize = ftell(srcFile);
    fseek(srcFile, 0, SEEK_SET);
    printf("%s is %d Bytes\n", sourceFilePath, sourceSize);
    u8* ptrSourceOrigin = (u8*)malloc(sourceSize);
    if (ptrSourceOrigin == NULL)
    {
        printf("Malloc failed\n");
        return;
    }
    fread(ptrSourceOrigin, sourceSize, 1, srcFile);
    fclose(srcFile);

    u8* ptrRead = ptrSourceOrigin;

    ///////////////////////////////////////////////////
    // Copy header
    ///////////////////////////////////////////////////
    
    StateSaveHeader_ProtocolZero sourceHeader = {};
    ptrRead += COM_COPY_STRUCT(ptrRead, &sourceHeader, StateSaveHeader_ProtocolZero);

    // Prepare target header for info
    StateSaveHeader_New tarHeader;

    // Step write point forward, header will be written last.
    fseek(tarFile, sizeof(StateSaveHeader_New), SEEK_SET);


    ///////////////////////////////////////////////////
    // Static commands
    ///////////////////////////////////////////////////
    printf("Static cmds: size %d count %d offset %d\n",
        sourceHeader.staticCommands.size,
        sourceHeader.staticCommands.count,
        sourceHeader.staticCommands.offset
    );

    u8* ptrEnd;
    ptrRead = ptrSourceOrigin + sourceHeader.staticCommands.offset;
    ptrEnd = ptrRead + sourceHeader.staticCommands.size;
	Test_PatchCommandBufferToFile(ptrRead, ptrEnd, tarFile, &tarHeader.staticCommands);
	
    ///////////////////////////////////////////////////
    // Dynamic commands
    ///////////////////////////////////////////////////
    printf("Dynamic cmds: size %d count %d offset %d\n",
        sourceHeader.dynamicCommands.size,
        sourceHeader.dynamicCommands.count,
        sourceHeader.dynamicCommands.offset
    );
    ptrRead = ptrSourceOrigin + sourceHeader.dynamicCommands.offset;
    ptrEnd = ptrRead + sourceHeader.dynamicCommands.size;
	Test_PatchCommandBufferToFile(ptrRead, ptrEnd, tarFile, &tarHeader.dynamicCommands);
    
    ///////////////////////////////////////////////////
    // Write target header
    ///////////////////////////////////////////////////
    fseek(tarFile, 0, SEEK_SET);

    printf("Tar header %c %c %c %c protocol: %d settings: %d %d %d %d\n",
        tarHeader.magic[0],
        tarHeader.magic[1],
        tarHeader.magic[2],
        tarHeader.magic[3],
        tarHeader.protocol,
        tarHeader.settings[0],
        tarHeader.settings[1],
        tarHeader.settings[2],
        tarHeader.settings[3]
    );
    FileSegment* seg;
    seg = &tarHeader.staticCommands;
    printf("  Static: offset %d count %d size %d\n",
        seg->offset,
        seg->count,
        seg->size
    );
    seg = &tarHeader.dynamicCommands;
    printf("  Dynamic: offset %d count %d size %d\n",
        seg->offset,
        seg->count,
        seg->size
    );
    seg = &tarHeader.frames;
    printf("  Frames: offset %d count %d size %d\n",
        seg->offset,
        seg->count,
        seg->size
    );

    fwrite(&tarHeader, sizeof(StateSaveHeader_New), 1, tarFile);
    fseek(tarFile, 0, SEEK_END);
    printf("  Wrote %d bytes to %s\n", ftell(tarFile), targetFilePath);
    fclose(tarFile);
}

void Test_PatchStateFile()
{
    HeaderTest h = {};
    h.type = 102;
    h.size = 84;
    h.flags = 127;
    //printf("Size of test header %d\n", sizeof(h));
    //printf("type %d size %d flags %d\n", h.type, h.size, h.flags);
    //printf("Data: %d\n", h.data);

    //printf("size of packing test:%d\n", sizeof(PackingTest));

    printf("Size of CMD Headers:\nOld state header: %d\nNew state header: %d\n", sizeof(StateSaveHeader_ProtocolZero), sizeof(StateSaveHeader_New));
    Test_PatchToProtocol2("base\\testbox_old.lvl", "base\\testbox_new.lvl");
}
