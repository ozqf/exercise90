#pragma once

#include "../common/com_module.h"

#define SYS_EVENT_SENTINEL 0xDEADBEEF
#define SYS_EVENT_NULL 0
#define SYS_EVENT_INPUT 1
#define SYS_EVENT_PACKET 2

#define SYS_CAST_EVENT_TO_BASE(sysEvPtr) ((SysEvent*)##sysEvPtr##)

// place at top of all event structs
struct SysEvent
{
    i32 sentinel;
    i32 type;
    i32 size;
};

struct SysInputEvent
{
    SysEvent header;
    u32 inputID = 0;
    i32 value = 0;
};

static void Sys_PrepareEvent(SysEvent* ev, i32 type, i32 size)
{
    ev->sentinel = SYS_EVENT_SENTINEL;
    ev->type = type;
    ev->size = size;
}

static i32 Sys_ValidateEvent(SysEvent* ev)
{
    if (ev == NULL || ev->type == SYS_EVENT_NULL || ev->size <= 0)
    {
        return COM_ERROR_NULL_ARGUMENT;
    }
    return COM_ERROR_NONE;
}

static void Sys_EnqueueEvent(ByteBuffer* buf, SysEvent* ev)
{
    Assert(Sys_ValidateEvent(ev) == COM_ERROR_NONE)
    Assert(buf->Space() >= ev->size)
    //printf("SYS Enqueue ev %d size %d\n", ev->type, ev->size);
    buf->ptrWrite += COM_COPY(ev, buf->ptrWrite, ev->size);
}


////////////////////////////////////////////////////
// Concrete event types
////////////////////////////////////////////////////
static void Sys_CreateInputEvent(SysInputEvent* ev, u32 inputID, i32 value)
{
    Sys_PrepareEvent(
        SYS_CAST_EVENT_TO_BASE(ev),
        SYS_EVENT_INPUT,
        sizeof(SysInputEvent));
    ev->inputID = inputID;
    ev->value = value;
}

static void Sys_WriteInputEvent(ByteBuffer* b, u32 inputID, i32 value)
{
    SysInputEvent ev = {};
    Sys_CreateInputEvent(&ev, inputID, value);
    Sys_EnqueueEvent(b, SYS_CAST_EVENT_TO_BASE(&ev));
}
