#pragma once

#include "../common/com_module.cpp"

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
    printf("%s is %d Bytes\n", sourceFilePath, sourceSize);
    u8* ptrSourceOrigin = (u8*)malloc(sourceSize);
    if (ptrSourceOrigin == NULL)
    {
        printf("Malloc failed");
        return;
    }
    fread(ptrSourceOrigin, sourceSize, 1, srcFile);
    fclose(srcFile);

    u8* ptrRead = ptrSourceOrigin;

    ///////////////////////////////////////////////////
    // Copy header
    ///////////////////////////////////////////////////
    
    StateSaveHeader sourceHeader = {};
    ptrRead += COM_COPY_STRUCT(ptrRead, &sourceHeader, StateSaveHeader);

    // Prepare target header for info
    StateSaveHeader2 tarHeader;
    printf("Tar header %c %c %c %c protocol: %d flags: %d\n",
        tarHeader.magic[0],
        tarHeader.magic[1],
        tarHeader.magic[2],
        tarHeader.magic[3],
        tarHeader.protocol,
        tarHeader.flags
    );

    // Step write point forward, header will be written afterward.
    fseek(tarFile, sizeof(StateSaveHeader2), SEEK_SET);


    ///////////////////////////////////////////////////
    // Static commands
    ///////////////////////////////////////////////////
    printf("Static cmds: size %d count %d offset %d\n",
        sourceHeader.staticCommands.size,
        sourceHeader.staticCommands.count,
        sourceHeader.staticCommands.offset
    );

    
    // Finish file
    fseek(tarFile, 0, SEEK_SET);
    fwrite(&tarHeader, sizeof(StateSaveHeader2), 1, tarFile);
    fclose(tarFile);
}

void Test_PatchStateFile()
{
    Test_PatchToProtocol2("base\\testbox_old.lvl", "base\\testbox_new.lvl");
}
