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
