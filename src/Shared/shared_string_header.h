#pragma once

#include "shared_types.h"

i32 ZSTR_WriteChars(ZStringHeader* str, char* sourceChars, i32 numChars)
{
    i32 index = 0;
    while (numChars > 0 && str->length < str->maxLength)
    {
        str->chars[str->length++] = sourceChars[index++];
    }
    return index;
}
