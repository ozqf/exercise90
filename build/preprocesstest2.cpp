#line 1 "../preprocess/preprocesstest2.h"
#pragma once
//////////////////////////////////////////////////
// Preprocessor Test 2
//////////////////////////////////////////////////












struct CmdHeader
{
    unsigned int type;
    unsigned int size;

    unsigned int Write(unsigned char* ptr)
    {
        return 8;
    }
};

struct GameCmd
{
    int id;

    unsigned int Write(unsigned char* ptr)
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












void Foo()
{
    unsigned char* ptr;
    CmdHeader h = {};
    GameCmd cmd = {};

    unsigned char* ptrOrigin = ptr; ptr += 8;
    int cmdBytesWritten = &cmd->Write(ptr);
    ptr += cmdBytesWritten;
    &h->size = cmdBytesWritten;
    &h->Write(ptrOrigin);
}
