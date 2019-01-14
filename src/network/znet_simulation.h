#pragma once

#include "znet_module.cpp"

struct ZNetLagSimInfo
{
    i32 mode;
    i32 minMS;
    i32 maxMS;
    f32 lossNormal;
    i32 randomIndex;

    i32 RollDropPacket()
    {
        //f32 num = COM_Randf32(&this->randomIndex);
        f32 num = COM_STDRandf32();
        //printf("Drop? %.2f vs %.2f\n", num, this->lossNormal);
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
	i32 inUse = 0;
    ZNetAddress address;
    i32 size;
    f32 tick;
};

struct ZNetDelayedPacketStore
{
    ZNetLagSimInfo info;
    ZNetDelayedPacketHeader* handles[256];

    void Init(i32 minLagMS, i32 maxLagMS, f32 normalisedPacketLossChance)
    {
        if (minLagMS > maxLagMS) { minLagMS = maxLagMS; }

        COM_ClampF32(&normalisedPacketLossChance, 0, 0.9f);
        COM_ClampI32(&minLagMS, 0, 500);
        COM_ClampI32(&maxLagMS, 0, 1000);

        COM_ZeroMemory((u8*)this, sizeof(ZNetDelayedPacketStore));
        info.minMS = minLagMS;
        info.maxMS = maxLagMS;
        info.lossNormal = normalisedPacketLossChance;
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

    void SendPacket(ZNet* net, ZNetAddress* address, u8* data, u16 numBytes)
    {
        if (this->info.RollDropPacket())
        {
            printf("GULP");
            return;
        }
        i32 i = this->GetFreeHandleIndex();
        if (i == -1)
        {
            printf("ZNet Sim: Slots full, sending directly\n");
            ZNet_SendActual(net, address, data, numBytes);
            return;
        }

        i32 space = sizeof(ZNetDelayedPacketHeader) + numBytes;
        ZNetDelayedPacketHeader* h = (ZNetDelayedPacketHeader*)malloc(space);
        h->address = *address;
        h->size = numBytes;
        h->tick = (f32)this->info.RollDelay() / 1000.0f;
        //printf("ZNet Sim: Packet delay: %.2f\n", h->tick);
        handles[i] = h;
        u8* ptr = (u8*)h + sizeof(ZNetDelayedPacketHeader);
        COM_COPY(data, ptr, numBytes);
    }

    void Tick(ZNet* net, f32 deltaTime)
    {
        for (i32 i = 0; i < 256; ++i)
        {
            ZNetDelayedPacketHeader* h = this->handles[i];
            if (h == NULL) { continue; }
            h->tick -= deltaTime;
            if (h->tick > 0) { continue; }
            u8* ptr = (u8*)h + sizeof(ZNetDelayedPacketHeader);
            ZNet_SendActual(net, &h->address, ptr, (u16)h->size);
            this->FreeHandle(i);
        }
    }
};
