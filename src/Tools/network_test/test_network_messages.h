#pragma once

#include "../../common/com_module.h"


#define TNET_MSG_TYPE_TEST 1
#define TNET_MSG_TYPE_C2S_CHAT 2
#define TNET_MSG_TYPE_C2S_INFO 3
#define TNET_MSG_TYPE_S2C_CHAT 4
#define TNET_MSG_TYPE_S2C_CLIENT_LIST 5

struct MsgTest
{
    u32 data;
};

struct MsgC2SChat
{
    u16 numChars;
    char message[256];
};

struct MsgS2CChat
{

};

u16 Net_MeasureMessageBytes(u8 type, u8* bytes)
{
    switch (type)
    {
        case TNET_MSG_TYPE_TEST:
        {
            return sizeof(MsgTest);
        } break;
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
