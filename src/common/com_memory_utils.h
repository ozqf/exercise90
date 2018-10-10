#pragma once

#include "com_defines.h"


static inline u32 SafeTruncateUInt64(u64 value);

static inline u32 COM_CopyMemory(u8* source, u8* target, u32 numBytes);
static inline void COM_ZeroMemory(u8 *ptr, u32 numBytes);
static inline void COM_SetMemory(u8 *ptr, u32 numBytes, u8 val);
/**
 * Repeatedly write the given pattern into ptr
 * Pattern size must fit exactly into numBytes!
 */
static inline void COM_SetMemoryPattern(u8* ptr, u32 numBytes, u8* pattern, u32 patternSize);
static inline void COM_DebugFillMemory(u8 *ptr, u8 value, u32 numBytes);
/**
Returns <value> enlarged to match the alignment specified.
eg value = 52, align = 32 == 64, 
52 mod 32 = 20
32 - 20 = 12
52 + 12 = 64

but
64 mod 32 = 0
32 - 0 = 32
64 + 32 = 96

*/
static inline u32 COM_AlignSize(u32 value, u32 alignment);
