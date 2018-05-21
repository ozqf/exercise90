#pragma once

#include "../common/com_module.h"
#include <stdio.h>

#define READ_MODE_NONE 0
#define READ_MODE_NUMBER 1
#define READ_MODE_SKIP_LINE 2

void Test_ReadMesh()
{
    char *path = "./base/box.obj";
    FILE *f;
    fopen_s(&f, path, "r");
    if (f == NULL)
    {
        printf("Failed to open %s...Aboring\n", path);
        return;
    }
    i32 end;
    fseek(f, 0, SEEK_END);
    end = ftell(f);
    fseek(f, 0, SEEK_SET);
    printf("Read %d chars in %s\n", end, path);

    i32 pos = 0;

    u8 reading = 1;
    u32 count = 0;

    i32 bufSize = 64;
    char buf[64];
    i32 bufReadPos = 0;
    COM_ZeroMemory((u8*)buf, 64);
    u8 mode = READ_MODE_NONE;

    i32 numVerts = 0;
    i32 numFaces = 0;

    while (reading)
    {
        char c;
        fread(&c, 1, 1, f);

        pos++;
        if (pos >= end)
        {
            break;
        }
        
        if (pos > 1000)
        {
            break;
        }

        switch (mode)
        {
            case READ_MODE_SKIP_LINE:
            {
                if (c == '\n')
                {
                    mode = READ_MODE_NONE;
                    continue;
                }
            }
            break;
            case READ_MODE_NUMBER:
            {
                if ((c >= '0' && c <= '9') || c == '.')
                {
                    buf[bufReadPos] = c;
                    bufReadPos++;
                    //printf("%c", c);
                }
                else
                {
                    // End reading float
                    buf[bufReadPos] = NULL;
                    bufReadPos++;
                    f32 num = (f32)atof(buf);
                    printf(" F: %.2f ", num);
                    mode = READ_MODE_NONE;
                }
            }
            break;
            default:
            {
                if (c == '#')
                {
                    mode = READ_MODE_SKIP_LINE;
                }
                else if (c == 'v')
                {
                    numVerts++;
                    mode = READ_MODE_SKIP_LINE;
                }
                else if (c == 'f')
                {
                    numFaces++;
                    mode = READ_MODE_SKIP_LINE;
                }
                else if (c == '-' || (c >= '0' && c <= '9'))
                {
                    //printf("%c", c);
                    mode = READ_MODE_NUMBER;
                    bufReadPos = 0;
                    buf[bufReadPos] = c;
                    bufReadPos++;
                    continue;
                }
                else if (c == '\n')
                {

                }
                else
                {
                    mode = READ_MODE_SKIP_LINE;
                    //printf("%c", c);
                    //count++;
                }
            }
            break;
        }
    }

    fclose(f);

    printf("Printed %d characters\nRead %d vertices and %d faces\n", count, numVerts, numFaces);
}
