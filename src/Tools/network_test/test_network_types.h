#pragma once

#include "../../common/com_module.h"

#define MAX_TEST_CLIENT_NAME_LENGTH 32
struct TestClient
{
    // should be kept private between this specific client and server
    i32 connId; 
    // public client id is how the game and other clients can refer to each other.
    // eg players 0, 1, 2. but their actual *connection* ids are not exposed to other clients.
    // this will require a 'game info' packet to tell the client what there specific public id is.
    i32 publicClientId;
    i32 inUse;
	char* name[MAX_TEST_CLIENT_NAME_LENGTH];
};

struct TestServer
{
    i32 connId;
    i32 inUse;
};
#if 0
struct MsgClientListHeader
{
    i32 clientPublicId;

};

struct MsgClientInfo
{
    u8 colourRGB[3];
    u8 nameLength;
    char name[32];

    i32 Read(u8* ptr)
    {
        this->colourRGB[0] = COM_ReadByte(&read);
        this->colourRGB[1] = COM_ReadByte(&read);
        this->colourRGB[2] = COM_ReadByte(&read);
        this->nameLength = COM_ReadByte(&read);
        if (nameLength > 32)
        {
            nameLength = 32;
        }
        ptr += COM_CopyStringLimited_Safe((char*)ptr, this->name, 32);
    }

    i32 Write(u8* ptr)
    {
        ptr += COM_WriteByte(this->colourRGB[0], ptr);
        ptr += COM_WriteByte(this->colourRGB[1], ptr);
        ptr += COM_WriteByte(this->colourRGB[2], ptr);
        i32 chars = COM_StrLenPlusTerminator(this->name);
        this->nameLength = chars;
        ptr += COM_WriteByte(chars, ptr);
        ptr += COM_COPY(this->name, ptr, chars);
    }
};
#endif
#define MAX_TEST_CLIENTS 32
struct TestGameNetwork
{
    i32 isServer;
    TestServer server;
    // Clients still maintain the clients list as they want to know info about other players etc
    // however, connIds will all be blank as this is private to specific clients and the server
    TestClient clients[MAX_TEST_CLIENTS];
    i32 capacity = MAX_TEST_CLIENTS;
    i32 nextClientId = 0;

    void SetServer(i32 id)
    {
        this->server.connId = id;
        this->server.inUse = 0;
    }

    TestClient* GetClientByPublicId(i32 publicId)
    {
        for (i32 i = 0; i < this->capacity; ++i)
        {
            if (this->clients[i].publicClientId == publicId)
            {
                return &this->clients[i];
            }
        }
        return NULL;
    }

    TestClient* GetClient(i32 id)
    {
        for (i32 i = 0; i < this->capacity; ++i)
        {
            if (this->clients[i].connId == id)
            {
                return &this->clients[i];
            }
        }
        return NULL;
    }

    // If this system is the server, then public id will be ignored
    // if this system is a client, then id should be zero if
    // the new client is NOT the client on this machine
    TestClient* AssignNewClient(i32 id, i32 newPublicId)
    {
        TestClient* cl = this->GetClient(id);
        if (cl) { return cl; }
        for (i32 i = 0; i < this->capacity; ++i)
        {
            cl = &this->clients[i];
            if (!cl->inUse)
            {
                cl->inUse = 1;
                cl->connId = id;
                if (this->isServer)
                {
                    cl->publicClientId = this->nextClientId++;
                }
                else
                {
                    cl->publicClientId = newPublicId;
                }
                
                
                return cl;
            }
        }
        return NULL;
    }

    void DeleteClient(i32 id)
    {
        for (i32 i = 0; i < this->capacity; ++i)
        {
            if (this->clients[i].connId == id)
            {
                this->clients[i].inUse = 0;
                this->clients[i].connId = 0;
            }
        }
    }

    void PrintfDebug()
    {
        i32 emptySlots = 0;
        printf("-- TEST NETWORK --\n");
        printf("- Server -\n");
        printf("%d\n", this->server.connId);

        printf("- Clients -\n");
        for (i32 i = 0; i < this->capacity; ++i)
        {
            TestClient* cl = &this->clients[i];
            if (!cl->inUse) { emptySlots++; continue; }
            printf("%d\n", cl->connId);
        }
        printf("  (%d unused slots\n", emptySlots);
    }
};
