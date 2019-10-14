#ifndef ZE_BLOB_ARRAY_H
#define ZE_BLOB_ARRAY_H

#include "../common/common.h"
#include "string.h"

#define ZE_BA_INVALID_INDEX -1

#ifndef ZE_BA_MALLOC
#define ZE_BA_MALLOC(numBytesToAlloc) \
(u8*)malloc(##numBytesToAlloc##)
#endif

#ifndef ZE_BA_FREE
#define ZE_BA_FREE(ptrToFree) \
free(##ptrToFree##)
#endif

#ifndef ZE_BA_SET_ZERO
#define ZE_BA_SET_ZERO(ptrToSet, numBytesToZero) \
memset(##ptrToSet##, 0, numBytesToZero##)
#endif

#ifndef ZE_BA_COPY
#define ZE_BA_COPY(destBlobPtr, sourceBlobPtr, blobSize) \
memcpy((void*)##destBlobPtr##, (void*)##sourceBlobPtr##, blobSize##);
#endif

#define ZE_BA_STATUS_FREE 0
#define ZE_BA_STATUS_IN_USE 1
#define ZE_BA_STATUS_INVALID 2

struct ZEBlobHeader
{
    i32 id;
    // if not marked as valid, this is an 'empty' slot but not available for reassignment yet
    // valid items are slots safe for truncation.
    i32 valid;
    // TODO: sentinel is not checked as when GetByIndex runs for a free blob,
    // the sentinel is not set yet!
    i32 sentinel;
};

struct ZEBlobArray
{
    u8* m_blobs;
    u8* m_end;
    i32 m_totalBlobSize;
    i32 m_blobUserSize;
    i32 m_maxBlobs;
    i32 m_numBlobs;
    i32 m_invalidId;

    ///////////////////////////////////////////////////
    // Get headers
    ///////////////////////////////////////////////////
    ZEBlobHeader* GetHeaderByIndex(i32 index)
    {
        COM_ASSERT((index >= 0 && index < m_numBlobs), "Blob array out of bounds");
        i32 offset = m_totalBlobSize * index;
        ZEBlobHeader* result = (ZEBlobHeader*)(m_blobs + offset);
        COM_ASSERT(result->sentinel == COM_SENTINEL, "Blob GetByIndex sentinel failed")
        return result;
    }

    ZEBlobHeader* GetHeaderByIndexUnchecked(i32 index)
    {
        i32 offset = m_totalBlobSize * index;
        ZEBlobHeader* result = (ZEBlobHeader*)(m_blobs + offset);
        return result;
    }

    ///////////////////////////////////////////////////
    // Get users slots
    ///////////////////////////////////////////////////

    u8* GetByIndexUnchecked(i32 index)
    {
        ZEBlobHeader* h = GetHeaderByIndexUnchecked(index);
        return (u8*)h + sizeof(ZEBlobHeader);
    }

    u8* GetByIndex(i32 index)
    {
        COM_ASSERT((index >= 0 && index < m_numBlobs), "Blob array out of bounds");
        i32 offset = m_totalBlobSize * index;
        ZEBlobHeader* h = GetHeaderByIndexUnchecked(index);
        COM_ASSERT(h->sentinel == COM_SENTINEL, "Blob GetByIndex sentinel failed")
        return (u8*)h + sizeof(ZEBlobHeader);
    }

    ErrorCode GetFreeSlot(u8** result, i32 id)
    {
        if (result == NULL) { return COM_ERROR_NULL_ARGUMENT; }
        if (id == m_invalidId) { return COM_ERROR_BAD_ARGUMENT; }
        if (m_numBlobs == m_maxBlobs) { return COM_ERROR_NO_SPACE; }

        i32 newBlobIndex = m_numBlobs++;

        i32 offset = m_totalBlobSize * newBlobIndex;
        ZEBlobHeader* header = (ZEBlobHeader*)(m_blobs + offset);
        header->id = id;
        header->sentinel = COM_SENTINEL;
        header->valid = YES;
        *result = (u8*)header + sizeof(ZEBlobHeader);
        return COM_ERROR_NONE;
    }

    void MarkFree(ZEBlobHeader* h)
    {
        h->id = m_invalidId;
        h->valid = NO;
        h->sentinel = 0;
    }

    ErrorCode MarkForFree(i32 index)
    {
        if (index >= m_numBlobs) { return COM_ERROR_OUT_OF_BOUNDS; }
        ZEBlobHeader* header = GetHeaderByIndex(index);
        if (header == NULL) { return COM_ERROR_NOT_FOUND; }
        header->valid = NO;
        return COM_ERROR_NONE;
    }

    void CopyOver(i32 sourceIndex, i32 destIndex)
    {
        u8* sourcePtr = m_blobs + (m_totalBlobSize * sourceIndex);
        u8* destPtr = m_blobs + (m_totalBlobSize * destIndex);
        ZE_BA_COPY(destPtr, sourcePtr, m_totalBlobSize)
    }
    
    void Truncate()
    {
        i32 i = 0;
        while (i < m_numBlobs)
        {
            // if blob is not valid, replace it with the last in the array
            // and reduce the blob count
            ZEBlobHeader* blob = GetHeaderByIndex(i);
            if (blob->valid == YES) { i++; continue; }
            
            if (m_numBlobs == 1)
            {
                // no blob to replace this with, just clear list
                MarkFree(GetHeaderByIndexUnchecked(i));
                m_numBlobs = 0;
                return;
            }
            else
            {
                i32 swapIndex = (m_numBlobs - 1);
                CopyOver(swapIndex, i);
                MarkFree(GetHeaderByIndexUnchecked(swapIndex));
                m_numBlobs--;
            }
        }
    }
};

static ErrorCode ZE_CreateBlobArray(
    ZEBlobArray** result, i32 capacity, i32 sizePerObject, i32 invalidId)
{
    i32 sizePerBlob = sizeof(ZEBlobHeader) + sizePerObject;
    i32 totalBytes = sizeof(ZEBlobArray) + (sizePerBlob * capacity);
    u8* mem = ZE_BA_MALLOC(totalBytes);
    if (mem == NULL) { return COM_ERROR_ALLOCATION_FAILED; }
    ZEBlobArray* arr = (ZEBlobArray*)mem;
    *arr = {};
    arr->m_blobs = mem + sizeof(ZEBlobArray);
    arr->m_end = mem + totalBytes;
    arr->m_blobUserSize = sizePerObject;
    arr->m_totalBlobSize = sizePerBlob;
    arr->m_numBlobs = 0;
    arr->m_maxBlobs = capacity;
    arr->m_invalidId = invalidId;
    for (i32 i = 0; i < arr->m_maxBlobs; ++i)
    {
        ZEBlobHeader* h = arr->GetHeaderByIndexUnchecked(i);
        h->id = invalidId;
        h->valid = NO;
        h->sentinel = 0;
    }
    *result = arr;
    return COM_ERROR_NONE;
}

#endif // ZE_BLOB_ARRAY_H