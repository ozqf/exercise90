#ifndef ZE_BLOB_STORE_H
#define ZE_BLOB_STORE_H

#include "../common/common.h"
#include "ze_lookup_table.h"
#include "ze_blob_array.h"

struct ZEBlobStore
{
    ZEBlobArray* m_array;
    ZELookupTable* m_lookup;
    
    i32 Count()
    {
        return m_array->m_numBlobs;
    }

    u8* GetById(i32 id)
    {
        i32 index = m_lookup->FindData(id);
        if (index == ZE_LT_INVALID_INDEX) { return NULL; }
        return m_array->GetByIndex(index);
    }

    u8* GetByIndex(i32 index)
    {
        return m_array->GetByIndex(index);
    }

    u8* GetFreeSlot(i32 id)
    {
        u8* mem = NULL;
        i32 index = -1;
        ErrorCode err = m_array->GetFreeSlot(&mem, &index, id);
        COM_ASSERT(err == COM_ERROR_NONE, "Could not get free blob slot\n");
        err = m_lookup->Insert(id, index);
        COM_ASSERT(err == COM_ERROR_NONE, "Could not insert lookup key\n");
        return mem;
    }

    ErrorCode MarkForRemoval(i32 id)
    {
        // Lookup key
        i32 index = m_lookup->FindData(id);
        COM_ASSERT(index != ZE_LT_INVALID_INDEX, "Blob store could not find lookup key\n");
        // Blob
        ErrorCode err = m_array->MarkForFree(index);
        COM_ASSERT(err == COM_ERROR_NONE, "Error marking blob for removal\n");
        return err;
    }
	
	i32 ClearEntireStore()
	{
		printf("Clear entire store\n");
		for (i32 i = 0; i < m_array->m_numBlobs; ++i)
		{
			ZEBlobHeader* h = m_array->GetHeaderByIndex(i);
			m_lookup->Remove(h->id);
			m_array->ClearHeader(h);
		}
		i32 numDeletes = m_array->m_numBlobs;
		m_array->m_numBlobs = 0;
		return numDeletes;
	}

    /**
     * Scans over store and removes items marked for removal
     * > Items removed will be copied over from the end of the store
     * so items will move around!
     * > Must update the lookup table to keep it in-sync
     */
    i32 Truncate()
    {
        i32 numDeletes = 0;
        i32 i = m_array->m_numBlobs - 1;
        ErrorCode err;
        i32 swapIndex = m_array->FindLastOccupiedSlot(ZE_BA_INVALID_INDEX);
        if (swapIndex == ZE_BA_INVALID_INDEX)
        {
            // No slots in the array are in use. Clear everything
            return ClearEntireStore();
        }
        i32 escape = 0;

        while (i >= 0)
        {
            if (escape++ == m_lookup->m_maxKeys)
            {
                printf("Truncate ran away!\n");
                return numDeletes;
            }
            ZEBlobHeader* blob = m_array->GetHeaderByIndexUnchecked(i);
            if (blob->status == ZE_BA_STATUS_OCCUPIED)
            {
                // Record swap index if necessary
                //if (i > swapIndex) { swapIndex = i; }
                i--;
                continue;
            }
            // only valid values in this section of the array are occupied or recycle.
            COM_ASSERT(blob->status == ZE_BA_STATUS_RECYCLE,
                "Blob store: Free blob found during truncate!")
            
            // k time to delete a blob
            i32 lastIndex = m_array->m_numBlobs - 1;
            if (i == lastIndex)
            {
                // either only item in the list, or the last item in the list.
                // just collapse and move on
                err = m_lookup->Remove(blob->id);
                COM_ASSERT(err == COM_ERROR_NONE, "Blob store - Error removing lookup key")

                m_array->ClearHeader(blob);
                m_array->m_numBlobs--;
                // done
                i--;
            }
            else
            {
                // Swap with a valid item
                swapIndex = m_array->FindLastOccupiedSlot(ZE_BA_INVALID_INDEX);
				if (swapIndex == ZE_BA_INVALID_INDEX)
				{
					// nothing to swap with. Clear entire store
					return numDeletes + ClearEntireStore();
				}
				ZEBlobHeader* swapBlob = m_array->GetHeaderByIndexUnchecked(swapIndex);
                COM_ASSERT(swapBlob->status == ZE_BA_STATUS_OCCUPIED, "Attempting to swap unoccupied blob")

                // store Ids before copy!
                i32 swapId = swapBlob->id;
                i32 copiedOverId = blob->id;

                m_array->CopyOver(swapIndex, i);
                m_array->ClearHeader(swapBlob);
                m_array->m_numBlobs--;
                
                // update lookup table
                m_lookup->Remove(copiedOverId);
                m_lookup->SetData(swapId, i);
                i--;
            }
        }
        return numDeletes;
    }
};

static void ZE_InitBlobStore(ZEBlobStore* store, i32 capacity, i32 sizePerObject, i32 invalidId)
{
    *store = {};
    i32 err = ZE_CreateBlobArray(&store->m_array, capacity, sizePerObject, invalidId);
    store->m_lookup = ZE_LT_Create(capacity * 2, invalidId, NULL);
}

#endif // ZE_BLOB_STORE_H