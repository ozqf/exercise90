#pragma
/*
Read/Write packets for client
*/
#include "../../common/com_module.h"
#include "../packet.h"
#include "../../interface/sys_events.h"

internal i32 CL_ReadPacketReliableInput(ByteBuffer* buf, NetStream* stream)
{
    u8* read = buf->ptrStart;
    u8* end = buf->ptrWrite;
    while (read < end)
    {
        Command* h = (Command*)read;
        i32 err = Cmd_Validate(h);
        if (err != COM_ERROR_NONE)
        {
            return err;
        }
        Assert(!Cmd_Validate(h))
        read += h->size;
        Stream_EnqueueInput(stream, h);
    }
    return COM_ERROR_NONE;
}

internal i32 CL_ReadPacket(SysPacketEvent* ev, NetStream* reliableStream)
{
    // Create Descriptor
    i32 headerSize = sizeof(SysPacketEvent);
    i32 dataSize = ev->header.size - headerSize;
    u8* data = (u8*)(ev) + headerSize;
    printf("CL %d Packet bytes from %d\n", dataSize, ev->sender.port);

    PacketDescriptor p;
    i32 err = Packet_InitDescriptor(
        &p, data, dataSize);
	if (err != COM_ERROR_NONE)
	{
		printf("  Error %d deserialising packet\n", err);
		return COM_ERROR_DESERIALISE_FAILED;
	}
    printf("  Tick %d Time %.3f\n",
        p.transmissionSimFrameNumber,
        p.transmissionSimTime);
    
    // Read reliable section
    ByteBuffer reliableSection = {};
    printf("  Reliable bytes: %d\n", p.numReliableBytes);
    reliableSection.ptrStart = data + Packet_GetHeaderSize();
    reliableSection.ptrWrite = reliableSection.ptrStart + p.numReliableBytes;
    reliableSection.ptrEnd = reliableSection.ptrWrite;
    reliableSection.capacity = p.numReliableBytes;
    CL_ReadPacketReliableInput(&reliableSection, reliableStream);
    //Cmd_PrintBuffer(reliableSection.ptrStart, reliableSection.Written());

    return COM_ERROR_NONE;
}
