#pragma once

#include "app_network_types.h"
#include "app_network_stream.h"

#include "app_module.cpp" 

/////////////////////////////////////////////////////////////////
// Network Execute from read
/////////////////////////////////////////////////////////////////

u16 Net_ServerExecuteClientMessage(Client* cl, u8* bytes, u16 numBytes)
{
    //printf("SV Exec msg from %d: type %d size %d\n",
    //  cl->connectionId, *bytes, numBytes);
    u8 type = *bytes;
    u16 bytesRead = 0;
    if (type == CMD_TYPE_TEST)
    {
        Cmd_Test cmd;
        bytesRead = cmd.Read(bytes);
        if (numBytes != UINT16_MAX)
        {
            APP_ASSERT((bytesRead == numBytes),
                "SV Exec CL msg - Read() size mismatch")
        }
        // Don't actually try to execute this command type!
        //printf("SV Keep alive %d\n", cmd.data);
    }
    else if (type == CMD_TYPE_PLAYER_INPUT)
    {
        Cmd_PlayerInput cmd;
        bytesRead += cmd.Read(bytes);
        if (numBytes != UINT16_MAX)
        {
            APP_ASSERT((bytesRead == numBytes),
                "SV Exec CL msg - Read() size mismatch")
        }
        APP_WRITE_CMD(0, cmd);
    }
    else if (type == CMD_TYPE_IMPULSE)
    {
        Cmd_ServerImpulse cmd;
        bytesRead += cmd.Read(bytes);
        if (numBytes != UINT16_MAX)
        {
            APP_ASSERT((bytesRead == numBytes),
                "SV Exec CL msg - Read() size mismatch")
        }
        APP_WRITE_CMD(0, cmd);
    }
    return bytesRead;
}

/////////////////////////////////////////////////////////////////
// Network Read
// > Reliable commands are transfered to a stream input buffer
// then executed.
// > Unreliable commands are read and executed straight from the
// packet
/////////////////////////////////////////////////////////////////
internal void Net_ServerReadUnreliable(Client* cl, u8* ptr, u16 numBytes)
{
    u8* end = ptr + numBytes;
    u8* read = ptr;
    //printf("SV reading %d unreliable bytes for CL %d\n", numBytes, cl->connectionId);
    i32 escapeCounter = 0;
    while (read < end)
    {
        u8 type = *read;
        //printf("SV Exec unreliable cmd %d\n", type);
        u16 bytesRead = Net_ServerExecuteClientMessage(cl, read, UINT16_MAX);
        if (bytesRead > 0)
        {
            read += bytesRead;
        }
        else
        {
            break;
        }
        if (++escapeCounter > 9999) { break; }
    }
}

/////////////////////////////////////////////////////////////////
// Load Client avatar and input commands
/////////////////////////////////////////////////////////////////
internal i32 Net_ServerWriteClientUnreliable(
    Client* cl, GameSession* session, GameScene* gs, ByteBuffer* b)
{
    u8* start = b->ptrWrite;
    // iterate client list, load updates of other client avatars
    for (i32 i = 0; i < session->clientList.max; ++i)
    {
        Client* other = &session->clientList.items[i];
        if (other->entId.value == ENT_ID_NULL) { continue; }
        EC_Transform* etc = EC_FindTransform(gs, &other->entId);
        if (etc == NULL) { continue; }

        // TODO: Check for overflowing packet!
        Cmd_EntitySync cmd = {};
        cmd.entId = other->entId;
        cmd.pos.x = etc->t.pos.x;
        cmd.pos.y = etc->t.pos.y;
        cmd.pos.z = etc->t.pos.z;
        //printf("@");

        b->ptrWrite += COM_WriteU16(
                Stream_WritePacketHeader(0, cmd.MeasureForWriting()),
                b->ptrWrite);
        b->ptrWrite += cmd.Write(b->ptrWrite);
    }
    return (b->ptrWrite - start);
}

/////////////////////////////////////////////////////////////////
// Network Send
/////////////////////////////////////////////////////////////////

internal void Net_TransmitToClients(GameSession* session, GameScene* gs, GameTime* time)
{
    if(!IsRunningServer(session->netMode)) { return; }

    const i32 packetSize = 1024;
    u8 packetBytes[packetSize];
    ByteBuffer packetBuf = Buf_FromBytes(packetBytes, packetSize);
    i32 numTransmissions = 0;
    for (i32 i = 0; i < session->clientList.max; ++i)
    {
        Client* cl = &session->clientList.items[i];
        if (cl->state == CLIENT_STATE_FREE) { continue; }
        ByteBuffer* b = &cl->stream.outputBuffer;
        // TODO: Preventing transmit to local clients... this will have to loopback
        if (cl->connectionId == 0)
        {
            // Clear their output just in case
            Buf_Clear(&cl->stream.outputBuffer);
            continue;
        }
        // TODO: Sending once per tick regardless of framerate at the moment

        // sanitise
        Buf_Clear(&packetBuf);

        i32 numReliableBytes = b->Written();
		// Write reliable
        Stream_OutputToPacket(cl->connectionId, &cl->stream, &packetBuf, time->deltaTime);
		// sync check
		packetBuf.ptrWrite += COM_WriteU32(NET_DESERIALISE_CHECK, packetBuf.ptrWrite);
        
        u8* unreliableHeaderPos = packetBuf.ptrWrite;
        // skip header
        packetBuf.ptrWrite += sizeof(u16);
        // Write unreliable
        // TODO: This is where entity state will be transmitted
        i32 numUnreliableBytes = Net_ServerWriteClientUnreliable(
            cl, session, gs, &packetBuf);

        // write *something* for acking and keepalive
        if (numReliableBytes == 0 && numUnreliableBytes == 0)
        {
            // num bytes
            packetBuf.ptrWrite += COM_WriteU16(7, packetBuf.ptrWrite);
            // msg
            packetBuf.ptrWrite += COM_WriteU16(
                Stream_WritePacketHeader(0, 5),
                packetBuf.ptrWrite);
            Cmd_Test cmd = {};
            cmd.data = 1234;
            packetBuf.ptrWrite += cmd.Write(packetBuf.ptrWrite);
        }
        else
        {
            COM_WriteU16((u16)numUnreliableBytes, unreliableHeaderPos);
        }
        // Send
        ZNet_SendData(
            cl->connectionId, packetBuf.ptrStart, (u16)packetBuf.Written(), 0);
        numTransmissions++;
    }
    //printf("%d,", numTransmissions);
}
