#ifndef ZE_BLOB_STORE_H
#define ZE_BLOB_STORE_H

#include "../common/common.h"
#include "ze_lookup_table.h"
#include "ze_blob_array.h"

struct ZEBlobStore
{
    ZEBlobArray* array;
    ZELookupTable* lookup;
    
    i32 Count()
    {
        return array->m_numBlobs;
    }

    u8* GetFreeSlot(i32 id)
    {
        u8* mem = NULL;
        i32 index = -1;
        ErrorCode err = array->GetFreeSlot(&mem, &index, id);
        COM_ASSERT(err == COM_ERROR_NONE, "Could not get free blob slot\n");
        err = lookup->Insert(id, index);
        COM_ASSERT(err == COM_ERROR_NONE, "Could not insert lookup key\n");
        return mem;
    }

    ErrorCode MarkForRemoval(i32 id)
    {
        // Lookup key
        i32 index = lookup->FindData(id);
        COM_ASSERT(index != ZE_LT_INVALID_INDEX, "Blob store could not find lookup key\n");
        // Blob
        ErrorCode err = array->MarkForFree(index);
        COM_ASSERT(err == COM_ERROR_NONE, "Error marking blob for removal\n");
        return err;
    }

    void Truncate()
    {
        
    }
};

static void ZE_InitBlobStore(ZEBlobStore* store, i32 capacity, i32 sizePerObject, i32 invalidId)
{
    *store = {};
    i32 err = ZE_CreateBlobArray(&store->array, capacity, sizePerObject, invalidId);
    store->lookup = ZE_LT_Create(capacity * 2, invalidId, NULL);
}

#endif // ZE_BLOB_STORE_H