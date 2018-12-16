#pragma once

#include "app_module.cpp"

#define CLIENT_APP_STATE_NULL 0
#define CLIENT_APP_STATE_LOADING 1
#define CLIENT_APP_STATE_IN_PLAY 2

#define MAX_TEST_CLIENT_NAME_LENGTH 32

struct ClientInfo
{
    u8 colourRGB[3];
    u8 nameLength;
    char name[MAX_TEST_CLIENT_NAME_LENGTH];

    i32 Read(u8* ptr)
    {
        this->colourRGB[0] = COM_ReadByte(&ptr);
        this->colourRGB[1] = COM_ReadByte(&ptr);
        this->colourRGB[2] = COM_ReadByte(&ptr);
        this->nameLength = COM_ReadByte(&ptr);
        if (nameLength > MAX_TEST_CLIENT_NAME_LENGTH)
        {
            nameLength = MAX_TEST_CLIENT_NAME_LENGTH;
        }
        ptr += COM_CopyStringLimited((char*)ptr, this->name, MAX_TEST_CLIENT_NAME_LENGTH);
    }

    i32 Write(u8* ptr)
    {
        ptr += COM_WriteByte(this->colourRGB[0], ptr);
        ptr += COM_WriteByte(this->colourRGB[1], ptr);
        ptr += COM_WriteByte(this->colourRGB[2], ptr);
        u8 chars = (u8)COM_StrLen(this->name) + 1; // + 1 for terminator;
        this->nameLength = chars;
        ptr += COM_WriteByte(chars, ptr);
        ptr += COM_COPY(this->name, ptr, chars);
    }
};
