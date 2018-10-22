#pragma once

#include "../../common/com_module.h"

#define CLIENT_APP_STATE_NULL 0
#define CLIENT_APP_STATE_LOADING 1
#define CLIENT_APP_STATE_IN_PLAY 2

#define MAX_TEST_CLIENT_NAME_LENGTH 32

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

// 1 byte for type, 2 bytes for size
#define MSG_TYPE_SIZE 1

// Test data objects to store
struct TestMsg1
{
    i32 member1;
    i32 member2;
    i32 member3;

    u16 Measure()
    {
        return 3 * sizeof(i32) + MSG_TYPE_SIZE;
    }
    
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
    
    u16 Measure()
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

#if 1
struct MsgClientListHeader
{
    i32 clientPublicId;

};

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
#endif

struct ReliableStream
{
    u32 inputSequence;
    u32 outputSequence;
    ByteBuffer inputBuffer;
    ByteBuffer outputBuffer;
};

struct TestClient
{
    // Ids are taken from ZNet layer
    i32 connId; 
    i32 publicClientId;

    i32 inUse;
    // client application state not involving
    // raw connection state. eg 'loading, inPlay' etc
    // or game related (alive, dead etc)
    i32 clientAppState;
    // name etc
	ClientInfo info;
    ReliableStream reliableStream;
};

struct TestServer
{
    i32 connId;
    i32 inUse;
    ReliableStream reliableStream;
};

#define MAX_TEST_CLIENTS 32
struct TestGameNetwork
{
    i32 isActive = 0;
    i32 isServer;
    TestServer server;
    // Clients still maintain the clients list as they want to know info about other players etc
    // however, connIds will all be blank as this is private to specific clients and the server
    TestClient clients[MAX_TEST_CLIENTS];
    i32 capacity = MAX_TEST_CLIENTS;
    i32 nextClientId = 0;
    i32 initialised = 0;

    void Init()
    {
        if (initialised != 0) { return; }
        initialised = 1;
        i32 bytesPerBuffer = KiloBytes(64);
        // Allocate stream buffers
        printf("Allocating server stream buffers\n");
        server.reliableStream = {};
        server.reliableStream.inputBuffer = Buf_FromMalloc(malloc(bytesPerBuffer), bytesPerBuffer);
        server.reliableStream.outputBuffer = Buf_FromMalloc(malloc(bytesPerBuffer), bytesPerBuffer);
        COM_ZeroMemory(server.reliableStream.inputBuffer.ptrStart, bytesPerBuffer);
        COM_ZeroMemory(server.reliableStream.outputBuffer.ptrStart, bytesPerBuffer);

        printf("Allocating %d client stream buffers\n", MAX_TEST_CLIENTS);
        for (i32 i = 0; i < MAX_TEST_CLIENTS; ++i)
        {
            TestClient* cl = &clients[i];
            cl->reliableStream = {};
            cl->reliableStream.inputBuffer = Buf_FromMalloc(malloc(bytesPerBuffer), bytesPerBuffer);
            cl->reliableStream.outputBuffer = Buf_FromMalloc(malloc(bytesPerBuffer), bytesPerBuffer);
            COM_ZeroMemory(cl->reliableStream.inputBuffer.ptrStart, bytesPerBuffer);
            COM_ZeroMemory(cl->reliableStream.outputBuffer.ptrStart, bytesPerBuffer);
        }
    }

    void Destroy()
    {
        if (!initialised) { return; }
        initialised = 0;
    }

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
                cl->publicClientId = newPublicId;
                
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
