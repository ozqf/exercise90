#pragma once

#include "znet_module.cpp"

void ZNet_RunTests()
{
    printf("=== ZNet tests ===\n");
    ZNetConnection send, rec;
    COM_ZeroMemory((u8*)&send, sizeof(ZNetConnection));
    COM_ZeroMemory((u8*)&rec, sizeof(ZNetConnection));
    // pretend to have sent some packets
    u32 seq = 6;
    send.remoteSequence = seq;
    while (seq > 0)
    {
        ZNet_RecordPacketTransmission(&send, seq);
        seq--;
    }
    printf("Awaiting ack:\n");
    ZNet_PrintAwaitingAcks(&send);
    printf("\n");

    // pretend to receive some packets
    ZNet_RecordPacketForAck(&rec, 6);
    ZNet_RecordPacketForAck(&rec, 5);
    ZNet_RecordPacketForAck(&rec, 3);
    printf("Received:\n");
    ZNet_PrintReceived(&rec);
    printf("\n");
    u32 ackBits = ZNet_BuildAckBits(&rec, 6);
    printf("Ack bits:\n%u\n", ackBits);
    COM_PrintBits((i32)ackBits, 1);

    // Check acks would be reported:
    ZNet_CheckAcks(&send, 6, ackBits);
}

char* ZNet_WriteDebug(char* start, char* end)
{
    char* write = start;
    ZNet* net = &g_net;
    write += sprintf_s(write, (end - write),
        "--- ZNET LAYER ---\nState: %d, isListening %d, socket %d\n, selfPort: %d\nClient2ServId %d\n- CONNECTIONS -\n",
        net->state, net->isListening, net->socketIndex, net->selfPort, net->client2ServerId
    );
    i32 numConnections = MAX_CONNECTIONS;
    for (i32 i = 0; i < numConnections; ++i)
    {
        ZNetConnection* conn = &net->connections[i];
        if (conn->id == 0) { continue; }
        write += sprintf_s(write, (end - write),
            "ConnId %d, seq %d, remote seq: %d, Timeout: %.2f\n",
            conn->id, conn->sequence, conn->remoteSequence, conn->timeSinceLastMessage
        );
    }

    return write;
}
