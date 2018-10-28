#pragma once

#include "../../common/com_module.h"

///////////////////////////////////////////////////////////
// Try to keep data specific details in here and away from
// the streams they are read from/written to
///////////////////////////////////////////////////////////

struct MessageHeader
{
    u32 id;
    u32 size;

    i32 Write(u8* bytes)
    {
        u8* write = bytes;
        write += COM_WriteU32(id, write);
        write += COM_WriteU32(size, write);
        return (i32)(write - bytes);
    }
    
    i32 Read(u8* bytes)
    {
        u8* read = bytes;
        id = COM_ReadU32(&read);
        size = COM_ReadU32(&read);
        return read - bytes;
    }
};

#define TEST_MSG_TYPE_NULL 0
#define TEST_MSG_TYPE_1 1
#define TEST_MSG_TYPE_2 2
#define TEST_MSG_TYPE_3 3
// app specific
#define TNET_MSG_TYPE_C2S_CHAT 4
#define TNET_MSG_TYPE_C2S_INFO 5
#define TNET_MSG_TYPE_S2C_CHAT 6
#define TNET_MSG_TYPE_S2C_CLIENT_LIST 7


// 1 byte for type, 2 bytes for size
#define MSG_TYPE_SIZE 1

// Something to pass around msg pointers...?
#if 0
struct MsgRef
{
    u8 type;
    u8* data;
    u16 numBytes;
};
#endif

// Test data objects to store
struct TestMsg1
{
    i32 member1;
    i32 member2;
    i32 member3;

    u16 MeasureForWriting()
    {
        return 3 * sizeof(i32) + MSG_TYPE_SIZE;
    }
    
    // TODO: Change Read/Write to Pack/Unpack. Copy raw struct once it has been
    // copied out of an incoming network message and into internal buffers
    // This will also make the message patchable when in the output buffer!
    u16 Write(u8* bytes)
    {
        u8* write = bytes;
        write += COM_WriteByte(TEST_MSG_TYPE_1, write);
        write += COM_WriteI32(member1, write);
        write += COM_WriteI32(member2, write);
        write += COM_WriteI32(member3, write);
        return (u16)(write - bytes);
    }

    u16 Read(u8* bytes)
    {
        u8* read = bytes;
        u8 type = COM_ReadByte(&read);
        Assert(type == TEST_MSG_TYPE_1);
        member1 = COM_ReadI32(&read);
        member2 = COM_ReadI32(&read);
        member3 = COM_ReadI32(&read);
        return (u16)(read - bytes);
    }
};

struct TestMsg2
{
    i32 member1;
    i32 member2;
    i32 pos[3];
    
    u16 MeasureForWriting()
    {
        return (5 * sizeof(i32)) + MSG_TYPE_SIZE;
    }
    
    u16 Write(u8* bytes)
    {
        u8* write = bytes;
        printf("Write type %d\n", TEST_MSG_TYPE_2);
        write += COM_WriteByte(TEST_MSG_TYPE_2, write);
        write += COM_WriteI32(member1, write);
        write += COM_WriteI32(member2, write);

        write += COM_WriteI32(pos[0], write);
        write += COM_WriteI32(pos[1], write);
        write += COM_WriteI32(pos[2], write);
        return (u16)(write - bytes);
    }

    u16 Read(u8* bytes)
    {
        u8* read = bytes;
        u8 type = COM_ReadByte(&read);
        Assert(type == TEST_MSG_TYPE_2);
        member1 = COM_ReadI32(&read);
        member2 = COM_ReadI32(&read);

        pos[0] = COM_ReadI32(&read);
        pos[1] = COM_ReadI32(&read);
        pos[2] = COM_ReadI32(&read);
        return (u16)(read - bytes);
    }
};

struct TestMsg3
{
    i32 member1;
    i32 member2;
};


struct MsgClientListHeader
{
    i32 clientPublicId;

};

struct MsgC2SChat
{
    u16 numChars;
    char message[256];
};

struct MsgS2CChat
{

};

u16 Msg_MeasureForReading(u8 type, u8* bytes)
{
    switch (type)
    {
        case TEST_MSG_TYPE_1:
		{
			// bleh, this is just meh
			TestMsg1 m;
			return m.MeasureForWriting();
		}
        case TEST_MSG_TYPE_2:
		{
			TestMsg2 m;
			return m.MeasureForWriting();
		}
        case TEST_MSG_TYPE_3:
		{
			ILLEGAL_CODE_PATH
			TestMsg1 m;
			return m.MeasureForWriting();
		}
        
        case TNET_MSG_TYPE_C2S_CHAT:
        {
            u16 strLength = COM_ReadU16(&bytes);
            return sizeof(u16) + strLength;
        } break;
        case TNET_MSG_TYPE_S2C_CHAT:
        {
            u16 strLength = COM_ReadU16(&bytes);
            return sizeof(u16) + strLength;
        } break;
    }
    ILLEGAL_CODE_PATH
    return 0;
}

u16 Msg_Write(u8 type, u8* bytes, u8* numBytes)
{
    ILLEGAL_CODE_PATH
    return 0;
}

u16 Msg_Read(u8 type, u8* bytes, u8* numBytes)
{
    ILLEGAL_CODE_PATH
    return 0;
}
