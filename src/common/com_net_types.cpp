#pragma once

#include "../common/com_module.h"

ZNetAddress COM_LocalHost(u16 port)
{
    ZNetAddress addr;
    addr.ip4Bytes[0] = 127;
    addr.ip4Bytes[1] = 0;
    addr.ip4Bytes[2] = 0;
    addr.ip4Bytes[3] = 1;
    addr.port = port;
    return addr;
}

void COM_InitStream(NetStream* stream, ByteBuffer input, ByteBuffer output)
{
    stream->inputBuffer = input;
    stream->outputBuffer = output;
    stream->inputSequence = 1;
}
