#pragma once
//////////////////////////////////////////////////
// Preprocessor Test 2
//////////////////////////////////////////////////
#define u32 unsigned int
#define i32 int
#define u8 unsigned char
#define ASSERT(expr) if (!(expr)) { *(int *)0 = 0; }

#define MAX_FOO 2048
#define MAX_BAR 2048

#define NULL 0

#define CMD_HEADER_SIZE 8

struct CmdHeader
{
    u32 type;
    u32 size;

    u32 Write(u8* ptr)
    {
        return 8;
    }
};

struct GameCmd
{
    i32 id;

    u32 Write(u8* ptr)
    {
        return 8;
    }
};

/**
 * Write cmd boilerplate
 * > ptrWrite will be left incremented to the next
 *   write position
 * > Header must be written after the actual command
 *   so that the header can record bytes written
 * > Therefore header must have fixed size
 * > Header and Command structs must have a u32 Write(u8*) function
 */
#define WRITE_CMD(ptrWrite, ptrHeader, ptrCmd) \
 \
u8* ptrOrigin = ptrWrite##; \
 \
ptrWrite += CMD_HEADER_SIZE; \
 \
i32 cmdBytesWritten = ptrCmd##->Write(##ptrWrite##); \
 \
ptrWrite += cmdBytesWritten; \
ptrHeader##->size = cmdBytesWritten; \
ptrHeader##->Write(ptrOrigin) 

void Foo()
{
    u8* ptr;
    CmdHeader h = {};
    GameCmd cmd = {};

    WRITE_CMD(ptr, &h, &cmd);
}
