#ifndef ZE_BLOB_ARRAY_H
#define ZE_BLOB_ARRAY_H

#include "../common/common.h"
#include "string.h"

#define ZE_BA_INVALID_INDEX -1
#define ZE_BA_INVALID_ID 0

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

struct ZEBlobHeader
{
    i32 id;
    // if marked as invalid, this is an 'empty' slot but not available for reassignment yet
    i32 invalid;
    // TODO: sentinel is not checked as when GetByIndex runs for a free blob,
    // the sentinel is not set yet!
    i32 sentinel;
};

struct ZEBlobArray
{
    u8* m_blobs;
    u8* m_end;
    i32 m_blobUserSize;
    i32 m_maxBlobs;
    i32 m_numBlobs;

    ZEBlobHeader* GetByIndex(i32 index)
    {
        i32 offset = m_blobUserSize * index;
        ZEBlobHeader* result = (ZEBlobHeader*)(m_blobs + offset);
        COM_ASSERT(result->sentinel == COM_SENTINEL, "Blob GetByIndex sentinel failed")
        return result;
    }

    ErrorCode GetFree(ZEBlobHeader** result, i32 id)
    {
        if (result == NULL) { return COM_ERROR_NULL_ARGUMENT; }
        if (id == ZE_BA_INVALID_ID) { return COM_ERROR_BAD_ARGUMENT; }
        if (m_numBlobs == m_maxBlobs) { return COM_ERROR_NO_SPACE; }

        i32 newBlobIndex = m_numBlobs++;

        i32 offset = m_blobUserSize * newBlobIndex;
        ZEBlobHeader* header = (ZEBlobHeader*)(m_blobs + offset);
        header->id = id;
        header->sentinel = COM_SENTINEL;
        header->invalid = NO;
        *result = header;
        return COM_ERROR_NONE;
    }

    ErrorCode MarkForFree(i32 index)
    {
        if (index >= m_numBlobs) { return COM_ERROR_OUT_OF_BOUNDS; }
        ZEBlobHeader* header = GetByIndex(index);
        if (header == NULL) { return COM_ERROR_NOT_FOUND; }
        header->invalid = YES;
        return COM_ERROR_NONE;
    }

    void CopyOver(i32 sourceIndex, i32 destIndex)
    {
        u8* sourcePtr = m_blobs + (m_blobUserSize * sourceIndex);
        u8* destPtr = m_blobs + (m_blobUserSize * destIndex);
        ZE_BA_COPY(destPtr, sourcePtr, m_blobUserSize)
    }
    
    void Truncate()
    {
        i32 i = 0;
        while (i < m_numBlobs)
        {
            // if blob is invalid, replace it with the last in the array
            // and reduce the blob count
            ZEBlobHeader* blob = GetByIndex(i);
            if (blob->invalid == YES)
            {
                if (i == (m_numBlobs - 1))
                {
                    // no blob to replace this with, just clear list
                    m_numBlobs--;
                }
                else
                {
                    CopyOver(i, (m_numBlobs - 1));
                    m_numBlobs--;
                }
            }
            i++;
        }
    }
};

static ErrorCode ZE_CreateBlobArray(ZEBlobArray** result, i32 capacity, i32 sizePerBlob)
{
    i32 totalBytes = sizeof(ZEBlobArray) + (sizePerBlob * capacity);
    u8* mem = ZE_BA_MALLOC(totalBytes);
    if (mem == NULL) { return COM_ERROR_ALLOCATION_FAILED; }
    ZEBlobArray* arr = (ZEBlobArray*)mem;
    *arr = {};
    arr->m_blobs = mem + sizeof(ZEBlobArray);
    arr->m_end = mem + totalBytes;
    arr->m_blobUserSize = sizePerBlob;
    arr->m_numBlobs = 0;
    arr->m_maxBlobs = capacity;
    *result = arr;
    return COM_ERROR_NONE;
}

#endif // ZE_BLOB_ARRAY_H