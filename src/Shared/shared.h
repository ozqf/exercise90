/*******************************************
 * Shared - functions/types/utilities shared
 * between sub-components of application
 ******************************************/
#pragma once

#include "shared_types.h"


void COM_CopyMemory(u8* source, u8* target, u32 numBytes);

void COM_ZeroMemory(u8 *ptr, u32 numBytes);

void COM_SetMemory(u8 *ptr, u32 numBytes, u8 val);

void COM_SetMemoryPattern(u8* ptr, u32 numBytes, u8* pattern, u32 patternSize);

void COM_DebugFillMemory(u8 *ptr, u8 value, u32 numBytes);

u32 COM_AlignSize(u32 value, u32 alignment);

#include "shared_string_header.h"
#include "Maths/shared_maths.h"
#include "Memory/shared_memory_utils.h"
#include "shared_string_utils.h"
#include "blit_types.h"
#include "shared_assets.h"
#include "shared_render_types.h"
#include "shared_render_types.cpp"
#include "shared_input.h"
