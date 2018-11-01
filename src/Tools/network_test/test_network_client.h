#pragma once

#include "test_network_win32.h"

void Client_ExecuteMessage(u8* ptr, u16 numBytes)
{
    u8 type = *ptr;
    switch (type)
    {
        case TEST_MSG_TYPE_1:
        {
            TestMsg1 msg = {};
            u16 msgSize = Msg_MeasureForReading(TEST_MSG_TYPE_1, ptr);
            if (numBytes != msgSize)
            {
                printf("  MSG TYPE 1 size mismatch: got %d but measured %d\n", numBytes, msgSize);
                ILLEGAL_CODE_PATH
            }
        } break;

        default:
        {
            printf("  Unknown MSG type %d\n", type);
            ILLEGAL_CODE_PATH
        } break;
    }
}

void Client_ExecuteInputStream(ReliableStream* stream)
{
    //i32 reading = 1;
    ByteBuffer* b = &stream->inputBuffer;
    //COM_PrintBytes(b->ptrStart, (u16)b->Written(), 64);
    for(;;)
    {
        u32 nextMsg = stream->inputSequence + 1;
        if (nextMsg == 0)
        {
            printf("ERROR: Next msg is 0!\n");
        }
        StreamMsgHeader* h = Stream_FindMessageById(b->ptrStart, b->ptrWrite, nextMsg);
        if (!h) { return; }

        // Execute input, advance sequence, Collapse buffer
        stream->inputSequence = nextMsg;

        printf("Client Exec message %d (%d bytes)\n", nextMsg, h->size);
		u8* msg = (u8*)h;
		msg += sizeof(StreamMsgHeader);
        Client_ExecuteMessage(msg, (u16)h->size);

        // Clear
        u8* blockStart = (u8*)h;
        u32 blockSize = sizeof(StreamMsgHeader) + h->size;
        //printf("Clearing %d bytes\n", blockSize);
        b->ptrWrite = Stream_CollapseBlock(blockStart, blockSize, b->ptrWrite);
    }
}

//////////////////////////////////////////////////////////////////////
// CLIENT
//////////////////////////////////////////////////////////////////////
void Test_ClientSendChatMessage(char* buf, u32 length)
{
    if (!g_network.isActive) { printf("Cannot send, network not active\n"); return; }
    ByteBuffer b = TNET_GetWriteBuffer();
    b.ptrWrite += COM_WriteByte(TNET_MSG_TYPE_C2S_CHAT, b.ptrWrite);
    b.ptrWrite += COM_WriteU16((u16)length, b.ptrWrite);
    u32 pos = 0;
    while (pos < length)
    {
        *b.ptrWrite = (u8)buf[pos];
        b.ptrWrite += sizeof(u8);
        pos++;
    }

    // Unreliable direct send in single packet:
    printf("\nSent %d chars\n", length);
    ZNet_SendData(g_network.server.connId, b.ptrStart, (u16)b.Written(), 0);

    // Buffer into reliable output:
    //Stream_WriteToOutput(&g_network.server.reliableStream, b.ptrStart, b.Written());
}

/*
Writing a packet:
> Requires packet sequence number to create transmission record
> WriteReliable messages, filling in transmission record message ids.
> Set reliable message bytes written in packet header.
> Write unreliable messages
> Set unreliable message bytes written in packet header
*/
void Client_Transmit()
{
    #if 0
    ByteBuffer b = TNET_GetWriteBuffer();
    u32 sequence = ZNet_GetNextSequenceNumber(g_network.server.connId);
    Stream_FindTransmissionRecord(g_transmissions, sequence);

    // step forward over header
    u16 reliableBytes = 0, unreliableBytes = 0;
    u8* headerWrite = b.ptrWrite;
    b.ptrWrite += sizeof(u16) * 2;

    // Write reliable messages
    reliableBytes = Stream_WriteReliableOutput(&g_network.server.reliableStream, b.ptrWrite, b.Space());
    b.ptrWrite += reliableBytes;

    // Write unreliable messages

    // Fill in header, send
    headerWrite += COM_WriteU16(reliableBytes, headerWrite);
    headerWrite += COM_WriteU16(unreliableBytes, headerWrite);
    u32 bytesWritten = b.Written();
    #endif
}

void Test_ClientSendState(i32 connId)
{
    ByteBuffer b = TNET_GetWriteBuffer();
    b.ptrWrite += COM_WriteByte(1, b.ptrWrite);
    u32 sendSequence = ZNet_SendData(connId, b.ptrStart, (u16)b.Written(), 0);
    //printf("Sent sequence %d\n", sendSequence);
}

void Test_ClientSendInfo()
{

}

// Keep seeing 83... wtf is keycode 83?
#define KEYCODE_ENTER 13
#define KEYCODE_BACKSPACE 8
#define KEYCODE_DELETE 224
#define KEYCODE_ESCAPE 27

void Test_Client(u16 serverPort, u16 clientPort)
{
    printf("Client\n");
    ZNet_Init(TNet_CreateNetFunctions(), TNet_CreateOutputInterface(), ZNET_SIM_MODE_BAD);
    g_network.Init();

    f32 tickRateSeconds = SERVER_TICK_RATE;
    i32 tickRateMS = (i32)(1000.0f / tickRateSeconds);
    
    ZNetAddress addr = {};
    addr.ip4Bytes[0] = 127;
    addr.ip4Bytes[1] = 0;
    addr.ip4Bytes[2] = 0;
    addr.ip4Bytes[3] = 1;
    addr.port = serverPort;
    g_network.isServer = 0;
    ZNet_StartSession(NETMODE_CLIENT, &addr, clientPort);

    // basic chat interface

    char chatMsg[256];
    COM_ZeroMemory((u8*)chatMsg, 256);
    i32 position = 0;

    char lineClearBuf[80];
    COM_SetMemory((u8*)lineClearBuf, 80, (u8)' ');
    lineClearBuf[79] = '\0';
    
    i32 running = 1;
    u32 ticks = 0;
    while(running)
    {
        //printf("Waiting for key at pos %d:\n", position);
        while (!_kbhit())
        {
            //system("cls");
            // update connections
            i32 error = ZNet_Tick(tickRateSeconds);
            // read
            Client_ExecuteInputStream(&g_network.server.reliableStream);
            
            if (g_network.server.connId != 0)
            {
                Test_ClientSendState(g_network.server.connId);
            }
            
            //printf("\r%d", ticks++);
            printf("\r%s", lineClearBuf);
            printf("\r%s", chatMsg);
            Sleep(tickRateMS);
        }
        int c = _getch();
        if (c == KEYCODE_ESCAPE)
        {
            running = false;
            printf("Exit");
            continue;
        }
        else if (c == KEYCODE_ENTER)
        {
            //printf("\rSAY: %s\n", chatMsg);
            
            if (position > 0)
            {
                if (chatMsg[0] == '/')
                {
                    // exec command
                    printf("EXEC COMMAND\n");
                }
                else
                {
                    // send chat
                    // + 1 to position to include null terminator
                    Test_ClientSendChatMessage(chatMsg, position + 1);
                }
                COM_ZeroMemory((u8*)chatMsg, 256);
                position = 0;
            }
        }
        else if (c == KEYCODE_BACKSPACE)
        {
            //printf("Delete char at %d\n", position);
            chatMsg[position] = '\0';
            if (position > 0)
            {
                position--;
            }
        }
        else if (c != '\0')
        {
            chatMsg[position] = (char)c;
            if (position < (256 - 1))
            {
                position++;
            }
        }
        //printf("\nYou pressed %c (keycode %d)\n", c, c);
    }
    // keep window open
    getc(stdin);
}
