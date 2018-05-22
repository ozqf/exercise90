#pragma once

#include "../common/com_module.h"
#include <stdio.h>

#define READ_MODE_NONE 0
#define READ_MODE_NUMBER 1
#define READ_MODE_SKIP_LINE 2

struct FileReadInfo
{
    FILE* f;
    u32 pos;
    u32 length;
};

struct MeshStats
{
    Vec3* vertices;
    u32 numVertices;
    u32 maxVertices;
    Vec3* triangles;
    u32 numTriangles;
    u32 maxTriangles;
};

struct ObjFileProperties
{
    u32 fileSize;
    u32 numVertices;
    u32 numFaces;
};

void Test_SkipLine(FileReadInfo* f);

ObjFileProperties Test_ReadObjProperties(FileReadInfo* f)
{
    ObjFileProperties props = {};

    fseek(f->f, 0, SEEK_END);
    props.fileSize = ftell(f->f);
    fseek(f->f, 0, SEEK_SET);

    u32 pos = 0;
    u8 mode = READ_MODE_NONE;

    for (;;)
    {
        char c;
        fread(&c, 1, 1, f->f);
        pos++;
        if (pos >= props.fileSize)
        {
            break;
        }
        if (mode == READ_MODE_SKIP_LINE)
        {
            if (c == '\n')
            {
                mode = READ_MODE_NONE;
                continue;
            }
        }
        else
        {
            if (c == 'v')
            {
                props.numVertices++;
                //mode = READ_MODE_SKIP_LINE;
                Test_SkipLine(f);
                continue;
            }
            else if (c == 'f')
            {
                props.numFaces++;
                Test_SkipLine(f);
                //mode = READ_MODE_SKIP_LINE;
                continue;
            }
            else if (c == '\n')
            {
                // Make sure detecting empty lines doesn't cause excess skipping
            }
            else
            {
                mode = READ_MODE_SKIP_LINE;
            }
        }
    }

    fseek(f->f, 0, SEEK_SET);

    return props;
}

void Test_SkipLine(FileReadInfo* f)
{
    char c = 0;
    do
    {
        fread(&c, 1, 1, f->f);
        f->pos++;
    } while (c != '\n' && f->pos < f->length);
}

f32 Test_ReadVertex(FileReadInfo* f, MeshStats* stats)
{
    char buf[32];
    i32 bufPos = 0;
    char c;
    do
    {
        fread(&c, 1, 1, f->f);
        f->pos++;
        
        // Read into parse buffer
        if ((c >= '0' && c <= '9') || c == '.')
        {
            buf[bufPos] = c;
            bufPos++;
        }
        else
        {
            break;
        }

    } while (c != '\n' && f->pos < f->length);

    // end reading the float and parse
    buf[bufPos] = NULL;
    bufPos++;
    return (f32)atof(buf);
}

void Test_ReadMesh()
{
    char *path = "./base/box.obj";
    
    FileReadInfo info = {};
    fopen_s(&info.f, path, "r");
    if (info.f == NULL)
    {
        printf("Failed to open %s...Aboring\n", path);
        return;
    }
    fseek(info.f, 0, SEEK_END);
    info.length = ftell(info.f);
    fseek(info.f, 0, SEEK_SET);
    printf("Read %d chars in %s\n", info.length, path);

    info.pos = 0;

    u8 reading = 1;
    u32 count = 0;

    i32 bufSize = 64;
    char buf[64];
    i32 bufReadPos = 0;
    COM_ZeroMemory((u8*)buf, 64);
    u8 mode = READ_MODE_NONE;

    i32 numVerts = 0;
    i32 numFaces = 0;

    ObjFileProperties props = Test_ReadObjProperties(&info);

    MeshStats mesh = {};
    mesh.maxVertices = props.numVertices;
    mesh.vertices = (Vec3*)malloc(sizeof(Vec3) * mesh.maxVertices);

#if 1
    while (reading)
    {
        char c;
        fread(&c, 1, 1, info.f);

        info.pos++;
        if (info.pos >= info.length)
        {
            break;
        }
        
        if (info.pos > 1000)
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
#if 0
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
#endif
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
#endif

    fclose(info.f);

    printf("Printed %d characters\nRead %d vertices and %d faces\n", count, props.numVertices, props.numFaces);
}
