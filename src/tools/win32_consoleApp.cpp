/*******************************************
 * Windows Console application main for 
 * testing purposes
 ******************************************/

#pragma once

#include <stdio.h>

#include "../common/common.h"
#include "tests/tests.h"

#include "../Platform/win32_system_include.h"

#define MAX_PATH_LENGTH 256

//#include "tests/test_command_stream.h"

/**
 * Returns number of files found
 */
internal i32 Test_Win32_ScanDirectory(char* searchPath)
{
    WIN32_FIND_DATA findData;

    HANDLE handle = INVALID_HANDLE_VALUE;
    handle = FindFirstFile(searchPath, &findData);
    if (handle == INVALID_HANDLE_VALUE)
    {
        return 0;
    }

    // searchPath ends with * and we need to ignore
    // that if we have to enter a sub-directory
    i32 searchPathLength = COM_StrLen(searchPath);
    char subDirRoot[MAX_PATH_LENGTH];
    COM_SET_ZERO(subDirRoot, MAX_PATH_LENGTH);
    COM_COPY(searchPath, subDirRoot, searchPathLength - 1);

    i32 count = 0;
    do
    {
        // skip navigation file names:
        if (COM_CompareStrings(findData.cFileName, ".") == 0)
        { continue; }
        if (COM_CompareStrings(findData.cFileName, "..") == 0)
        { continue; }
        if (COM_CompareStrings(findData.cFileName, ".vs") == 0)
        { continue; }
        if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            // recurse
            printf("Found Dir %s in search path %s\n", findData.cFileName, searchPath);
            char subPath[MAX_PATH_LENGTH];
            sprintf_s(subPath, "%s%s\\*", subDirRoot, findData.cFileName);
            printf("Search Sub-directory %s\n", subPath);
            Test_Win32_ScanDirectory(subPath);
            //printf("Found directory. attribs %d: \"%s\"\n", findData.dwFileAttributes, findData.cFileName);
        }
        else
        {
            printf("Found file. Attribs %d Name \"%s\"\n", findData.dwFileAttributes, findData.cFileName);
        }

    } while (FindNextFile(handle, &findData) != 0);
    // Clean up
    FindClose(handle);
    return count;
}

/**
 * Returns number of files found
 */
internal i32 Test_Win32_BeginDirectoryScan(char* relativePath)
{
    char currentDir[MAX_PATH_LENGTH];
    u32 dirStrSize = GetCurrentDirectory(256, currentDir);
    COM_ASSERT(dirStrSize > 0, "GetCurrentDirectory failed")
    COM_ASSERT(dirStrSize < MAX_PATH_LENGTH, "GetCurrentDirectory name overflow")

    // Build search path
    char searchPath[MAX_PATH_LENGTH];
    sprintf_s(searchPath, MAX_PATH_LENGTH, "%s\\%s\\*", currentDir, relativePath);

    // buffer for concat of search dir and file name
    //char fileLoadPath[max_length];

    // search
    return Test_Win32_ScanDirectory(searchPath);
}

internal void Test_FatalError(const char* message, const char* heading)
{
    printf("!--- %s ---!\n%s", heading, message);
}

// main function for everywhere except windows
int main(i32 argc, char* argv[])
{
    COM_SetFatalError(Test_FatalError);
    Test_Win32_BeginDirectoryScan("");
    //Tests_Run(argc, argv);
    //TestCommandStream();
    //TestAcks();
    return 0;
}

// Windows 'Main'
// int CALLBACK WinMain(
//     HINSTANCE hInstance,
//     HINSTANCE hPrevInstance,
//     LPSTR lpCmdLine,
//     int nCmdShow) { }
