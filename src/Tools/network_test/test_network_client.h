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
            msg.Read(ptr);
            //printf("CL exec 1: %d, %d, %d\n", msg.member1, msg.member2, msg.member3);
        } break;

        default:
        {
            printf("  Unknown MSG type %d\n", type);
            ILLEGAL_CODE_PATH
        } break;
    }
}

void Client_ReadInput(ReliableStream* stream)
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
        // No pending message?
        if (!h) { return; }

        // Execute input, advance sequence, Collapse buffer
        stream->inputSequence = nextMsg;

        //printf("Client Exec message %d (%d bytes)\n", nextMsg, h->size);
		u8* msg = (u8*)h;
		msg += sizeof(StreamMsgHeader);
        //printf("CL exec msg %d from SV\n", nextMsg);
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

    ReliableStream* stream = &g_network.server.reliableStream;
    ByteBuffer* b = &stream->outputBuffer;

    MsgC2SChat msg = {};
    msg.numChars = (u16)COM_CopyStringLimited(buf, msg.message, 256);
    //u32 size = msg.Write(b->ptrWrite);
    //b->ptrWrite += size;
    //Buf_WriteStreamMsgHeader(b, TNET_MSG_TYPE_C2S_CHAT, size);
    
    u16 msgSize = msg.MeasureForWriting();
    printf("CL Adding msg (%d bytes) to output: \"%s\"\n", msgSize, msg.message);
    b->ptrWrite += Stream_WriteStreamMsgHeader(b->ptrWrite, ++stream->outputSequence, msgSize);
    b->ptrWrite += msg.Write(b->ptrWrite);

    //printf("Manifest with chat msg:\n");
    //Stream_PrintBufferManifest(b);
}

/*
Writing a packet:
> Requires packet sequence number to create transmission record
> WriteReliable messages, filling in transmission record message ids.
> Set reliable message bytes written in packet header.
> Write unreliable messages
> Set unreliable message bytes written in packet header
*/
void CL_TransmitOutput(TestGameNetwork* net)
{
    Stream_OutputToPacket(
        net,
        net->server.connId,
        &net->server.reliableStream,
        g_dataBuffer,
        DATA_BUFFER_SIZE
    );
}

/**
 * Write frame messages to output buffer
 */
void CL_WriteOutput(ReliableStream* stream)
{
    //printf("CL Send\n");

    TestMsg1 m1 = {};
	    m1.member1 = COM_GetI32Sentinel();
	    m1.member2 = COM_GetI32Sentinel();
	    m1.member3 = COM_GetI32Sentinel();
    
    Buf_WriteStreamMsgHeader(
        &stream->outputBuffer,
        ++stream->outputSequence,
        m1.MeasureForWriting());
    stream->outputBuffer.ptrWrite += m1.Write(stream->outputBuffer.ptrWrite);

    
    
    #if 0
    ByteBuffer b = TNET_GetWriteBuffer();
    b.ptrWrite += COM_WriteByte(1, b.ptrWrite);
    u32 sendSequence = ZNet_SendData(connId, b.ptrStart, (u16)b.Written(), 0);
    //printf("Sent sequence %d\n", sendSequence);
    #endif
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
            Client_ReadInput(&g_network.server.reliableStream);
            
            if (g_network.server.connId != 0)
            {
                CL_WriteOutput(&g_network.server.reliableStream);
                CL_TransmitOutput(&g_network);
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
