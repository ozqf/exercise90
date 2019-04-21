#pragma once

#include "common.h"

void COM_PrintBits(i32 val, u8 printNewLine);
void COM_PrintBytes(u8* bytes, i32 numBytes, i32 bytesPerRow);
void COM_PrintBytesHex(u8* bytes, i32 numBytes, i32 bytesPerRow);
