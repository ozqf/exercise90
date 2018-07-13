#pragma once

#include "com_module.cpp"

//////////////////////////////////////////////////////////////
// Command header
//////////////////////////////////////////////////////////////


#define COMMAND_PROTOCOL_ID (u32)1

/**
 * Game Command Header
 * 
 * !Do not access members directly, use attached functions!
 * 
 * Notes;
 * > Describes the type of command and the number of bytes used to store it
 * > data is accessed via getter/setter to allow struct internals to be changed
 *      more easily
 * > Storing size allows free stepping through command buffers without knowledge of
 *      commands themselves.
 * > In future will store some bit flags for meta data about the command itself
 *      (eg how it is encoded)
 */
struct CmdHeader
{
    u32 data1;
    //u16 flags;
    u32 data2;
    /* 4 bytes?
    u8 type;
    u8 flags;
	u16 size;
    */

    //u32 data1;
	//u32 data2;
    
    inline u8 GetType() { return (u8)data1; }
    inline void SetType(u8 newType) { this->data1 = newType; }
    inline u16 GetSize() { return (u16)data2; }
    inline void SetSize(u16 newSize) { this->data2 = newSize; }

    inline u16 Read(u8* ptr)
    {
        COM_COPY(ptr, this, sizeof(CmdHeader));
        return sizeof(CmdHeader);
    }

    inline u16 Write(u8* ptr)
    {
        COM_COPY(this, ptr, sizeof(CmdHeader));
        return sizeof(CmdHeader);
    }
};

/**
 * Write cmd boilerplate
 * > ptrWrite will be incremented to the next
 *   write position
 * > Header must be written after the actual command
 *   so that the header can record bytes written
 * > Therefore header must have fixed size
 * > Header and Command structs must have a
 *   u32 Write(u8*) function, returning bytes writtten
 */
#ifndef COM_WRITE_CMD_TO_BUFFER
#define COM_WRITE_CMD_TO_BUFFER(u8ptr2ptr_write, u8_cmdType, u8_cmdFlags, cmdObject) \
{ \
u8* ptrOrigin = *u8ptr2ptr_write##; \
 \
*u8ptr2ptr_write += sizeof(CmdHeader); \
 \
u16 cmdBytesWritten = cmdObject##.WriteRaw(*##u8ptr2ptr_write##); \
 \
*u8ptr2ptr_write += cmdBytesWritten; \
CmdHeader newCmdHeader = {}; \
newCmdHeader.SetType(##u8_cmdType##); \
newCmdHeader.SetSize(cmdBytesWritten); \
newCmdHeader.Write(ptrOrigin); \
}
#endif

//////////////////////////////////////////////////////////////
// Command stream-from-file
//////////////////////////////////////////////////////////////

struct ReplayFrameHeader
{
    u32 frameNumber;
    u32 size;
	char label[16];
};

struct FileSegment
{
	u32 offset = 0;
	u32 count = 0;
    // if size is 0, nothing was written, if size is negative, writing was started
    // but not completed (ie it crashed), so size of data is unknown
    // try reading regardless, for debugging!
	i32 size = 0;
};

 
/* File Sections:
> static entities are skipped if a baseFile was specified
> Dynamic entities are skipped if this file is a 'base' file
> Frames are written into replay demo files.
*/
   
struct StateSaveHeader
{
	char magic[4] = { 'S', 'A', 'V', 'E' };
	char baseFile[32];
    
	FileSegment staticCommands;
	FileSegment dynamicCommands;
	FileSegment frames;
};

struct StateSaveHeader2
{
	char magic[4] = { 'S', 'A', 'V', 'E' };
    u32 protocol = COMMAND_PROTOCOL_ID;
    u32 flags = 0;
	char baseFile[32];
    
	FileSegment staticCommands;
	FileSegment dynamicCommands;
	FileSegment frames;
};
