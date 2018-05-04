#pragma once

#include "com_module.h"

static inline void BUF_Reset(ByteBuffer* b)
{
    b->ptrWrite = b->ptrStart;
    b->ptrEnd = b->ptrStart;
}
