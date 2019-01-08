#pragma once

#include "com_module.h"

void COM_PrintBits(i32 val, u8 printNewLine);
void COM_PrintBytes(u8* bytes, i32 numBytes, i32 bytesPerRow);
void COM_PrintBytesHex(u8* bytes, u16 numBytes, i32 bytesPerRow);