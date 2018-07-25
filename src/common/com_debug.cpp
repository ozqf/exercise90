#pragma once

#include "com_module.cpp"

inline void COM_PrintBits(i32 val, u8 printNewLine)
{
    for (i32 i = 31; i >= 0; --i)
    {
        i32 result = 1 & (val >> i);
        printf("%d", result);
    }
    if (printNewLine)
    {
        printf("\n");
    }
}
