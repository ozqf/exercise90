#pragma once

#include "znet_module.cpp"

struct ZNetLagSimInfo
{
    i32 minMS;
    i32 maxMS;
    f32 lossNormal;
    i32 randomIndex;

    i32 RollDropPacket()
    {
        f32 num = COM_Randf32(&this->randomIndex);
        return (num < lossNormal);
    }

    i32 RollDelay()
    {
        f32 num = COM_Randf32(&this->randomIndex);
        return (i32)((f32)(maxMS - minMS) * num) + minMS;
    }
};

struct ZNetDelayedPacketHeader
{
    i32 connId;
    i32 size;
    f32 tick;
};

struct ZNetDelayedPacketStore
{
    ZNetLagSimInfo info;
    ZNetDelayedPacketHeader* handles[256];

    void Init()
    {
        COM_ZeroMemory((u8*)this, sizeof(ZNetDelayedPacketStore));
    }

    i32 GetFreeHandleIndex()
    {
        for (i32 i = 0; i < 256; ++i)
        {
            if (this->handles[i] == 0)
            {
                return i;
            }
        }
        return -1;
    }

    void FreeHandle(i32 i)
    {
        if (this->handles[i] == 0) { return; }
        ZNetDelayedPacketHeader* h = this->handles[i];
        this->handles[i] = NULL;
        free((void*)h);
    }

    void Destroy()
    {
        for (i32 i = 0; i < 256; ++i)
        {
            this->FreeHandle(i);
        }
    }

    void SendPacket(ZNetAddress* address, u8* data, u16 numBytes)
    {
        if (this->info.RollDropPacket()) { return; }
        i32 i = this->GetFreeHandleIndex();
        if (i == -1)
        {
            ZNet_SendData(connId, data, numBytes);
            return;
        }

        i32 space = sizeof(ZNetDelayedPacketHeader) + numBytes;
        ZNetDelayedPacketHeader* h = (ZNetDelayedPacketHeader*)malloc(space);
        h->connId = connId;
        h->size = numBytes;
        h->tick = (f32)this->info.RollDelay() / 1000.0f;
        handles[i] = h;
        u8* ptr = (u8*)h + sizeof(ZNetDelayedPacketHeader);
        COM_COPY(data, ptr, numBytes);
    }

    void Tick(f32 deltaTime)
    {
        for (i32 i = 0; i < 256; ++i)
        {
            ZNetDelayedPacketHeader* h = this->handles[i];
            if (h->connId == 0) { continue; }
            h->tick -= deltaTime;
            if (h->tick > 0) { continue; }
            u8* ptr = (u8*)h + sizeof(ZNetDelayedPacketHeader);
            ZNet_SendData(h->connId, ptr, (u16)h->size);
            this->FreeHandle(i);
        }
    }
};
