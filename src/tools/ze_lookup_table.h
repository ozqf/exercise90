#ifndef ZE_LOOKUP_TABLE_H
#define ZE_LOOKUP_TABLE_H

#include "../common/common.h"

#define ZE_LT_INVALID_INDEX -1
#define ZE_LT_INVALID_ID 0

#ifndef ZE_LT_MALLOC
#define ZE_LT_MALLOC(numBytesToAlloc) \
(u8*)malloc(##numBytesToAlloc##)
#endif

#ifndef ZE_LT_FREE
#define ZE_LT_FREE(ptrToFree) \
free(##ptrToFree##)
#endif

#ifndef ZE_LT_SET_ZERO
#define ZE_LT_SET_ZERO(ptrToSet, numBytesToZero) \
memset(##ptrToSet##, 0, numBytesToZero##)
#endif

// uncomment this for TESTING ONLY
#define ZE_LT_USE_BAD_HASH_FUNCTION

#ifndef ZE_LT_USE_BAD_HASH_FUNCTION
// Credit: Thomas Wang
// https://burtleburtle.net/bob/hash/integer.html
static u32 ZE_LT_HashUint(u32 a)
{
	a = (a ^ 61) ^ (a >> 16);
	a = a + (a << 3);
    a = a ^ (a >> 4);
    a = a * 0x27d4eb2d;
    a = a ^ (a >> 15);
    return a;
}
#endif

// DO NOT USE!
// This is a deliberately broken hash for testing
// hash collision resolution!
#ifdef ZE_LT_USE_BAD_HASH_FUNCTION
static u32 ZE_LT_HashUint(u32 a)
{
	return 1;
}
#endif

struct ZELookupKey
{
    i32 id;
    i32 data;
    u32 idHash;
    i32 collisionsOnInsert;
};

struct ZELookupTable
{
    ZELookupKey* m_keys;
    i32 m_maxKeys;

    void StepKeyIndex(i32* index)
    {
        *index += 1;
        if (*index >= m_maxKeys)
        {
            *index = 0;
        }
    }

    void Clear()
    {
        for (i32 i = 0; i < m_maxKeys; ++i)
        {
            //m_keys[i].id = ZE_LT_INVALID_ID;
            m_keys[i] = {};
        }
    }

    i32 Insert(i32 id, i32 data)
    {
        u32 idHash = ZE_LT_HashUint(id);
        i32 keyIndex = idHash % m_maxKeys;
        i32 escape = 0;
        u32 numCollisions = 0;
        do
        {
            ZELookupKey* key = &m_keys[keyIndex];
            if (key->id == ZE_LT_INVALID_ID)
            {
                // insert
                key->id = id;
                key->idHash = idHash;
                key->data = data;
                key->collisionsOnInsert = numCollisions;
                return COM_ERROR_NONE;
            }
            else if (key->id == id)
            {
                // already inserted. Update data
                key->data = data;
                return COM_ERROR_NONE;
            }
            else
            {
                numCollisions++;
                StepKeyIndex(&keyIndex);
            }
            
        } while (escape++ < m_maxKeys);
        return COM_ERROR_FUNC_RAN_AWAY;
    }
};

static i32 ZE_LT_CalcBytesForTable(i32 numKeys)
{
    i32 bytesForKeys = sizeof(ZELookupKey) * numKeys;
    i32 bytesTotal = sizeof(ZELookupTable) + bytesForKeys;
    return bytesTotal;
}

static ZELookupTable* ZE_LT_Create(i32 capacity, i32 scale)
{
    if (scale < 0) { scale = 2; }
    capacity = capacity * scale;

    i32 numBytes = ZE_LT_CalcBytesForTable(capacity);
    u8* mem = (u8*)ZE_LT_MALLOC(numBytes);
    ZELookupTable* table = (ZELookupTable*)mem;
    table->m_maxKeys = capacity;
    table->m_keys = (ZELookupKey*)(mem + sizeof(ZELookupTable));
    table->Clear();
    return table;
}

#endif // ZE_LOOKUP_TABLE_H